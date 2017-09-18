#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QSet>
#include <QMainWindow>
#include <QFileDialog>
#include <QRadioButton>
#include <QSpinBox>
#include <QStandardPaths>
#include <QMessageBox>
#include <QListWidget>
#include <QImage>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QByteArray>
#include <QObject>
#include <QScrollBar>
#include <QComboBox>
#include <QApplication>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include <string>
#include <vector>
#include <math.h>

#include "qcustomplot.h"

#include "../core/image.h"
#include "cppcolormap/cppcolormap.h"
#include "json/json.hpp"

using json = nlohmann::json;

// =================================================================================================
// Store an image read from a file, and different interpreted versions, including view settings
// =================================================================================================

class QtImage {
public:

// -------------------------------------------------------------------------------------------------

// Fields:
mat::matrix<int>           data        ; // raw image as 8-bit matrix ('gray'-values 0..255)
QImage                     dataQt      ; // raw image as Qt-object
mat::matrix<int>           imgBin      ; // interpreted image (0    |  1  )
mat::matrix<double>        imgFlt      ; // interpreted image (0.0 ... 1.0), exists always
mat::matrix<int>           clsInt      ; // clusters (0...), corresponding to "imgBin"
mat::matrix<int>           cenInt      ; // center of the clusters in "clsInt"
mat::matrix<int>           msk         ; // mask (binary: 0 | 1)
mat::matrix<unsigned char> view        ; // image to view using Qt
size_t                     nrow = 0    ; // number of rows
size_t                     ncol = 0    ; // number of columns
double                     N    = 0.0  ; // number of data-points (points that are not masked)
double                     mean = 0.0  ; // spatial average of "imgFlt" (bin images eqv to "imgBin")
double                     scale       ; // scale factor to be used in rendering (see "setScale")
// Methods:
// - setScale: compute "scale" from a widget's with/height and a zoom-factor

// -------------------------------------------------------------------------------------------------

QtImage            (const QtImage &) = default;
QtImage& operator= (const QtImage &) = default;
QtImage(){};

// -------------------------------------------------------------------------------------------------

void setScale(int width, int height, int zoom)
{
  double zm   = pow(1.066,static_cast<double>(zoom));
  double wdth = zm*static_cast<double>(width )/static_cast<double>(data.shape()[1]);
  double hght = zm*static_cast<double>(height)/static_cast<double>(data.shape()[0]);
  scale       = 0.95*std::min(wdth,hght);
}

// -------------------------------------------------------------------------------------------------

};

// =================================================================================================
// Store threshold as [min0,max0,min1,max1,...] (e.g. gray-values, or rows/columns)
// =================================================================================================

class Filter {
public:

// -------------------------------------------------------------------------------------------------

// Fields:
bool                active = false; // filter can be set inactive (values or stored but ignored)
std::vector<size_t> data;           // values stored as [min,max,...], convention: min <= I < max
// Methods:
// - fromList: overwrite values from input list
// - asList: output values as list of a specified list (it is optionally zero-padded)
// - newPath

// -------------------------------------------------------------------------------------------------

Filter            (const Filter &) = default;
Filter& operator= (const Filter &) = default;
Filter(){};

// -------------------------------------------------------------------------------------------------

void fromlist(std::vector<size_t> inp)
{
  data.resize(inp.size());

  for ( size_t i=0; i<inp.size(); ++i )
    data[i] = inp[i];
}

// -------------------------------------------------------------------------------------------------

std::vector<size_t> aslist(size_t n)
{
  std::vector<size_t> out(n,0);

  for ( size_t i=0; i<std::min(n,data.size()); ++i )
    out[i] = data[i];

  return out;
};

// -------------------------------------------------------------------------------------------------

};

// =================================================================================================
// Store threshold settings and filename (in relative path)
// =================================================================================================

class File {
public:

// -------------------------------------------------------------------------------------------------

// Fields:
QString name;               // file-name (below a relative path is assumed)
Filter  phase;              // phase threshold(s)
Filter  mask;               // phase threshold(s)
Filter  row;                // rows to select
Filter  col;                // columns to select
bool    setDefault = true;  // signal to overwrite all thresholds with defaults at the first chance

// -------------------------------------------------------------------------------------------------

File            (const File &) = default;
File& operator= (const File &) = default;
File(){};

// -------------------------------------------------------------------------------------------------

};

