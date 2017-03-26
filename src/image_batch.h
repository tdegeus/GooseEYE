
#ifndef IMAGE_BATCH_H
#define IMAGE_BATCH_H

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <string>

#include "image.h"

namespace ImageBatch {

class Files8_2D
{

private:

std::vector<std::string> _fname           ;
std::vector<size_t>      _saved           ;
std::vector<size_t>      _rowMin          ;
std::vector<size_t>      _rowMax          ;
std::vector<size_t>      _colMin          ;
std::vector<size_t>      _colMax          ;
std::vector<size_t>      _phaseMin        ;
std::vector<size_t>      _phaseMax        ;
std::vector<size_t>      _maskMin         ;
std::vector<size_t>      _maskMax         ;
size_t                   _nset     = 2    ;
size_t                   _ndim     = 2    ;
size_t                   _nmsk     = 3    ;
bool                     _periodic = true ;
bool                     _zeropad  = false;
std::string              _stat     = ""   ;
std::string              _mode     = ""   ;
std::vector<size_t>      _roi             ;
std::vector<size_t>      _N               ;
std::vector<std::string> _dtype           ;
std::vector<std::string> _phase           ;
Image::Matrix<double>    _result          ;
double                   _norm            ;

public:

// (copy) constructor
// ------------------

Files8_2D (const Files8_2D &) = default;

Files8_2D& operator=(const Files8_2D &) = default;

Files8_2D(){};

Files8_2D( size_t N ) { this->reserve(N); };

// reserve a given size
// --------------------

void reserve ( size_t N )
{
  while ( _roi     .size()<_ndim         ) { _roi     .push_back(0);  }
  while ( _N       .size()<_nset         ) { _N       .push_back(0);  }
  while ( _dtype   .size()<_nset         ) { _dtype   .push_back(""); }
  while ( _phase   .size()<_nset         ) { _phase   .push_back(""); }
  while ( _fname   .size()<_nset*N       ) { _fname   .push_back(""); }
  while ( _saved   .size()<_nset*N       ) { _saved   .push_back(0);  }
  while ( _rowMin  .size()<_nset*N       ) { _rowMin  .push_back(0);  }
  while ( _rowMax  .size()<_nset*N       ) { _rowMax  .push_back(0);  }
  while ( _colMin  .size()<_nset*N       ) { _colMin  .push_back(0);  }
  while ( _colMax  .size()<_nset*N       ) { _colMax  .push_back(0);  }
  while ( _phaseMin.size()<_nset*N       ) { _phaseMin.push_back(0);  }
  while ( _phaseMax.size()<_nset*N       ) { _phaseMax.push_back(0);  }
  while ( _maskMin .size()<_nset*N*_nmsk ) { _maskMin .push_back(0);  }
  while ( _maskMax .size()<_nset*N*_nmsk ) { _maskMax .push_back(0);  }
};

// clear entries
// -------------

void clear ( void )
{
  _periodic = true;
  _zeropad  = false;
  _stat     = "";
  _mode     = "";
  for ( size_t i=0 ; i<_roi     .size() ; i++ ) { _roi     [i] = 0;  }
  for ( size_t i=0 ; i<_N       .size() ; i++ ) { _N       [i] = 0;  }
  for ( size_t i=0 ; i<_dtype   .size() ; i++ ) { _dtype   [i] = ""; }
  for ( size_t i=0 ; i<_phase   .size() ; i++ ) { _phase   [i] = ""; }
  for ( size_t i=0 ; i<_fname   .size() ; i++ ) { _fname   [i] = ""; }
  for ( size_t i=0 ; i<_saved   .size() ; i++ ) { _saved   [i] = 0;  }
  for ( size_t i=0 ; i<_rowMin  .size() ; i++ ) { _rowMin  [i] = 0;  }
  for ( size_t i=0 ; i<_rowMax  .size() ; i++ ) { _rowMax  [i] = 0;  }
  for ( size_t i=0 ; i<_colMin  .size() ; i++ ) { _colMin  [i] = 0;  }
  for ( size_t i=0 ; i<_colMax  .size() ; i++ ) { _colMax  [i] = 0;  }
  for ( size_t i=0 ; i<_phaseMin.size() ; i++ ) { _phaseMin[i] = 0;  }
  for ( size_t i=0 ; i<_phaseMax.size() ; i++ ) { _phaseMax[i] = 0;  }
  for ( size_t i=0 ; i<_maskMin .size() ; i++ ) { _maskMin [i] = 0;  }
  for ( size_t i=0 ; i<_maskMax .size() ; i++ ) { _maskMax [i] = 0;  }
}

// add empty item
// --------------

void itemAdd( size_t set , std::string fname="" )
{
  this->reserve(0);
  _N[set]++;
  this->reserve(_N[set]);
  _fname[(_N[set]-1)*_nset+set] = fname;
}

// remove item
// -----------

void itemRm ( size_t set, size_t idx )
{
  for ( size_t i=idx+1 ; i<_N[set] ; i++ ) {

    _fname   [(i-1)*_nset+set] = _fname   [i*_nset+set];
    _saved   [(i-1)*_nset+set] = _saved   [i*_nset+set];
    _rowMin  [(i-1)*_nset+set] = _rowMin  [i*_nset+set];
    _rowMax  [(i-1)*_nset+set] = _rowMax  [i*_nset+set];
    _colMin  [(i-1)*_nset+set] = _colMin  [i*_nset+set];
    _colMax  [(i-1)*_nset+set] = _colMax  [i*_nset+set];
    _phaseMin[(i-1)*_nset+set] = _phaseMin[i*_nset+set];
    _phaseMax[(i-1)*_nset+set] = _phaseMax[i*_nset+set];

    for ( size_t imsk=0 ; imsk<_nmsk ; imsk++ ) {
      _maskMin[(i-1)*_nset*_nmsk+set*_nmsk+imsk] = _maskMin[i*_nset*_nmsk+set*_nmsk+imsk];
      _maskMax[(i-1)*_nset*_nmsk+set*_nmsk+imsk] = _maskMax[i*_nset*_nmsk+set*_nmsk+imsk];
    }

  }

  _N[set]--;
}

// move item up
// ------------

void itemUp ( size_t set, size_t i )
{
  if ( i<=0 )
    return;

  std::string fname   ;
  size_t      saved   ;
  size_t      rowMin  ;
  size_t      rowMax  ;
  size_t      colMin  ;
  size_t      colMax  ;
  size_t      phaseMin;
  size_t      phaseMax;
  std::vector<size_t> maskMin (_nmsk);
  std::vector<size_t> maskMax (_nmsk);

  fname    = _fname   [i*_nset+set];
  saved    = _saved   [i*_nset+set];
  rowMin   = _rowMin  [i*_nset+set];
  rowMax   = _rowMax  [i*_nset+set];
  colMin   = _colMin  [i*_nset+set];
  colMax   = _colMax  [i*_nset+set];
  phaseMin = _phaseMin[i*_nset+set];
  phaseMax = _phaseMax[i*_nset+set];

  for ( size_t imsk=0 ; imsk<_nmsk ; imsk++ ) {
    maskMin[set*_nmsk+imsk] = _maskMin[i*_nset*_nmsk+set*_nmsk+imsk];
    maskMax[set*_nmsk+imsk] = _maskMax[i*_nset*_nmsk+set*_nmsk+imsk];
  }

  _fname   [i*_nset+set] = _fname   [(i-1)*_nset+set];
  _saved   [i*_nset+set] = _saved   [(i-1)*_nset+set];
  _rowMin  [i*_nset+set] = _rowMin  [(i-1)*_nset+set];
  _rowMax  [i*_nset+set] = _rowMax  [(i-1)*_nset+set];
  _colMin  [i*_nset+set] = _colMin  [(i-1)*_nset+set];
  _colMax  [i*_nset+set] = _colMax  [(i-1)*_nset+set];
  _phaseMin[i*_nset+set] = _phaseMin[(i-1)*_nset+set];
  _phaseMax[i*_nset+set] = _phaseMax[(i-1)*_nset+set];

  for ( size_t imsk=0 ; imsk<_nmsk ; imsk++ ) {
    _maskMin[i*_nset*_nmsk+set*_nmsk+imsk] = _maskMin[(i-1)*_nset*_nmsk+set*_nmsk+imsk];
    _maskMax[i*_nset*_nmsk+set*_nmsk+imsk] = _maskMax[(i-1)*_nset*_nmsk+set*_nmsk+imsk];
  }

  _fname   [(i-1)*_nset+set] = fname   ;
  _saved   [(i-1)*_nset+set] = saved   ;
  _rowMin  [(i-1)*_nset+set] = rowMin  ;
  _rowMax  [(i-1)*_nset+set] = rowMax  ;
  _colMin  [(i-1)*_nset+set] = colMin  ;
  _colMax  [(i-1)*_nset+set] = colMax  ;
  _phaseMin[(i-1)*_nset+set] = phaseMin;
  _phaseMax[(i-1)*_nset+set] = phaseMax;

  for ( size_t imsk=0 ; imsk<_nmsk ; imsk++ ) {
    _maskMin[(i-1)*_nset*_nmsk+set*_nmsk+imsk] = maskMin[set*_nmsk+imsk];
    _maskMax[(i-1)*_nset*_nmsk+set*_nmsk+imsk] = maskMax[set*_nmsk+imsk];
  }

}

// move item down
// --------------

void itemDown ( size_t set, size_t i )
{
  if ( i>=_N[set]-1 )
    return;

  std::string fname   ;
  size_t      saved   ;
  size_t      rowMin  ;
  size_t      rowMax  ;
  size_t      colMin  ;
  size_t      colMax  ;
  size_t      phaseMin;
  size_t      phaseMax;
  std::vector<size_t> maskMin (_nmsk);
  std::vector<size_t> maskMax (_nmsk);

  for ( size_t set=0 ; set<_nset ; set++ ) {

    fname    = _fname   [i*_nset+set];
    saved    = _saved   [i*_nset+set];
    rowMin   = _rowMin  [i*_nset+set];
    rowMax   = _rowMax  [i*_nset+set];
    colMin   = _colMin  [i*_nset+set];
    colMax   = _colMax  [i*_nset+set];
    phaseMin = _phaseMin[i*_nset+set];
    phaseMax = _phaseMax[i*_nset+set];

    for ( size_t imsk=0 ; imsk<_nmsk ; imsk++ ) {
      maskMin[set*_nmsk+imsk] = _maskMin[i*_nset*_nmsk+set*_nmsk+imsk];
      maskMax[set*_nmsk+imsk] = _maskMax[i*_nset*_nmsk+set*_nmsk+imsk];
    }

    _fname   [i*_nset+set] = _fname   [(i+1)*_nset+set];
    _saved   [i*_nset+set] = _saved   [(i+1)*_nset+set];
    _rowMin  [i*_nset+set] = _rowMin  [(i+1)*_nset+set];
    _rowMax  [i*_nset+set] = _rowMax  [(i+1)*_nset+set];
    _colMin  [i*_nset+set] = _colMin  [(i+1)*_nset+set];
    _colMax  [i*_nset+set] = _colMax  [(i+1)*_nset+set];
    _phaseMin[i*_nset+set] = _phaseMin[(i+1)*_nset+set];
    _phaseMax[i*_nset+set] = _phaseMax[(i+1)*_nset+set];

    for ( size_t imsk=0 ; imsk<_nmsk ; imsk++ ) {
      _maskMin[i*_nset*_nmsk+set*_nmsk+imsk] = _maskMin[(i+1)*_nset*_nmsk+set*_nmsk+imsk];
      _maskMax[i*_nset*_nmsk+set*_nmsk+imsk] = _maskMax[(i+1)*_nset*_nmsk+set*_nmsk+imsk];
    }

    _fname   [(i+1)*_nset+set] = fname   ;
    _saved   [(i+1)*_nset+set] = saved   ;
    _rowMin  [(i+1)*_nset+set] = rowMin  ;
    _rowMax  [(i+1)*_nset+set] = rowMax  ;
    _colMin  [(i+1)*_nset+set] = colMin  ;
    _colMax  [(i+1)*_nset+set] = colMax  ;
    _phaseMin[(i+1)*_nset+set] = phaseMin;
    _phaseMax[(i+1)*_nset+set] = phaseMax;

    for ( size_t imsk=0 ; imsk<_nmsk ; imsk++ ) {
      _maskMin[(i+1)*_nset*_nmsk+set*_nmsk+imsk] = maskMin[set*_nmsk+imsk];
      _maskMax[(i+1)*_nset*_nmsk+set*_nmsk+imsk] = maskMax[set*_nmsk+imsk];
    }

  }

}

// insert information (no bounds-check is performed)
// -------------------------------------------------

void set_roi ( std::vector<size_t> in ) { for ( size_t i=0 ; i<_ndim ; i++ ) { _roi[i] = in[i]; } };
void set_roi ( size_t i , size_t in ) { _roi[i] = in; };

void set_periodic (                                    bool   in ) { _periodic                               = in; };
void set_zeropad  (                                    bool   in ) { _zeropad                                = in; };
void set_stat     (                               std::string in ) { _stat                                   = in; };
void set_mode     (                               std::string in ) { _mode                                   = in; };
void set_dtype    ( size_t set,                   std::string in ) { _dtype   [               set          ] = in; };
void set_phase    ( size_t set,                   std::string in ) { _phase   [               set          ] = in; };
void set_fname    ( size_t set, size_t i,         std::string in ) { _fname   [i*_nset       +set          ] = in; };
void set_saved    ( size_t set, size_t i,              size_t in ) { _saved   [i*_nset       +set          ] = in; };
void set_rowMin   ( size_t set, size_t i,              size_t in ) { _rowMin  [i*_nset       +set          ] = in; };
void set_rowMax   ( size_t set, size_t i,              size_t in ) { _rowMax  [i*_nset       +set          ] = in; };
void set_colMin   ( size_t set, size_t i,              size_t in ) { _colMin  [i*_nset       +set          ] = in; };
void set_colMax   ( size_t set, size_t i,              size_t in ) { _colMax  [i*_nset       +set          ] = in; };
void set_phaseMin ( size_t set, size_t i,              size_t in ) { _phaseMin[i*_nset       +set          ] = in; };
void set_phaseMax ( size_t set, size_t i,              size_t in ) { _phaseMax[i*_nset       +set          ] = in; };
void set_maskMin  ( size_t set, size_t i, size_t imsk, size_t in ) { _maskMin [i*_nset*_nmsk+set*_nmsk+imsk] = in; };
void set_maskMax  ( size_t set, size_t i, size_t imsk, size_t in ) { _maskMax [i*_nset*_nmsk+set*_nmsk+imsk] = in; };

// return information (no bounds-check is performed)
// -------------------------------------------------

Image::Matrix<double> result ( void ) { return _result; }
double                norm   ( void ) { return _norm;   }

size_t count ( size_t set ) { return _N[set]; }

size_t nset  ( void ) { size_t i=0; while ( i+1<_nset && _dtype[i].size()!=0 ) i++; return i; }

std::vector<size_t> roi ( void ) { return _roi; };

bool        periodic ( void                              ) { return _periodic;                               };
bool        zeropad  ( void                              ) { return _zeropad ;                               };
std::string stat     ( void                              ) { return _stat    ;                               };
std::string mode     ( void                              ) { return _mode    ;                               };
std::string dtype    ( size_t set                        ) { return _dtype   [               set          ]; };
std::string phase    ( size_t set                        ) { return _phase   [               set          ]; };
std::string fname    ( size_t set, size_t i              ) { return _fname   [i*_nset       +set          ]; };
size_t      saved    ( size_t set, size_t i              ) { return _saved   [i*_nset       +set          ]; };
size_t      rowMin   ( size_t set, size_t i              ) { return _rowMin  [i*_nset       +set          ]; };
size_t      rowMax   ( size_t set, size_t i              ) { return _rowMax  [i*_nset       +set          ]; };
size_t      colMin   ( size_t set, size_t i              ) { return _colMin  [i*_nset       +set          ]; };
size_t      colMax   ( size_t set, size_t i              ) { return _colMax  [i*_nset       +set          ]; };
size_t      phaseMin ( size_t set, size_t i              ) { return _phaseMin[i*_nset       +set          ]; };
size_t      phaseMax ( size_t set, size_t i              ) { return _phaseMax[i*_nset       +set          ]; };
size_t      maskMin  ( size_t set, size_t i, size_t imsk ) { return _maskMin [i*_nset*_nmsk+set*_nmsk+imsk]; };
size_t      maskMax  ( size_t set, size_t i, size_t imsk ) { return _maskMax [i*_nset*_nmsk+set*_nmsk+imsk]; };

// return image and mask, based on settings (no bounds-check is performed)
// -----------------------------------------------------------------------

// (see overload function below)
std::tuple<Image::Matrix<int>,Image::Matrix<int>> image ( \
  size_t set, size_t idx, Image::Matrix<int> im, bool crop=true )
{
  size_t irow,jrow,icol,jcol;
  int    min,max;

  // set start/end rows/columns -> automatic crop below
  if ( crop ) {
    irow = _rowMin[idx*_nset+set];
    jrow = _rowMax[idx*_nset+set];
    icol = _colMin[idx*_nset+set];
    jcol = _colMax[idx*_nset+set];
  }
  else {
    irow = 0;
    jrow = im.shape()[0];
    icol = 0;
    jcol = im.shape()[1];
  }

  // allocate output
  Image::Matrix<int> out({jrow-irow,jcol-icol});
  Image::Matrix<int> msk({jrow-irow,jcol-icol});

  // phase threshold: read bounds from settings
  min = (int)_phaseMin[idx*_nset+set];
  max = (int)_phaseMax[idx*_nset+set];

  // phase threshold float: retain values; everything outside bounds is masked
  if ( _dtype[set]=="float" ) {
    for ( size_t i=0 ; i<(jrow-irow) ; i++ ) {
      for ( size_t j=0 ; j<(jcol-icol) ; j++ ) {
        if ( im(i+irow,j+icol)>=min && im(i+irow,j+icol)<=max )
          out(i,j) = im(i+irow,j+icol);
        else
          msk(i,j) = 1;
      }
    }
  }
  // phase threshold binary/int: between (min,max)->1, everything else ->0
  else if ( _dtype[set]=="binary" || _dtype[set]=="int" ) {
    for ( size_t i=0 ; i<(jrow-irow) ; i++ )
      for ( size_t j=0 ; j<(jcol-icol) ; j++ )
        if ( im(i+irow,j+icol)>=min && im(i+irow,j+icol)<=max )
          out(i,j) = 1;
  }

  // int: determine clusters from binary image
  if ( _dtype[set]=="int" ) {
    Image::Matrix<int> clusters,centers;
    std::tie(clusters,centers) = Image::clusters(out,0,_periodic);
    for ( size_t i=0 ; i<out.size() ; i++ )
      out[i] = clusters[i];
  }

  // mask threshold
  for ( size_t imsk=0 ; imsk<_nmsk ; imsk++ ) {
    min = (int)_maskMin[idx*_nset*_nmsk+set*_nmsk+imsk];
    max = (int)_maskMax[idx*_nset*_nmsk+set*_nmsk+imsk];
    if ( min!=max )
      for ( size_t i=0 ; i<(jrow-irow) ; i++ )
        for ( size_t j=0 ; j<(jcol-icol) ; j++ )
          if ( im(i+irow,j+icol)>=min && im(i+irow,j+icol)<=max )
            msk(i,j) = 1;
  }

  return std::make_tuple(out,msk);
}

// overload function, to supply function to read image from file
std::tuple<Image::Matrix<int>,Image::Matrix<int>> image ( \
  size_t set, size_t idx, Image::Matrix<int> (*func)(std::string),\
  bool crop=true )
{
  Image::Matrix<int> im = func(_fname[idx*_nset+set]);
  return this->image(set,idx,im,crop);
}

// perform computation: front end
// ------------------------------

void compute_S2 ( Image::Matrix<int> (*func)(std::string) )
{
  _result.resize(_roi);
  int n;
  int N = 0;
  Image::Matrix<double> tmp;
  Image::Matrix<int>    f,g,fmask,gmask;

  // loop over ensemble
  for ( size_t i=0 ; i<this->count(0) ; i++ ) {
    // load images
    if ( true            ) std::tie(f,fmask) = this->image(0,i,func);
    if ( this->nset()==2 ) std::tie(g,gmask) = this->image(1,i,func);
    else                   std::tie(g,gmask) = this->image(0,i,func);
    // compute correlation
    std::tie(tmp,n) = Image::S2(f,g,_roi);
    // add to ensemble average
    tmp     *= static_cast<double>(n);
    _result += tmp;
    N       += n;
  }
  // scale ensemble average result
  _result /= static_cast<double>(N);

}

void compute ( Image::Matrix<int> (*func)(std::string) )
{
  if     ( _stat=="S2" ) { this->compute_S2(func); }
}

}; // class Files8_2D

}; // namespace ImageBatch

#endif
