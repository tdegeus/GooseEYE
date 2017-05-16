
#ifndef BATCH_H
#define BATCH_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include "json/json.hpp"

using json = nlohmann::json;

// =================================================================================================
// Class to read a batch of 8-bit images (2-D) from files
//
// Methods:
// .write(std::string)      write to   JSON file
// .read (std::string)      read  from JSON file
// =================================================================================================

class Files
{

private:

public:

// -------------------------------------------------------------------------------------------------
// variables
// -------------------------------------------------------------------------------------------------

json data;

// -------------------------------------------------------------------------------------------------
// constructors
// -------------------------------------------------------------------------------------------------

Files (const Files &) = default;

Files& operator=(const Files &) = default;

Files(){};

// -------------------------------------------------------------------------------------------------
// write to file
// -------------------------------------------------------------------------------------------------

void write(std::string fname=std::string())
{
  if ( fname.size()==0 )
    fname = data["store_json"];

  std::ofstream o(fname);
  o << std::setw(2) << data << std::endl;
};

// -------------------------------------------------------------------------------------------------
// read from file
// -------------------------------------------------------------------------------------------------

void read(std::string fname=std::string())
{
  if ( fname.size()==0 )
    fname = data["store_json"];

  data.clear();

  std::ifstream i(fname);
  i >> data;
};

// -------------------------------------------------------------------------------------------------
// set defaults: avoids having do perform all kind of checks throughout the code
// -------------------------------------------------------------------------------------------------

void defaults()
{
  data["work_dir"        ] = "";
  data["store_json"      ] = "GooseEYE.json";
  data["store_pdf"       ] = "GooseEYE_result.pdf";
  data["store_pdf_interp"] = "GooseEYE_result_interpretation.pdf";
  data["stat"            ] = "S2";
  data["periodic"        ] = true;
  data["zeropad"         ] = false;
  data["mask_weight"     ] = true;
  data["pixel_path"      ] = "Bresenham";
  data["roi"             ] = {51,51};
  data["sets"            ] = {"set1","set1"};

  data["set1"]["dtype" ]   = "binary";
  data["set1"]["field" ]   = "phase";
  data["set1"]["files" ]   = {};
  data["set1"]["config"]   = {};
}

// -------------------------------------------------------------------------------------------------
// (read image) -> apply settings -> return image as matrix
// -------------------------------------------------------------------------------------------------

std::tuple<mat::matrix<int>,mat::matrix<int>> image (
  size_t set, size_t idx, mat::matrix<int> im, bool crop=true )
{
  // convert set index -> name (specified in the JSON file)
  bool        periodic = data["periodic"];
  std::string sname    = data["sets"    ][set     ];
  std::string fname    = data[sname     ]["files" ][idx  ];
  json        file     = data[sname     ]["config"][fname];
  std::string dtype    = data[sname     ]["dtype" ];

  // default shape of output matrix
  size_t irow = 0;
  size_t jrow = im.shape()[0];
  size_t icol = 0;
  size_t jcol = im.shape()[1];

  // default range of the output matrix
  size_t min  = 0;
  size_t max  = 255;

  // crop: modify dimensions of the output matrix
  if ( crop && file.count("row") ) {
    irow = file["row"][0];
    jrow = file["row"][1];
  }
  if ( crop && file.count("col") ) {
    icol = file["col"][0];
    jcol = file["col"][1];
  }

  // threshold: read range
  if ( file.count("phase") ) {
    min  = file["phase"][0];
    max  = file["phase"][1];
  }

  // allocate output
  mat::matrix<int> out({jrow-irow,jcol-icol}); out.zeros();
  mat::matrix<int> msk({jrow-irow,jcol-icol}); msk.zeros();

  // phase threshold float: retain values; everything outside bounds is masked
  if ( dtype=="float" ) {
    for ( size_t i=0; i<(jrow-irow); ++i ) {
      for ( size_t j=0; j<(jcol-icol); ++j ) {
        if ( im(i+irow,j+icol)>=min && im(i+irow,j+icol)<=max )
          out(i,j) = im(i+irow,j+icol);
        else
          msk(i,j) = 1;
      }
    }
  }
  // phase threshold binary/int: between (min,max)->1, else->0 (nothing masked)
  else {
    for ( size_t i=0; i<(jrow-irow); ++i )
      for ( size_t j=0; j<(jcol-icol); ++j )
        if ( im(i+irow,j+icol)>=min && im(i+irow,j+icol)<=max )
          out(i,j) = 1;
  }

  // int: determine clusters from binary image
  if ( dtype=="int" ) {
    mat::matrix<int> clusters,centers;
    std::tie(clusters,centers) = Image::clusters(out,periodic);
    for ( size_t i=0; i<out.size(); ++i )
      out[i] = clusters[i];
  }

  // mask threshold
  if ( file.count("mask") ) {
    if ( file["mask"].size()%2!=0 )
      throw std::runtime_error("masks must be specified min,max , min,max , ...");
    for ( size_t imsk=0; imsk<file["mask"].size(); imsk+=2 )
      for ( size_t i=0 ; i<(jrow-irow) ; i++ )
        for ( size_t j=0 ; j<(jcol-icol) ; j++ )
          if ( im(i+irow,j+icol)>=file["mask"][imsk] && im(i+irow,j+icol)<=file["mask"][imsk+1] )
            msk(i,j) = 1;
  }

  return std::make_tuple(out,msk);
};

// -------------------------------------------------------------------------------------------------

std::tuple<mat::matrix<int>,mat::matrix<int>> image (
  size_t set, size_t idx, mat::matrix<int> (*func)(std::string,std::string), bool crop=true )
{
  std::string      sname = data["sets"][idx];
  std::string      fname = data[sname ]["files"][idx];
  mat::matrix<int> im    = func(data["work_dir"],fname);
  return this->image(set,idx,im,crop);
};

// -------------------------------------------------------------------------------------------------

std::tuple<mat::matrix<double>,mat::matrix<double>,double,int> compute_S2(
  mat::matrix<int> (*func)(std::string,std::string)
)
{

}

// -------------------------------------------------------------------------------------------------

void compute ( mat::matrix<int> (*func)(std::string,std::string) )
{
  // read from data
  std::string         set1 = data["sets"][0];
  std::string         set2 = data["sets"][1];
  size_t              nrow = data["roi" ][0];
  size_t              ncol = data["roi" ][1];
  std::vector<size_t> roi  = {nrow,ncol};

  // check input
  // - number of input files
  if ( data[set1]["files"].size()!=data[set1]["files"].size() )
    throw std::runtime_error("Inconsistent input files");
  // - first set should be weight
  if ( data["stat"]=="W2" || data["stat"]=="W2c" )
    if ( data[set1]["field"]=="phase" && data[set2]["field"]=="weight" )
      data["sets"] = { set2 , set1 };

  // initialize ensemble averages
  mat::matrix<double> res(roi); res.zeros();
  mat::matrix<double> nor(roi); nor.zeros();
  double mean = 0.0;
  int    N    = 0;

  // allocate temporary data
  mat::matrix<double> ires;
  mat::matrix<double> inor;
  double imean;
  int    iN;

  // loop over images
  for ( size_t idx=0; idx<data[set1]["files"].size(); ++idx ) {

  }



}

// void compute ( mat::matrix<int> (*func)(std::string) )
// {
//   // read size of the ROI
//   size_t              nrow = data["roi"][0];
//   size_t              ncol = data["roi"][1];
//   std::vector<size_t> roi  = {nrow,ncol};
//   bool                f    = false;
//   bool                per  = data["periodic"];
//   bool                pad  = data["zeropad" ];
//   std::string         set1 = data["sets"][0];
//   std::string         set2 = data["sets"][1];
//   // // check
//   // if ( data["stat"]=="S2" ) {
//   //   if ( data[set1]["dtype"]!= )
//   // }
//   // // profile
//   // if      ( data[set1]["dtype"]=="float" && data[set2]["dtype"]=="float" ) {
//   //   f = true;
//   // }
//   // else if ( data[set1]["dtype"]=="float" || data[set2]["dtype"]=="float" )
//   //   throw std::runtime_error("Data-types are incompatible");
//   // if      ( data[set1]["dtype"]=="float" && data[set2]["dtype"]=="float" )
//   //   f = true;
//   // else if ( data[set1]["dtype"]=="float" || data[set2]["dtype"]=="float" )
//   //   throw std::runtime_error("Data-types are incompatible");

//   // initialize
//   mat::matrix<double> res (roi); res .zeros();
//   mat::matrix<double> norm(roi); norm.zeros();
//   mat::matrix<double> ir  (roi); ir  .zeros();
//   mat::matrix<double> in  (roi); in  .zeros();
//   mat::matrix<double> ones(roi); ones.ones ();
//   double ins  = 0.0;
//   double mean = 0.0;
//   double N    = 0;
//   double N1,N2;
//   int    M1,M2;
//   bool   msk;
//   mat::matrix<int>    i1,m1,i2,m2;
//   mat::matrix<double> d1,d2;
//   std::string         fname1,fname2;

//   // compute
//   for ( auto &file : data[set1]["files"] ) {
//     // - read names
//     fname1 = file["input"][0];
//     fname2 = file["input"][1];
//     // - read images
//     std::tie(i1,m1) = image(0,fname1,func);
//     std::tie(i2,m2) = image(1,fname2,func);
//     // - convert to float
//     if ( f ) {
//       d1 = i1; d1 /= 255.;
//       d2 = i2; d2 /= 255.;
//     }
//     // - initialize
//     M1  = m1.sum();
//     M2  = m2.sum();
//     msk = pad || !per || M1>0 || M2>0;
//     N1  = static_cast<double>(i1.size()-M1);
//     N2  = static_cast<double>(i2.size()-M2);
//     ins = 0.0;
//     in.zeros();
//     // - compute statistic
//     if      ( data["stat"]=="S2"  ) {
//       if      (  f &&  msk ) std::tie(ir,in ) = Image::S2(d1,d2,roi,m1,m2,pad,per);
//       else if ( !f &&  msk ) std::tie(ir,in ) = Image::S2(i1,i2,roi,m1,m2,pad,per);
//       else if (  f         ) std::tie(ir,ins) = Image::S2(d1,d2,roi);
//       else if ( !f         ) std::tie(ir,ins) = Image::S2(i1,i2,roi);
//     }
//     else if ( data["stat"]=="W2"  ) {
//       if      (  f &&  msk ) std::tie(ir,in ) = Image::W2(d1,d2,roi,m2,pad,per);
//       else if ( !f &&  msk ) std::tie(ir,in ) = Image::W2(i1,i2,roi,m2,pad,per);
//       else if (  f         ) std::tie(ir,ins) = Image::W2(d1,d2,roi);
//       else if ( !f         ) std::tie(ir,ins) = Image::W2(i1,i2,roi);
//     }
//     else if ( data["stat"]=="W2c" ) {
//     }
//     else if ( data["stat"]=="L"   ) {
//     }
//     else {
//       throw std::runtime_error("Unknown statistic");
//     }
//     // - add unnormalized mean to ensemble
//     if      (  f &&  msk ) { mean += Image::mean(d1,m1)*N1; mean += Image::mean(d2,m2)*N2; }
//     else if (  f && !msk ) { mean += Image::mean(d1   )*N1; mean += Image::mean(d2   )*N2; }
//     else if ( !f &&  msk ) { mean += Image::mean(i1,m1)*N1; mean += Image::mean(i2,m2)*N2; }
//     else if ( !f && !msk ) { mean += Image::mean(i1   )*N1; mean += Image::mean(i2   )*N2; }
//     // - add to normalization
//     in   += ins*ones; // if normalization is scalar, convert to matrix (does nothing otherwise)
//     res  += in*ir;    // add unnormalized result to ensemble
//     norm += in;       // add normalization to ensemble
//     N    += N1;       // add normalization of the mean to ensemble
//     N    += N2;       // add normalization of the mean to ensemble
//   }

// };

};// class Files

// =================================================================================================

#endif