// =================================================================================================
// Store a batch of images, include their interpretation
// =================================================================================================

class Set {
public:

// -------------------------------------------------------------------------------------------------

// Fields:
QString           field = "";   // type of field: "phase", "weight"
QString           dtype = "";   // data-type to which to convert the image: "binary", "int", "float"
std::vector<File> files;        // files (names, and thresholds)
// Methods:
// - fileNames: list with filenames (convention: relative paths)
// - rm: take items with index "idx" from the list of files

// -------------------------------------------------------------------------------------------------

std::vector<QString> fileNames()
{
  std::vector<QString> out;

  for ( auto &file : files )
    out.push_back(file.name);

  return out;
};

// -------------------------------------------------------------------------------------------------

void rm(std::vector<size_t> idx)
{
  std::sort(idx.begin(),idx.end(),[](size_t i,size_t j){return i>j;});

  for ( size_t &i : idx )
    files.erase(files.begin()+i);
};

// -------------------------------------------------------------------------------------------------

};

// =================================================================================================
// Store plot settings
// =================================================================================================

class Plot {
public:

// -------------------------------------------------------------------------------------------------

// Fields:
QString             name = "";        // file-name to store result (PDF)
QString             cmap = "RdBu_r";  // name of the colormap
std::vector<double> lim  = {0.0,1.0}; // range of the color-axis

// -------------------------------------------------------------------------------------------------

};

// =================================================================================================
// All data needed to read images, interpret them, and compute a statistic (incl. result)
// =================================================================================================

