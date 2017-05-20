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

// convert relative path "fname" from being relative to "old" to begin relative to "current"
std::string updatePath (QDir old, QDir current, std::string fname);

// =================================================================================================

class QtImage
{
public:
int              iset    = -1; // set-index of this image
int              iimg    = -1; // image-index (within the set) of this image
mat::matrix<int> data        ; // image, as matrix
QImage           dataQt      ; // image, as Qt object
size_t           nrow     = 0;
size_t           ncol     = 0;
double           scale       ; // scale factor, to be used to display

// -------------------------------------------------------------------------------------------------

QtImage            (const QtImage &) = default;
QtImage& operator= (const QtImage &) = default;
QtImage(){};

// -------------------------------------------------------------------------------------------------

bool read(QString fname, int Iset=-1, int Iimg=-1, int width=-1, int height=-1, int zoom=-1)
{
  // check to read
 if ( Iimg==iimg && Iset==iset && Iset>=0 && Iimg>=0 )
   return false;
  // store input
  iimg  = Iimg;
  iset  = Iset;
  // load from file
  dataQt.load(fname);
  // read the size
  nrow = dataQt.height();
  ncol = dataQt.width ();
  // allocate data
  data.resize({nrow,ncol});
  // read image
  for ( size_t i=0; i<nrow; ++i )
    for ( size_t j=0; j<ncol; ++j )
      data(i,j) = qGray(dataQt.pixel(j,i));
  // continue only if the widget's width is known
  if ( width<0 || height<0 || zoom<0 )
    return true;
  // convert zoom + widget's width/heigth to unit-scale
  double zm   = pow(1.066,static_cast<double>(zoom));
  double wdth = zm*static_cast<double>(width )/static_cast<double>(data.shape()[1]);
  double hght = zm*static_cast<double>(height)/static_cast<double>(data.shape()[0]);
  // find scale factor
  scale = 0.95*std::min(wdth,hght);

  return true;
};

// -------------------------------------------------------------------------------------------------

// crop==-1 -> image is cropped; crop>=0 -> "crop" is assigned to pixels that are out-of-bounds
std::tuple<mat::matrix<int>,mat::matrix<int>> config(
  json file, bool periodic, std::string dtype, int crop=-1)
{
  // default shape of output matrix
  size_t irow = 0;
  size_t jrow = data.shape()[0];
  size_t icol = 0;
  size_t jcol = data.shape()[1];

  // default range of the output matrix
  int min  = 0;
  int max  = 255;

  // crop: modify dimensions of the output matrix
  // threshold: read range
  if ( file.count("row") ) {
    if ( file["row"]["active"] ) {
      irow = file["row"]["values"][0];
      jrow = file["row"]["values"][1];
    }
  }
  if ( file.count("col") ) {
    if ( file["col"]["active"] ) {
      icol = file["col"]["values"][0];
      jcol = file["col"]["values"][1];
    }
  }
  if ( file.count("phase") ) {
    if ( file["phase"]["active"] ) {
      min  = static_cast<int>(file["phase"]["values"][0]);
      max  = static_cast<int>(file["phase"]["values"][1]);
    }
  }

  // allocate output
  mat::matrix<int> out({jrow-irow,jcol-icol});
  mat::matrix<int> msk({jrow-irow,jcol-icol});

  // phase threshold float: retain values; everything outside bounds is masked
  if ( dtype=="float" ) {
    for ( size_t i=0; i<(jrow-irow); ++i ) {
      for ( size_t j=0; j<(jcol-icol); ++j ) {
        if ( data(i+irow,j+icol)>=min && data(i+irow,j+icol)<max )
          out(i,j) = data(i+irow,j+icol);
        else
          msk(i,j) = 1;
      }
    }
  }
  // phase threshold binary/int: between (min,max)->1, else->0 (nothing masked)
  else {
    for ( size_t i=0; i<(jrow-irow); ++i )
      for ( size_t j=0; j<(jcol-icol); ++j )
        if ( data(i+irow,j+icol)>=min && data(i+irow,j+icol)<max )
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
    if ( file["mask"]["active"] ) {
      for ( size_t imsk=0; imsk<file["mask"]["values"].size(); imsk+=2 ) {
        int low = file["mask"]["values"][imsk  ];
        int hgh = file["mask"]["values"][imsk+1];
        if ( low<hgh )
          for ( size_t i=0; i<(jrow-irow); ++i )
            for ( size_t j=0; j<(jcol-icol); ++j )
              if ( data(i+irow,j+icol)>=low && data(i+irow,j+icol)<hgh )
                msk(i,j) = 1;
      }
    }
  }

  if ( crop<0 )
    return std::make_tuple(out,msk);

  // allocate output
  mat::matrix<int> out_tot(data.shape()); out_tot.ones (); out_tot *= crop;
  mat::matrix<int> msk_tot(data.shape()); msk_tot.zeros();

  for ( size_t i=0; i<out.shape()[0]; ++i ) {
    for ( size_t j=0; j<out.shape()[1]; ++j ) {
      out_tot(i+irow,j+icol) = out(i,j);
      msk_tot(i+irow,j+icol) = msk(i,j);
    }
  }

  return std::make_tuple(out_tot,msk_tot);
};
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
  void on_pushButtonT0_load_clicked();                        // load JSON      -> update "data"
  void on_pushButtonT0_path_clicked();                    // change path    -> update "data"
  void on_lineEditT0_path_textEdited(const QString &arg1);// change path    -> update "data"
  void on_lineEditT0_json_textEdited(const QString &arg1);// manual file-name change
  void on_lineEditT0_res1_textEdited(const QString &arg1);// manual file-name change
  void on_lineEditT0_res2_textEdited(const QString &arg1);// manual file-name change
  void on_pushButtonT2_cp_clicked();                          // copy files "set0" -> "set1"
  // connect groups of buttons
  void fileAdd(size_t); // add    files         -> update "data"
  void fileRmv(size_t); // remove files         -> update "data"
  void fileUp (size_t); // move   files up      -> update "data"
  void fileDwn(size_t); // move   files down    -> update "data"
  void fileSrt(size_t); // sort   files by name -> update "data"
  //
  void checkData();
  // synchronize widgets and "data"
  void tab1_read(); // update "data" with new information from widgets
  void tab0_show(); // refresh with new "data"
  void tab1_show(); // refresh with new "data"
  void tab2_show(); // refresh with new "data"
  void tab3_show(); // refresh with new "data"
  void tab3_read();
  // support functions
  // - read single file "iimg" from "iset", output as Qt string (absolute path)
  QString readFilePath(size_t iset, size_t iimg);
  // - read single file "iimg" from "iset", output as Qt string
  QString readFile(size_t iset, size_t iimg);
  // - read files from "iset", output as vector of strings
  std::vector<std::string> readFiles(size_t iset);
  // - replace all files in "iset" with vector of strings
  void setFiles(size_t iset, std::vector<std::string> files);

private:
  Ui::MainWindow  *ui;
  json             data;
  QDir             out_path = QDir(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
  QString          out_json = "";
  QtImage          image;
  int              iset_    = -1;        // store last read set/image
  int              iimg_    = -1;        // store last read set/image
  mat::matrix<int> imgRaw_      ;        // store image to view interpreted image without reread
  std::vector<QRadioButton*> statBtn;    // list with buttons, each corresponding to one statistic
  std::vector<std::string>   statKey;    // names corresponding to "statBtn"
  std::vector<QRadioButton*> typeBtn;    // list with buttons with dtypes: [b0,i0,f0,b1,i1,f1]
  std::vector<std::string>   typeKey;    // names corresponding to "typeBtn"
  std::vector<QCheckBox*>    nsetBtn;    // list with buttons to select the number of sets
  std::vector<std::string>   nsetKey;    // names corresponding to "nsetBtn": ["set0","set1"]
  std::vector<QPushButton*>  fileBtn;    // list with all file manipulation buttons (tab2)
  std::vector<QPushButton*>  fileBtnAdd; // - add buttons ["set0","set1"]
  std::vector<QPushButton*>  fileBtnRmv; // - add remove buttons
  std::vector<QPushButton*>  fileBtnUp ; // - all move up buttons
  std::vector<QPushButton*>  fileBtnDwn; // - all move down buttons
  std::vector<QPushButton*>  fileBtnSrt; // - all sort buttons
  std::vector<QListWidget*>  fileLst;    // all file lists
  std::vector<QLabel*>       propLbl;    // list with labels to denote the field-type ["phase",...]
  std::vector<QLabel*>       typeLbl;    // list with labels to denote the data-type
  std::vector<QComboBox*>    imgCombo;
  std::vector<QSpinBox*>     imgSpin;
  std::vector<size_t>        imgVal;
  std::vector<QCheckBox*>    imgCheck;
  std::vector<std::string>   imgCheckLbl;
  std::vector<QRadioButton*> imgRadio;
  std::vector<QPushButton*>  imgBtn;     // list with all image selection buttons
  std::vector<QButtonGroup*> btnGroup;   // list with all groups of radioButtons
  bool   promptQuestion(QString);        // prompt question to user (return response)
  void   promptWarning (QString);        // prompt warning to user
  bool   exists(QString);                // check if a file exits (NB: relative path wrt "out_path")
  size_t exists(QString,QString,QString);// check how many of these files exist
};

#endif // MAINWINDOW_H
