#ifndef GUI_MAINWINDOW_H
#define GUI_MAINWINDOW_H

#include <QSet>
#include <QMainWindow>
#include <QFileDialog>
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
#include <string>
#include <vector>
#include <math.h>

#include "image.h"
#include "image_batch.h"
#include "cppcolormap/cppcolormap.h"

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

  // custom functions to connect buttons
  // - manipulation of QlistWidget with files
  void filesAdd(QListWidget *list, size_t set=0);
  void filesRm (QListWidget *list, size_t set=0);
  void filesUp (QListWidget *list, size_t set=0);
  void filesDwn(QListWidget *list, size_t set=0);
  void filesCp (QListWidget *src,QListWidget *dest, size_t src_set=0, size_t dest_set=0);
  // - select statistics, set allowed data-types, etc.
  void tab1_selectStat(void);
  // - store selected files
  // void tab2_clearFiles(void);
  // - read/view images, or do both
  void tab3_readImage(void);
  void tab3_readPhase(void);
  void tab3_viewImage(void);
  void tab3_viewPhase(void);
  void tab3_syncImage(void);
  void tab3_syncPhase(void);
  void tab3_setDefault(void);
  void tab3_applySelection(int idx=-1);
  void tab3_applyAll(void);

  void tab4_compute(void);
  void tab4_plotResult(void);
  void tab4_plotInterp(void);
  void tab4_saveResult(void);
  void tab4_saveInterp(void);



  // write to message bar
  void on_tabWidget_tabBarClicked(int index);

  // tab0: load state, select directory/files
  void on_tab0_load_pushButton_clicked();
  void on_tab0_outdir_pushButton_clicked();
  void on_tab0_result_pushButton_clicked();
  void on_tab0_pdfRaw_pushButton_clicked();
  void on_tab0_pdfInterp_pushButton_clicked();

  // define "set" of images, create list of files
  void on_tab3_set_comboBox_currentIndexChanged(int index);

  void debug(void);


private:
  Ui::MainWindow *ui;

  // QStringList                  phase_;
  // QStringList                  dtype_;
  // QString                      func_;

  int                          prevTab_ = 0;
  QImage                       imgRawQt_;
  Image::Matrix<int>           imgRaw_;
  Image::Matrix<unsigned char> imgView_;
  ImageBatch::Files8_2D        data_;


  bool   fileExists ( QString fname , QString dirname="" );

  // prompt the user with a warning or a question
  bool   promptQuestion       ( QString msg );
  void   promptWarning        ( QString msg );
  void   promptWorkInProgress ( QString msg );


  double tab3_scaleImage(void);


};

Image::Matrix<int> readImage ( std::string fname );

#endif // MAINWINDOW_H