class Data {
public:

// -------------------------------------------------------------------------------------------------

// Fields (scalars): ("path": path to which all filenames are relative)
QDir    path        = QDir(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
QString fname       = "";           // filename of the JSON file to store all settings
QString stat        = "";           // name of the statistic: "S2", "C2", "L", "W2", "W2c"
bool    periodic    = true;         // assume all images periodic or not
bool    zeropad     = false;        // zero-pad all images (only for non-periodic analyses)
bool    mask_weight = false;        // signal to mask weights (for "W2" and "W2c")
QString pixel_path  = "Bresenham";  // pixel-path algorithm to use (for "L" and "W2c")
// Fields (lists):
std::vector<Plot>    plot;          // plot settings
std::vector<size_t>  roi  = {1,1};  // shape of the output ROI
std::vector<Set>     sets;          // sets of files (the order of files has to correspond)
mat::matrix<double>  res;           // result, unnormalized                 (shape == "roi")
mat::matrix<double>  resnorm;       // normalization to be applied to "res" (shape == "roi")
double               mean;          // spatial average, unnormalized
double               meannorm;      // normalization to be applied to "mean"
// Methods:
// - imageRead: read one image file, convert to mat::matrix, apply defaults (if requested)
// - image: (read one image using "imageRead"), interpret to image by applied thresholds
// - newPath: change the path (changes or the files to a new relative path)
// - write: write all settings of this class to a JSON file
// - read: read settings (partly or all) from a JSON file (used to restore a previous state)

// -------------------------------------------------------------------------------------------------

QtImage imageRead(size_t iset, size_t iimg)
{
  // allocate output
  QtImage out;

  // alias
  File *file = &sets[iset].files[iimg];

  // load using Qt
  out.dataQt.load(path.filePath(file->name));
  // read the size
  out.nrow = out.dataQt.height();
  out.ncol = out.dataQt.width ();
  // allocate data
  out.data.resize({out.nrow,out.ncol});
  // convert QImage -> mat::matrix
  for ( size_t i=0; i<out.nrow; ++i )
    for ( size_t j=0; j<out.ncol; ++j )
      out.data(i,j) = qGray(out.dataQt.pixel(j,i));

  // check to set defaults
  if ( !file->setDefault )
    return out;
  // - signal that default has been set
  file->setDefault   = false;
  // - set all default inactive
  file->phase.active = false;
  file->mask .active = false;
  file->row  .active = false;
  file->col  .active = false;
  // - resize values
  file->phase.data.resize(2); // - phase: all gray values in case of an 8-bit image (as read by Qt)
  file->mask .data.resize(0); // - mask : no mask
  file->row  .data.resize(2); // - row  : total image
  file->col  .data.resize(2); // - col  : total image
  // - apply values (see comments above)
  file->phase.data[0] = 0;  file->phase.data[1] = 256;
  file->row  .data[0] = 0;  file->row  .data[1] = out.nrow;
  file->col  .data[0] = 0;  file->col  .data[1] = out.ncol;

  return out;
};

// -------------------------------------------------------------------------------------------------

QtImage image(size_t iset, size_t iimg, QtImage &out, int crop=-2)
{
  // check
  if ( iset>=sets.size() )
    throw std::runtime_error("'iset' out-of-bounds");
  if ( iimg>=sets[iset].files.size() )
    throw std::runtime_error("'iimg' out-of-bounds");

  // alias
  File *file = &sets[iset].files[iimg];

  // default shape of output matrix
  size_t irow = 0;
  size_t jrow = out.data.shape()[0];
  size_t icol = 0;
  size_t jcol = out.data.shape()[1];

  // default range of the output matrix
  int min  = 0;
  int max  = 256;

  // crop: modify dimensions of the output matrix / threshold: read range
  if ( file->row.active ) {
    irow = file->row.data[0];
    jrow = file->row.data[1];
  }
  if ( file->col.active ) {
    icol = file->col.data[0];
    jcol = file->col.data[1];
  }
  if ( file->phase.active ) {
    min  = static_cast<int>(file->phase.data[0]);
    max  = static_cast<int>(file->phase.data[1]);
  }

  // allocate output
  out.imgBin.resize({jrow-irow,jcol-icol}); out.imgBin.zeros();
  out.imgFlt.resize({jrow-irow,jcol-icol}); out.imgFlt.zeros();
  out.clsInt.resize({jrow-irow,jcol-icol}); out.clsInt.zeros();
  out.cenInt.resize({jrow-irow,jcol-icol}); out.cenInt.zeros();
  out.msk   .resize({jrow-irow,jcol-icol}); out.msk   .zeros();
  out.view  .resize({jrow-irow,jcol-icol});

  // phase threshold
  // - float: retain values; everything outside bounds is masked
  if ( sets[iset].dtype=="float" )
  {
    for ( size_t i=0; i<(jrow-irow); ++i ) {
      for ( size_t j=0; j<(jcol-icol); ++j ) {
        if ( out.data(i+irow,j+icol)>=min && out.data(i+irow,j+icol)<max )
          out.imgFlt(i,j) = static_cast<double>(out.data(i+irow,j+icol))/255.;
        else
          out.msk(i,j) = 1;
      }
    }
  }
  // - binary (or int): between (min,max)->1, else->0 (nothing masked)
  else
  {
    // -- apply threshold
    for ( size_t i=0; i<(jrow-irow); ++i )
      for ( size_t j=0; j<(jcol-icol); ++j )
        if ( out.data(i+irow,j+icol)>=min && out.data(i+irow,j+icol)<max )
          out.imgBin(i,j) = 1;
    // -- copy to float image (for convenience)
    out.imgFlt = out.imgBin;
  }

  // mask threshold
  if ( file->mask.active )
  {
    for ( size_t imsk=0; imsk<file->mask.data.size(); imsk+=2 ) {
      int min = file->mask.data[imsk  ];
      int max = file->mask.data[imsk+1];
      for ( size_t i=0; i<(jrow-irow); ++i )
        for ( size_t j=0; j<(jcol-icol); ++j )
          if ( out.data(i+irow,j+icol)>=min && out.data(i+irow,j+icol)<max )
            out.msk(i,j) = 1;
    }
  }

  // compute average
  std::tie(out.mean,out.N) = Image::mean(out.imgFlt,out.msk);

  // int: determine clusters from binary image
  if ( sets[iset].dtype=="int" )
  {
    // - apply mask
    for ( size_t i=0; i<out.imgBin.size(); ++i )
      if ( out.msk[i] )
        out.imgBin[i] = 0;
    // - compute clusters and centers
    std::tie(out.clsInt,out.cenInt) = Image::clusters(out.imgBin,periodic);
  }

  // keep cropped image -> quit here
  // otherwise          -> extend the image
  // - check
  if ( crop<-1 )
    return out;
  // - copy to temporary variables
  mat::matrix<double> imgFlt = out.imgFlt;
  mat::matrix<int>    imgBin = out.imgBin;
  mat::matrix<int>    clsInt = out.clsInt;
  mat::matrix<int>    cenInt = out.cenInt;
  mat::matrix<int>    msk    = out.msk   ;
  // - allocate output: assign the value of "crop" to the entire image (the interior is overwritten)
  out.view  .resize(out.data.shape());
  out.imgFlt.resize(out.data.shape()); out.imgFlt.ones (); out.imgFlt *= static_cast<double>(crop);
  out.imgBin.resize(out.data.shape()); out.imgBin.ones (); out.imgBin *= crop;
  out.clsInt.resize(out.data.shape()); out.clsInt.ones (); out.clsInt *= crop;
  out.cenInt.resize(out.data.shape()); out.cenInt.ones (); out.cenInt *= crop;
  out.msk   .resize(out.data.shape()); out.msk   .zeros();
  // - copy interior
  for ( size_t i=0; i<imgBin.shape()[0]; ++i ) {
    for ( size_t j=0; j<imgBin.shape()[1]; ++j ) {
      out.imgFlt(i+irow,j+icol) = imgFlt(i,j);
      out.imgBin(i+irow,j+icol) = imgBin(i,j);
      out.clsInt(i+irow,j+icol) = clsInt(i,j);
      out.cenInt(i+irow,j+icol) = cenInt(i,j);
      out.msk   (i+irow,j+icol) = msk   (i,j);
    }
  }

  return out;
};

// -------------------------------------------------------------------------------------------------

QtImage image(size_t iset, size_t iimg, int crop=-2)
{
  // read image using method above
  QtImage out = imageRead(iset,iimg);
  // interpret image using method above (updates "out")
  image(iset,iimg,out,crop);
  // return result
  return out;
};

// -------------------------------------------------------------------------------------------------

void newPath(QDir new_path)
{
  // paths identical: do nothing
  if ( new_path==path )
    return;

  // update all filenames
  // - files (images) in all sets
  for ( auto &set : sets )
    for ( auto &file : set.files )
      file.name = new_path.relativeFilePath(path.filePath(file.name));
  // - plots of the result (PDF)
  for ( auto &pl : plot )
    pl.name = new_path.relativeFilePath(path.filePath(pl.name));
  // - settings file (JSON)
  fname = new_path.relativeFilePath(path.filePath(fname));

  // store new path
  path = new_path;
}

// -------------------------------------------------------------------------------------------------

void write()
{
  // allocate
  json out;
  // variables that can be copied 'as is'
  out["stat"       ] = stat.toStdString();
  out["periodic"   ] = periodic;
  out["zeropad"    ] = zeropad;
  out["mask_weight"] = mask_weight;
  out["pixel_path" ] = pixel_path.toStdString();
  out["roi"        ] = roi;
  out["mean"       ] = mean;
  out["meannorm"   ] = meannorm;
  // result -> flatten to array, store
  if ( res.size()>0 ) {
    if ( roi[0]==res.shape()[0] && roi[1]==res.shape()[1] ) {
      // - allocate flattened array
      std::vector<double> tmp;
      // - fill from "mat::matrix"
      for ( size_t i=0; i<res.size(); ++i )
        tmp.push_back(res[i]);
      // - store
      if ( tmp.size()>0 )
        out["res"] = tmp;
    }
  }
  // result normalization -> flatten to array, store
  if ( resnorm.size()>0 ) {
    if ( roi[0]==resnorm.shape()[0] && roi[1]==resnorm.shape()[1] ) {
      // - allocate flattened array
      std::vector<double> tmp;
      // - fill from "mat::matrix"
      for ( size_t i=0; i<resnorm.size(); ++i )
        tmp.push_back(resnorm[i]);
      // - store
      if ( tmp.size()>0 )
        out["resnorm"] = tmp;
    }
  }
  // output files
  if ( true ) {
    // - define "keys" (names in the JSON file)
    std::vector<std::string> keys = {"plot0","plot1"};
    // - store all plot settings
    for ( size_t i=0; i<keys.size(); ++i ) {
      out[keys[i]]["name"] = plot[i].name.toStdString();
      out[keys[i]]["cmap"] = plot[i].cmap.toStdString();
      out[keys[i]]["lim" ] = plot[i].lim;
    }
  }
  // write all files
  // - construct "keys" (names in JSON file)
  size_t                   n     = 0;
  std::vector<std::string> sname = {"set0","set1"};
  std::vector<std::string> keys;
  // - maximum number of files
  if ( sets.size()>=1 ) n = std::max(n,sets[0].files.size());
  if ( sets.size()>=2 ) n = std::max(n,sets[1].files.size());
  // - set file-index as "key"
  for ( size_t i=0; i<n; ++i )
    keys.push_back(QString::number(i).toStdString());
  // - fill
  for ( size_t iset=0; iset<sets.size(); ++iset ) {
    // -- alias
    std::string   s = sname[iset];
    // -- write data-type and field name
    out[s]["dtype"] = sets[iset].dtype.toStdString();
    out[s]["field"] = sets[iset].field.toStdString();
    // -- write images
    for ( size_t iimg=0; iimg<sets[iset].files.size(); ++iimg ) {
      // --- alias
      File       *f   = &sets[iset].files[iimg];
      std::string key = keys [iimg];
      // --- write filename
      out[s]["files"][key]["name"] = f->name.toStdString();
      // --- alias filters
      std::vector<std::string> st  = {   "phase",   "mask",   "row",   "col"};
      std::vector<Filter*>     val = {&f->phase ,&f->mask ,&f->row ,&f->col };
      // --- loop over filters
      for ( size_t k=0; k<st.size(); ++k ) {
        if ( val[k]->active && val[k]->data.size()>0 ) {
          std::vector<size_t> tmp;
          for ( size_t i=0; i<val[k]->data.size(); i+=2 ) {
            if ( val[k]->data[i]<val[k]->data[i+1] ) {
              tmp.push_back(val[k]->data[i  ]);
              tmp.push_back(val[k]->data[i+1]);
            }
          }
          out[s]["files"][key][st[k]] = tmp;
        }
      }
    }
  }

  // write JSON
  std::ofstream o(path.absoluteFilePath(fname).toStdString());
  o << std::setw(2) << out << std::endl;
};

// -------------------------------------------------------------------------------------------------

void read(QString iname)
{
  // read JSON file to "json" object
  json inp;
  std::ifstream fname_j(iname.toStdString());
  fname_j >> inp;

  // temporary variable from checks
  bool isroi = false;
  // direct copy
  if ( inp.count("stat"       ) ) stat        = QString::fromStdString(inp["stat"       ]);
  if ( inp.count("periodic"   ) ) periodic    =                        inp["periodic"   ];
  if ( inp.count("zeropad"    ) ) zeropad     =                        inp["zeropad"    ];
  if ( inp.count("mask_weight") ) mask_weight =                        inp["mask_weight"];
  if ( inp.count("pixel_path" ) ) pixel_path  = QString::fromStdString(inp["pixel_path" ]);
  if ( inp.count("mean"       ) ) mean        =                        inp["mean"       ];
  if ( inp.count("meannorm"   ) ) meannorm    =                        inp["meannorm"   ];
  // "roi"
  if ( inp.count("roi") ) {
    if ( inp["roi"].size()==2 ) {
      isroi = true;
      roi.resize(2);
      for ( size_t i=0; i<2; ++i ) roi[i] = inp["roi"][i];
    }
  }
  // "res" and "resnorm"
  if ( isroi )
  {
    // - create pointers to loop over both fields
    std::vector<std::string>          keys = {"res","resnorm"};
    std::vector<mat::matrix<double>*> vals = {&res ,&resnorm };
    // - loop over fields
    for ( size_t k=0; k<keys.size(); ++k ) {
      if ( inp[keys[k]].size()==roi[0]*roi[1] ) {
        vals[k]->resize(roi);
        for ( size_t i=0; i<vals[k]->size(); ++i ) (*vals[k])[i] = inp[keys[k]][i];
      }
    }
  }
  // "plot0" and "plot1"
  // - field names
  std::vector<std::string> keys = {"plot0","plot1"};
  // - allocate space
  if ( plot.size()<keys.size() )
    plot.resize(keys.size());
  // - loop over fields
  for ( size_t i=0; i<keys.size(); ++i ) {
    // -- check if field is present
    if ( !inp.count(keys[i]) )
      continue;
    // -- extract "name" and "cmap"
    if ( inp[keys[i]].count("name") ) plot[i].name = QString::fromStdString(inp[keys[i]]["name"]);
    if ( inp[keys[i]].count("cmap") ) plot[i].cmap = QString::fromStdString(inp[keys[i]]["cmap"]);
    // -- extract "lim"
    if ( inp[keys[i]].count("lim") ) {
      for ( size_t j=0; j<std::min(plot[i].lim.size(),inp[keys[i]]["lim"].size()); ++j )
        plot[i].lim[j] = inp[keys[i]]["lim"][j];
    }
  }
  // read files
  // - names of sets
  std::vector<std::string> sname = {"set0","set1"};
  // - read files in each set
  for ( size_t iset=0; iset<sname.size(); ++iset ) {
    // -- check if present in JSON
    if ( !inp.count(sname[iset]) )
      continue;
    // -- expand "sets" field
    sets.resize(iset+1);
    // -- clear all files that may be currently stored
    sets[iset].files.resize(0);
    // -- extract meta-data
    if ( inp[sname[iset]].count("dtype") )
      sets[iset].dtype = QString::fromStdString(inp[sname[iset]]["dtype"]);
    if ( inp[sname[iset]].count("field") )
      sets[iset].field = QString::fromStdString(inp[sname[iset]]["field"]);
    // -- loop over images found in JSON (assumed named "0","1","2",...)
    size_t iimg = 0;
    while ( true ) {
      // --- convert "iimg" to string, for read
      std::string key = QString::number(iimg).toStdString();
      // --- quit searching if the file is not found (numbering assumed sequential)
      if ( !inp[sname[iset]]["files"].count(key) )
        break;
      // --- allocate data
      File i;
      bool setDefault = true;
      // --- extract the file-name, skip if no name is specified
      if ( inp[sname[iset]]["files"][key].count("name") )
        i.name = QString::fromStdString(inp[sname[iset]]["files"][key]["name"]);
      else
        continue;
      // --- specify Filters to extract
      std::vector<std::string> names = {"phase" ,"mask" ,"row" ,"col" };
      std::vector<Filter*>     vals  = {&i.phase,&i.mask,&i.row,&i.col};
      // --- extract all filters
      for ( size_t k=0; k<names.size(); ++k ) {
        if ( inp[sname[iset]]["files"][key].count(names[k]) ) {
          setDefault        = false;
          vals[k]->active = true;
          vals[k]->data.resize(inp[sname[iset]]["files"][key][names[k]].size());
          for ( size_t j=0; j<vals[k]->data.size(); ++j )
            vals[k]->data[j] = inp[sname[iset]]["files"][key][names[k]][j];
        }
      }
      // --- signal to set defaults, only if no filters were found for this file
      i.setDefault = setDefault;
      // --- store file
      sets[iset].files.push_back(i);
      // --- proceed to next image
      ++iimg;
    }
  }
};

// -------------------------------------------------------------------------------------------------

};

// =================================================================================================
// MainWindow: the main (and only) widget (partly depends on the classes above)
// =================================================================================================

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

private slots:
  // Directly connect buttons
  void on_pushButtonT0_load_clicked();                    // load JSON   -> update "data"
  void on_pushButtonT0_path_clicked();                    // change path -> update "data"
  void on_lineEditT0_json_textEdited(const QString &text);// manual file-name change
  void on_lineEditT0_res1_textEdited(const QString &text);// manual file-name change
  void on_lineEditT0_res2_textEdited(const QString &text);// manual file-name change
  void on_pushButtonT2_cp_clicked();                      // copy files "data.sets[0]" to "...[1]"
  // Connect groups of buttons
  void fileAdd(size_t iset); // add    files         -> update "data"
  void fileRmv(size_t iset); // remove files         -> update "data"
  void fileUp (size_t iset); // move   files up      -> update "data"
  void fileDwn(size_t iset); // move   files down    -> update "data"
  void fileSrt(size_t iset); // sort   files by name -> update "data"
  // Compute statistic
  void compute   ();
  void computeS2 (size_t iimg); // applies one 'increment'
  void computeL  (size_t iimg); // applies one 'increment'
  void computeW2 (size_t iimg); // applies one 'increment'
  void computeW2c(size_t iimg); // applies one 'increment'
  // Help
  void openHelp();
  // Synchronize widgets and "data"
  void tab1_read    (); // update "data" with new information from widgets
  void tab3_read    (); // update "data" with new information from widgets
  void tab4_read    (); // update "data" with new information from widgets
  void tab3_default (); // signal to set "File" defaults for current image
  void tab3_applyAll(); // apply settings in "File" of current image to all images in the set
  void tab0_show    (); // refresh widget with new "data"
  void tab1_show    (); // refresh widget with new "data"
  void tab2_show    (); // refresh widget with new "data"
  void tab3_show    (); // refresh widget with new "data"
  void tab3_imag    (); // re-render image
  void tab4_show    (); // refresh widget with new "data"
  void tab4_plot    (); // re-render results
  void tab4_save    (); // save JSON and PDFs

private:
  // Store fields
  Ui::MainWindow  *ui;
  Data             data;
  QtImage          image;
  // Arrays with buttons
  std::vector<QRadioButton*>   statBtn;    // T1: buttons to select statistic
  std::vector<QString>         statKey;    // T1: names corresponding to "statBtn"
  std::vector<QCheckBox*>      statCheck;  // T1: algorithm choices (periodic, zero-pad)
  std::vector<QComboBox*>      statCombo;  // T1: algorithm choices (pixel-path)
  std::vector<QCheckBox*>      nsetBtn;    // T1: buttons to select the number of sets
  std::vector<QRadioButton*>   typeBtn;    // T1: buttons to select the dtype: [b0,i0,f0,b1,i1,f1]
  std::vector<QString>         typeKey;    // T1: names corresponding to "typeBtn"
  std::vector<QLabel*>         propLbl;    // T2: labels denoting the field-type ["phase",...]
  std::vector<QLabel*>         typeLbl;    // T2: labels denoting the data-type
  std::vector<QListWidget*>    fileLst;    // T2: all file lists
  std::vector<QPushButton*>    fileBtn;    // T2: all file manipulation buttons (tab2)
  std::vector<QPushButton*>    fileBtnAdd; //     - add buttons ["set0","set1"]
  std::vector<QPushButton*>    fileBtnRmv; //     - add remove buttons
  std::vector<QPushButton*>    fileBtnUp ; //     - all move up buttons
  std::vector<QPushButton*>    fileBtnDwn; //     - all move down buttons
  std::vector<QPushButton*>    fileBtnSrt; //     - all sort buttons
  std::vector<QPushButton*>    imBtn;      // T3: all image selection buttons
  std::vector<QComboBox*>      imCombo;    // T3: all comboBoxes (selection and colormap)
  std::vector<QRadioButton*>   imRadio;    // T3: all mouse selection buttons
  std::vector<QCheckBox*>      imCheck;    // T3: all checkBoxes for image manipulation
  std::vector<QSpinBox*>       imSpn;      // T3: all spinBoxes for image manipulation
  std::vector<QSpinBox*>       imSpnPhase; // T3: - phase selection
  std::vector<QSpinBox*>       imSpnMask;  // T3: - mask  selection
  std::vector<QSpinBox*>       imSpnRow;   // T3: - row   selection
  std::vector<QSpinBox*>       imSpnCol;   // T3: - col   selection
  std::vector<QSpinBox*>       roiSpin;    // T4: ROI shape
  std::vector<QDoubleSpinBox*> resSpin;    // T4: all color-scale ranges
  std::vector<QComboBox*>      resCombo;   // T4: all colormaps
  std::vector<QCustomPlot*>    resPlot;    // T4: all plots
  std::vector<QButtonGroup*>   btnGroup;   // all groups of radioButtons
  // Support functions
  bool   promptQuestion(QString);        // prompt question to user (return response)
  void   promptWarning (QString);        // prompt warning to user
  bool   exists(QString);                // check if a file exits (relative w.r.t. "data.path")
  size_t exists(QString,QString,QString);// check how many of these files exist
};

#endif // MAINWINDOW_H
