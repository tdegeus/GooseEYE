/* =================================================================================================

(c - GPLv3) T.W.J. de Geus (Tom) | tom@geus.me | www.geus.me | github.com/tdegeus/GooseEYE

================================================================================================= */

#ifndef GOOSEEYE_CLUSTERS_HPP
#define GOOSEEYE_CLUSTERS_HPP

// =================================================================================================

#include "GooseEYE.h"

// =================================================================================================

namespace GooseEYE {

// =================================================================================================
// core functions: called by wrappers below
// =================================================================================================

namespace Private {

// -------------------------------------------------------------------------------------------------

void _link ( std::vector<int> &linked, int a, int b )
{

  if ( a==b )
    return;

  int i,c;

  if ( a>b ) {
    c = a;
    a = b;
    b = c;
  }

  // both unlinked
  // -------------

  if ( linked[a]==a && linked[b]==b ) {
    linked[a] = b;
    linked[b] = a;
    return;
  }

  // a linked / b unlinked
  // ---------------------

  if ( linked[a]!=a && linked[b]==b ) {
    linked[b] = linked[a];
    linked[a] = b;
    return;
  }

  // a unlinked / b linked
  // ---------------------

  if ( linked[a]==a && linked[b]!=b ) {
    linked[a] = linked[b];
    linked[b] = a;
    return;
  }

  // both linked -> to each other
  // ----------------------------

  i = a;
  while ( 1 ) {
    if ( linked[i]==b ) { return; }
    i = linked[i];
    if ( i==a ) { break; }
  }

  // both linked -> to different groups
  // ----------------------------------

  c = linked[a];
  linked[a] = b;

  i = a;
  while ( 1 ) {
    i = linked[i];
    if ( linked[i]==b ) { break; }
  }
  linked[i] = c;

  return;

}

// -------------------------------------------------------------------------------------------------

std::tuple<ArrI,ArrI> clusters(ArrI f, ArrI kern, int min_size, bool periodic)
{
  int h,i,j,di,dj,dh,H,I,J,lH,lI,lJ,uH,uI,uJ,dI,dJ,dH,ii,jj,ilab,nlab;

  // cluster links (e.g. 4 coupled to 2: lnk[4]=2)
  std::vector<int> lnk(f.size());

  // saved clusters: 1=saved, 0=not-saved
  std::vector<int> inc(f.size());

  // zero-initialize result
  ArrI l = ArrI::Zero(f.shape());
  ArrI c = ArrI::Zero(f.shape());

  // apply periodicity
  f   .setPeriodic(periodic);
  l   .setPeriodic(periodic);
  c   .setPeriodic(periodic);
  kern.setPeriodic(periodic);

  // change rank to 3 (to simplify implementation)
  f   .chrank(3);
  kern.chrank(3);

  // get half shape of the kernel
  VecS mid = kern.shape();

  // check
  for ( auto &i : mid )
    if ( i%2 == 0 )
      throw std::domain_error("'kernel' must be odd shaped");

  // midpoint
  for ( auto &i : mid ) i = (i-1)/2;

  // get shape
  H  = f.shape(0);
  I  = f.shape(1);
  J  = f.shape(2);
  dH = mid[0];
  dI = mid[1];
  dJ = mid[2];

  // ---------------
  // basic labelling
  // ---------------

  // current label
  ilab = 0;

  // initialize cluster-links (only coupled to self)
  for ( i=0 ; i<(int)f.size() ; i++ )
    lnk[i] = i;
  // initialize: no saved clusters, except background (inc[0]=1)
  inc[0] = 1;
  for ( size_t i=1 ; i<f.size() ; i++ )
    inc[i] = 0;

  // periodic: lower/upper bound of the kernel always == (shape[i]-1)/2
  if ( periodic ) {
    lH = -dH; uH = +dH;
    lI = -dI; uI = +dI;
    lJ = -dJ; uJ = +dJ;
  }

  // loop through voxels (in all directions)
  for ( h=0 ; h<H ; h++ ) {
    for ( i=0 ; i<I ; i++ ) {
      for ( j=0 ; j<J ; j++ ) {

        // only continue for non-zero voxels
        if ( f(h,i,j) ) {

          // set lower/upper bound of the kernel near edges
          // -> avoids reading out-of-bounds
          if ( !periodic ) {
            if ( h <    dH ) lH=0; else lH=-dH;
            if ( i <    dI ) lI=0; else lI=-dI;
            if ( j <    dJ ) lJ=0; else lJ=-dJ;
            if ( h >= H-dH ) uH=0; else uH=+dH;
            if ( i >= I-dI ) uJ=0; else uJ=+dI;
            if ( j >= J-dJ ) uI=0; else uI=+dJ;
          }

          // cluster not yet labelled: try to couple to labelled neighbours
          if ( l(h,i,j)==0 ) {
            for ( dh=lH ; dh<=uH ; dh++ ) {
              for ( di=lI ; di<=uJ ; di++ ) {
                for ( dj=lJ ; dj<=uI ; dj++ ) {
                  if ( kern(dh+dH,di+dI,dj+dJ) ) {
                    if ( l(h+dh,i+di,j+dj) ) {
                      l(h,i,j) = l(h+dh,i+di,j+dj);
                      goto end;
                    }
          }}}}}
          end: ;

          // cluster not yet labelled: create new label
          if ( l(h,i,j)==0 ) {
            ilab++;
            l(h,i,j)  = ilab;
            inc[ilab] = 1;
          }

          // try to couple neighbours to current label
          // - not yet labelled -> label neighbour
          // - labelled         -> link labels
          for ( dh=lH ; dh<=uH ; dh++ ) {
            for ( di=lI ; di<=uJ ; di++ ) {
              for ( dj=lJ ; dj<=uI ; dj++ ) {
                if ( kern(dh+dH,di+dI,dj+dJ) ) {
                  if ( f(h+dh,i+di,j+dj) ) {
                    if ( l(h+dh,i+di,j+dj)==0 )
                     l(h+dh,i+di,j+dj) = l(h,i,j);
                    else
                     _link(lnk,l(h,i,j),l(h+dh,i+di,j+dj));
          }}}}}

        }

      }
    }
  }

  // ---------------------------------------
  // renumber labels: all links to one label
  // ---------------------------------------

  nlab = 0;

  // loop through assigned labels -> loop through all coupled links
  // lnk[i] will contain the new label number
  for ( i=0 ; i<=ilab ; i++ ) {
    if ( inc[i] ) {
      ii = i;
      while ( 1 ) {
        jj      = lnk[ii];
        lnk[ii] = nlab;
        inc[ii] = 0;
        if ( jj==i ) { break; }
        ii      = jj;
      }
      nlab++;
    }
  }
  // apply renumbering
  for ( size_t i=0 ; i<f.size() ; i++ )
    l[i] = lnk[l[i]];

  // --------------------------
  // threshold for cluster size
  // --------------------------

  if ( min_size>0 ) {

    // find the size of all clusters
    for ( i=0 ; i<nlab ; i++ ) {
      lnk[i] = 0;    // now: size of the cluster with the label
      inc[i] = 0;    // now: included label (true/false)
    }
    for ( size_t i=0 ; i<l.size() ; i++ ) {
      lnk[l[i]]++;
      inc[l[i]] = 1;
    }
    // remove clusters with too small size
    for ( i=0 ; i<nlab ; i++ )
      if ( lnk[i]<min_size )
        inc[i] = 0;

    // remove clusters with too small size
    for ( size_t i=0 ; i<l.size() ; i++ )
      if ( !inc[l[i]] )
        l[i] = 0;

    // new numbering for the included labels
    // inc[i] will contain the new cluster numbers,
    // j-1 will be the number of clusters
    j = 0;
    for ( i=0 ; i<nlab ; i++ ) {
      if ( inc[i] ) {
        inc[i] = j;
        j++;
      }
    }
    nlab = j;

    // renumber the labels
    for ( size_t i=0 ; i<l.size() ; i++ )
      l[i] = inc[l[i]];

  }

  // cluster centres: not periodic
  // -----------------------------

  if ( !periodic )
  {
    // number of labels
    nlab = l.max()+1;

    // allocate matrix to contain the average position and total size:
    // [ [ h,i,j , size_feature ] , ... ]
    MatI x = MatI::Zero((size_t)nlab, 4);

    // loop through the image to update the position and size of each label
    for ( h=0 ; h<H ; h++ ) {
      for ( i=0 ; i<I ; i++ ) {
        for ( j=0 ; j<J ; j++ ) {
          ilab = l(h,i,j);
          if ( ilab>0 ) {
            x(ilab,0) += h;
            x(ilab,1) += i;
            x(ilab,2) += j;
            x(ilab,3) += 1;
          }
        }
      }
    }

    // fill the centres of gravity
    for ( ilab=1 ; ilab<nlab ; ilab++ ) {
      if ( x(ilab,3)>0 ) {

        h = (int)round( (float)x(ilab,0) / (float)x(ilab,3) );
        i = (int)round( (float)x(ilab,1) / (float)x(ilab,3) );
        j = (int)round( (float)x(ilab,2) / (float)x(ilab,3) );

        if ( h <  0 ) { h = 0; }
        if ( i <  0 ) { i = 0; }
        if ( j <  0 ) { j = 0; }
        if ( h >= H ) { h = H; }
        if ( i >= I ) { i = I; }
        if ( j >= J ) { j = J; }

        c(h,i,j) = ilab;
      }
    }
  }

  // cluster centres: periodic
  // -------------------------

  if ( periodic )
  {
    int h,i,j,dh,di,dj,ilab,nlab,nlab_;

    // "l_": labels for the non-periodic version image "f"
    ArrI l_,c_;
    std::tie(l_,c_) = clusters(f,kern,min_size,false);

    // delete clusters that are also not present in "l"
    for ( size_t i=0 ; i<f.size() ; i++ )
      if ( f[i] )
        if ( !l[i] )
          l_[i] = 0;

    // number of labels
    nlab  = l .max()+1;
    nlab_ = l_.max()+1;

    // matrix to contain the average position and total size:
    // [ [ h,i,j , size_feature ] , ... ]
    MatI  x = MatI::Zero((size_t)nlab ,4);
    // if dx(ilab,ix)==1 than N[ix] is subtracted from the position in averaging
    MatI dx = MatI::Zero((size_t)nlab_,3);

    // label dependency: which labels in "l_" correspond to which labels in "l"
    std::vector<int> lnk(nlab_);

    for ( size_t i=0 ; i<l.size() ; i++ )
      lnk[l_[i]] = l[i];

    // check periodicity: to which sides the clusters are connected
    // ------------------------------------------------------------

    // i-j plane
    for ( i=0 ; i<I ; i++ )
      for ( j=0 ; j<J ; j++ )
        for ( dh=1 ; dh<=dH ; dh++ )
          for ( di=0 ; di<=dI ; di++ )
            for ( dj=0 ; dj<=dJ ; dj++ )
              if ( i+di<I && j+dj<J )
                if ( l_(H-1,i,j) )
                  if ( l(H-1,i,j)==l(H-1+dh,i+di,j+dj) )
                    dx(l_(H-1,i,j),0) = 1;

    // h-j plane
    for ( h=0 ; h<H ; h++ )
      for ( j=0 ; j<J ; j++ )
        for ( dh=0 ; dh<=dH ; dh++ )
          for ( di=1 ; di<=dI ; di++ )
            for ( dj=0 ; dj<=dJ ; dj++ )
              if ( h+dh<H && j+dj<J )
                if ( l_(h,I-1,j) )
                  if ( l(h,I-1,j)==l(h+dh,I-1+di,j+dj) )
                    dx(l_(h,I-1,j),1) = 1;

    // h-i plane
    for ( h=0 ; h<H ; h++ )
      for ( i=0 ; j<I ; j++ )
        for ( dh=0 ; dh<=dH ; dh++ )
          for ( di=0 ; di<=dI ; di++ )
            for ( dj=1 ; dj<=dJ ; dj++ )
              if ( h+dh<H && i+di<I )
                if ( l_(h,i,J-1) )
                  if ( l(h,i,J-1)==l(h+dh,i+di,J-1+dj) )
                    dx(l_(h,i,J-1),2) = 1;

    // calculate centres
    // -----------------

    // loop through the image to update the position and size of each label
    for ( h=0 ; h<H ; h++ ) {
      for ( i=0 ; i<I ; i++ ) {
        for ( j=0 ; j<J ; j++ ) {
          ilab = l_(h,i,j);
          if ( ilab>0 ) {
            if ( dx(ilab,0) ) dh = -H; else dh = 0;
            if ( dx(ilab,1) ) di = -I; else di = 0;
            if ( dx(ilab,2) ) dj = -J; else dj = 0;

            x(lnk[ilab],0) += h+dh;
            x(lnk[ilab],1) += i+di;
            x(lnk[ilab],2) += j+dj;
            x(lnk[ilab],3) += 1;
          }
        }
      }
    }

    // fill the centres of gravity
    for ( ilab=1 ; ilab<nlab ; ilab++ ) {
      if ( x(ilab,3)>0 ) {

        h = (int)round( (float)x(ilab,0) / (float)x(ilab,3) );
        i = (int)round( (float)x(ilab,1) / (float)x(ilab,3) );
        j = (int)round( (float)x(ilab,2) / (float)x(ilab,3) );

        while ( h <  0 ) { h += H; }
        while ( i <  0 ) { i += I; }
        while ( j <  0 ) { j += J; }
        while ( h >= H ) { h -= H; }
        while ( i >= I ) { i -= I; }
        while ( j >= J ) { j -= J; }

        c(h,i,j) = ilab;
      }
    }
  }

  return std::make_tuple(l,c);
}

// -------------------------------------------------------------------------------------------------

} // namespace Private

// =================================================================================================
// wrapper functions
// =================================================================================================

ArrI clusters(const ArrI &f, const ArrI &kern, int min_size, bool periodic)
{
  ArrI clus, cent;

  std::tie(clus, cent) = Private::clusters(f, kern, min_size, periodic);

  return clus;
}

// -------------------------------------------------------------------------------------------------

ArrI clusters(const ArrI &f, int min_size, bool periodic)
{
  ArrI clus, cent;

  std::tie(clus, cent) = Private::clusters(f, kernel(f.rank()), min_size, periodic);

  return clus;
}

// -------------------------------------------------------------------------------------------------

ArrI clusters(const ArrI &f, bool periodic)
{
  ArrI clus, cent;

  std::tie(clus, cent) = Private::clusters(f, kernel(f.rank()), 0, periodic);

  return clus;
}

// -------------------------------------------------------------------------------------------------

std::tuple<ArrI,ArrI> clusterCenters(const ArrI &f, const ArrI &kern, int min_size, bool periodic)
{
  return Private::clusters(f, kern, min_size, periodic);
}

// -------------------------------------------------------------------------------------------------

std::tuple<ArrI,ArrI> clusterCenters(const ArrI &f, int min_size, bool periodic)
{
  return Private::clusters(f, kernel(f.rank()), min_size, periodic);
}

// -------------------------------------------------------------------------------------------------

std::tuple<ArrI,ArrI> clusterCenters(const ArrI &f, bool periodic)
{
  return Private::clusters(f, kernel(f.rank()), 0, periodic);
}

// =================================================================================================

} // namespace ...

// =================================================================================================

#endif
