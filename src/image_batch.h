
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
std::vector<size_t>      _rowMin          ;
std::vector<size_t>      _rowMax          ;
std::vector<size_t>      _colMin          ;
std::vector<size_t>      _colMax          ;
std::vector<size_t>      _phaseMin        ;
std::vector<size_t>      _phaseMax        ;
std::vector<size_t>      _maskMin         ;
std::vector<size_t>      _maskMax         ;
size_t                   _N        = 0    ;
size_t                   _nset     = 2    ;
size_t                   _ndim     = 2    ;
size_t                   _nmsk     = 3    ;
bool                     _periodic = true ;
bool                     _zeropad  = false;
std::string              _stat     = ""   ;
std::string              _mode     = ""   ;
std::vector<size_t>      _roi             ;
std::vector<std::string> _dtype           ;

public:

// (copy) constructor
// ------------------

Files8_2D (const Files8_2D &) = default;

Files8_2D& operator=(const Files8_2D &) = default;

Files8_2D(){};

// explicit constructor
// --------------------

Files8_2D( size_t N, std::string stat="",\
  std::vector<size_t> roi=std::vector<size_t>(),\
  std::vector<std::string> dtype=std::vector<std::string>(),\
  bool zeropad=false, bool periodic=true,\
  std::string mode="Bresenham" )
{
  this->reserve(N);

  _periodic = periodic;
  _zeropad  = zeropad;
  _stat     = stat;
  _mode     = mode;

  for ( size_t i=0 ; i<roi  .size() ; i++ ) { _roi  [i] = roi  [i]; }
  for ( size_t i=0 ; i<dtype.size() ; i++ ) { _dtype[i] = dtype[i]; }
};

// reserve a given size
// --------------------

void reserve ( size_t N )
{
  _N = N;
  while ( _roi     .size()<_ndim         ) { _roi     .push_back(0);  }
  while ( _dtype   .size()<_nset         ) { _dtype   .push_back(""); }
  while ( _fname   .size()<_nset*N       ) { _fname   .push_back(""); }
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
  _N        = 0;
  _periodic = true;
  _zeropad  = false;
  _stat     = "";
  _mode     = "";
  for ( size_t i=0 ; i<_roi     .size() ; i++ ) { _roi     [i] = 0;  }
  for ( size_t i=0 ; i<_dtype   .size() ; i++ ) { _dtype   [i] = ""; }
  for ( size_t i=0 ; i<_fname   .size() ; i++ ) { _fname   [i] = ""; }
  for ( size_t i=0 ; i<_rowMin  .size() ; i++ ) { _rowMin  [i] = 0;  }
  for ( size_t i=0 ; i<_rowMax  .size() ; i++ ) { _rowMax  [i] = 0;  }
  for ( size_t i=0 ; i<_colMin  .size() ; i++ ) { _colMin  [i] = 0;  }
  for ( size_t i=0 ; i<_colMax  .size() ; i++ ) { _colMax  [i] = 0;  }
  for ( size_t i=0 ; i<_phaseMin.size() ; i++ ) { _phaseMin[i] = 0;  }
  for ( size_t i=0 ; i<_phaseMax.size() ; i++ ) { _phaseMax[i] = 0;  }
  for ( size_t i=0 ; i<_maskMin .size() ; i++ ) { _maskMin [i] = 0;  }
  for ( size_t i=0 ; i<_maskMax .size() ; i++ ) { _maskMax [i] = 0;  }
}

// insert information (no bounds-check is performed)
// -------------------------------------------------

void set_roi ( std::vector<size_t> in ) { for ( size_t i=0 ; i<_ndim ; i++ ) { _roi[i] = in[i]; } };

void set_periodic (                                    bool   in ) { _periodic                               = in; };
void set_zeropad  (                                    bool   in ) { _zeropad                                = in; };
void set_stat     (                               std::string in ) { _stat                                   = in; };
void set_mode     (                               std::string in ) { _mode                                   = in; };
void set_dtype    ( size_t set,                   std::string in ) { _dtype   [               set          ] = in; };
void set_fname    ( size_t set, size_t i,         std::string in ) { _fname   [i*_nset       +set          ] = in; };
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

std::vector<size_t> roi ( void ) { return _roi; };

bool        periodic ( void                              ) { return _periodic;                               };
bool        zeropad  ( void                              ) { return _zeropad ;                               };
std::string stat     ( void                              ) { return _stat    ;                               };
std::string mode     ( void                              ) { return _mode    ;                               };
std::string dtype    ( size_t set                        ) { return _dtype   [               set          ]; };
std::string fname    ( size_t set, size_t i              ) { return _fname   [i*_nset       +set          ]; };
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

}; // class Files8_2D

}; // namespace ImageBatch

#endif
