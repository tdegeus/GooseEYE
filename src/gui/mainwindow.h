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

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include <string>
#include <vector>
#include <math.h>

#include "../core/image.h"
#include "cppcolormap/cppcolormap.h"
#include "json/json.hpp"

using json = nlohmann::json;

// =================================================================================================

class QtImage {
public:

// -------------------------------------------------------------------------------------------------

mat::matrix<int>           data        ; // raw image
QImage                     dataQt      ; // raw image
mat::matrix<int>           img         ; // interpreted image
mat::matrix<int>           msk         ; // mask corresponding to interpreted images
mat::matrix<unsigned char> view        ; // image to view using Qt
size_t                     nrow     = 0; // number of rows
size_t                     ncol     = 0; // number of columns
double                     scale       ; // scale factor to be used in rendering

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

class Filter {
public:

// -------------------------------------------------------------------------------------------------

bool                active = false;
std::vector<size_t> data;

// -------------------------------------------------------------------------------------------------

Filter            (const Filter &) = default;
Filter& operator= (const Filter &) = default;
Filter(){};

// -------------------------------------------------------------------------------------------------

void fromlist(std::vector<size_t> inp)
{
  while ( data.size()<inp.size() )
    data.push_back(0);

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

class File {
public:

// -------------------------------------------------------------------------------------------------

QString name;
Filter  phase;
Filter  mask;
Filter  row;
Filter  col;
bool    setDefault = true;

// -------------------------------------------------------------------------------------------------

File            (const File &) = default;
File& operator= (const File &) = default;
File(){};

// -------------------------------------------------------------------------------------------------

};

// =================================================================================================

class Set {
public:

// -------------------------------------------------------------------------------------------------

QString           field = "";
QString           dtype = "";
std::vector<File> files;

// -------------------------------------------------------------------------------------------------

std::vector<QString> fileNames() {

  std::vector<QString> out;

  for ( auto &file : files )
    out.push_back(file.name);

  return out;
};

// -------------------------------------------------------------------------------------------------

void rm(std::vector<size_t> idx) {

  std::sort(idx.begin(),idx.end(),[](size_t i,size_t j){return i>j;});

  for ( size_t &i : idx )
    files.erase(files.begin()+i);

};

// -------------------------------------------------------------------------------------------------

};

// =================================================================================================

class Data {
public:

// -------------------------------------------------------------------------------------------------

QDir    path        = QDir(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
QString fname       = "";
QString stat        = "";
bool    periodic    = true;
bool    zeropad     = false;
bool    mask_weight = false;
QString pixel_path  = "Bresenham";

std::vector<QString> pdf  = {"",""};
std::vector<size_t>  roi  = {0,0};
std::vector<Set>     sets;

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

  file->phase.active = false;
  file->phase.data.resize(2);
  file->phase.data[0] = 0;
  file->phase.data[1] = 256;

  file->mask.active = false;
  file->mask.data.resize(0);

  file->row.active = false;
  file->row.data.resize(2);
  file->row.data[0] = 0;
  file->row.data[1] = out.nrow;

  file->col.active = false;
  file->col.data.resize(2);
  file->col.data[0] = 0;
  file->col.data[1] = out.ncol;

  file->setDefault = false;

  return out;
};

// -------------------------------------------------------------------------------------------------

void image(size_t iset, size_t iimg, QtImage &out, int crop=-2)
{
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
  out.img.resize({jrow-irow,jcol-icol});
  out.msk.resize({jrow-irow,jcol-icol});

  // phase threshold float: retain values; everything outside bounds is masked
  if ( sets[iset].dtype=="float" ) {
    for ( size_t i=0; i<(jrow-irow); ++i ) {
      for ( size_t j=0; j<(jcol-icol); ++j ) {
        if ( out.data(i+irow,j+icol)>=min && out.data(i+irow,j+icol)<max )
          out.img(i,j) = out.data(i+irow,j+icol);
        else
          out.msk(i,j) = 1;
      }
    }
  }
  // phase threshold binary/int: between (min,max)->1, else->0 (nothing masked)
  else {
    for ( size_t i=0; i<(jrow-irow); ++i )
      for ( size_t j=0; j<(jcol-icol); ++j )
        if ( out.data(i+irow,j+icol)>=min && out.data(i+irow,j+icol)<max )
          out.img(i,j) = 1;
  }

  // int: determine clusters from binary image
  if ( sets[iset].dtype=="int" ) {
    mat::matrix<int> clusters,centers;
    std::tie(clusters,centers) = Image::clusters(out.img,periodic);
    for ( size_t i=0; i<clusters.size(); ++i )
      out.img[i] = clusters[i];
  }

  // mask threshold
  if ( file->mask.active ) {
    for ( size_t imsk=0; imsk<file->mask.data.size(); imsk+=2 ) {
      int low = file->mask.data[imsk  ];
      int hgh = file->mask.data[imsk+1];
      if ( low<hgh )
        for ( size_t i=0; i<(jrow-irow); ++i )
          for ( size_t j=0; j<(jcol-icol); ++j )
            if ( out.data(i+irow,j+icol)>=low && out.data(i+irow,j+icol)<hgh )
              out.msk(i,j) = 1;
    }
  }

  if ( crop<-1 )
    return;

  // temporary copy
  mat::matrix<int> img = out.img;
  mat::matrix<int> msk = out.msk;

  // allocate output
  out.img.resize(out.data.shape()); out.img.ones (); out.img *= crop;
  out.msk.resize(out.data.shape()); out.msk.zeros();

  for ( size_t i=0; i<img.shape()[0]; ++i ) {
    for ( size_t j=0; j<img.shape()[1]; ++j ) {
      out.img(i+irow,j+icol) = img(i,j);
      out.msk(i+irow,j+icol) = msk(i,j);
    }
  }
};

// -------------------------------------------------------------------------------------------------

QtImage image(size_t iset, size_t iimg, int crop=-2)
{
  QtImage out = imageRead(iset,iimg);

  image(iset,iimg,out,crop);

  return out;
};

// -------------------------------------------------------------------------------------------------

void new_path(QDir new_path) {

  if ( new_path==path )
    return;

  for ( auto &set : sets )
    for ( auto &file : set.files )
      file.name = new_path.relativeFilePath(path.filePath(file.name));

  for ( auto &name : pdf )
    name = new_path.relativeFilePath(path.filePath(name));

  fname = new_path.relativeFilePath(path.filePath(fname));

  path  = new_path;
}

// -------------------------------------------------------------------------------------------------

// TODO
void write(QString fname="") {

  // default file-name (absolute path)
  if ( fname.size()==0 )
    fname = path.absoluteFilePath(fname);
  // allocate
  json out;
  // // basic settings
  // out["nset"] = files.size();
  // TODO
};

// -------------------------------------------------------------------------------------------------

// TODO
bool read(QString fname) {
  try {
    json inp;
    std::ifstream fname_j(fname.toStdString());
    fname_j >> inp;

    if ( inp.count("periodic") ) periodic = inp["periodic"];
  }
  catch (...) {
    return false;
  }

  return true;
};

// -------------------------------------------------------------------------------------------------

};

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
  // directly connect buttons
  void on_pushButtonT0_load_clicked();                    // load JSON      -> update "data"
  void on_pushButtonT0_path_clicked();                    // change path    -> update "data"
  void on_lineEditT0_json_textEdited(const QString &text);// manual file-name change
  void on_lineEditT0_res1_textEdited(const QString &text);// manual file-name change
  void on_lineEditT0_res2_textEdited(const QString &text);// manual file-name change
  void on_pushButtonT2_cp_clicked();                      // copy files "data.sets[0]" to "...[1]"
  // connect groups of buttons
  void fileAdd(size_t); // add    files         -> update "data"
  void fileRmv(size_t); // remove files         -> update "data"
  void fileUp (size_t); // move   files up      -> update "data"
  void fileDwn(size_t); // move   files down    -> update "data"
  void fileSrt(size_t); // sort   files by name -> update "data"
  // synchronize widgets and "data"
  void tab1_read    (); // update "data" with new information from widgets
  void tab3_read    (); // update "data" with new information from widgets
  void tab3_default (); // set "File" defaults for current image
  void tab3_applyAll(); // apply "File" of current image to all images in the set
  void tab0_show    (); // refresh widget with new "data"
  void tab1_show    (); // refresh widget with new "data"
  void tab2_show    (); // refresh widget with new "data"
  void tab3_show    (); // refresh widget with new "data"
  void tab3_imag    (); // re-render image

private:
  // store fields
  Ui::MainWindow  *ui;
  Data             data;
  QtImage          image;
  // arrays with buttons
  std::vector<QRadioButton*> statBtn;    // list with buttons, each corresponding to one statistic
  std::vector<QString>       statKey;    // names corresponding to "statBtn"
  std::vector<QRadioButton*> typeBtn;    // list with buttons with dtypes: [b0,i0,f0,b1,i1,f1]
  std::vector<QString>       typeKey;    // names corresponding to "typeBtn"
  std::vector<QCheckBox*>    nsetBtn;    // list with buttons to select the number of sets
  std::vector<QPushButton*>  fileBtn;    // list with all file manipulation buttons (tab2)
  std::vector<QPushButton*>  fileBtnAdd; // - add buttons ["set0","set1"]
  std::vector<QPushButton*>  fileBtnRmv; // - add remove buttons
  std::vector<QPushButton*>  fileBtnUp ; // - all move up buttons
  std::vector<QPushButton*>  fileBtnDwn; // - all move down buttons
  std::vector<QPushButton*>  fileBtnSrt; // - all sort buttons
  std::vector<QListWidget*>  fileLst;    // all file lists
  std::vector<QLabel*>       propLbl;    // list with labels to denote the field-type ["phase",...]
  std::vector<QLabel*>       typeLbl;    // list with labels to denote the data-type
  std::vector<QPushButton*>  imBtn;      // list with all image selection buttons
  std::vector<QComboBox*>    imCombo;    // list with all comboBoxes used to view the image
  std::vector<QCheckBox*>    imCheck;    // list with all checkBoxes used for image manipulation
  std::vector<QSpinBox*>     imSpn;      // list with all spinBoxes used for image manipulation
  std::vector<QSpinBox*>     imSpnPhase; // - phase selection
  std::vector<QSpinBox*>     imSpnMask;  // - mask  selection
  std::vector<QSpinBox*>     imSpnRow;   // - row   selection
  std::vector<QSpinBox*>     imSpnCol;   // - col   selection
  std::vector<QRadioButton*> imRadio;    // list with all mouse selection buttons
  std::vector<QButtonGroup*> btnGroup;   // list with all groups of radioButtons
  // support functions
  bool   promptQuestion(QString);        // prompt question to user (return response)
  void   promptWarning (QString);        // prompt warning to user
  bool   exists(QString);                // check if a file exits (relative w.r.t. "data.path")
  size_t exists(QString,QString,QString);// check how many of these files exist
};

#endif // MAINWINDOW_H
