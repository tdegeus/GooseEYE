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

#include <iostream>
#include <string>
#include <vector>
#include <math.h>

#include "image.h"

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

  // write to message bar
  void on_tabWidget_tabBarClicked(int index);

  // tab0: load state, select directory/files
  void on_tab0_load_pushButton_clicked();
  void on_tab0_outdir_pushButton_clicked();
  void on_tab0_result_pushButton_clicked();

  // run "tab1_selectStat()"
  void on_tab1_measure_treeWidget_clicked(const QModelIndex &index);

  // check: run "tab1_selectStat()" ; uncheck: clear radioBox
  void on_tab1_im0_checkBox_toggled(bool checked);
  void on_tab1_im1_checkBox_toggled(bool checked);

  // toggle "tab1_imX_checkBox"; run "tab1_selectStat()"
  void on_tab1_im0b_radioButton_clicked();
  void on_tab1_im0f_radioButton_clicked();
  void on_tab1_im0i_radioButton_clicked();
  void on_tab1_im1b_radioButton_clicked();
  void on_tab1_im1f_radioButton_clicked();
  void on_tab1_im1i_radioButton_clicked();

  // add files to QListWidget
  void on_tab2_im0Add_pushButton_clicked();
  void on_tab2_im1Add_pushButton_clicked();
  void on_tab2_im0Rmv_pushButton_clicked();
  void on_tab2_im1Rmv_pushButton_clicked();
  void on_tab2_im1Up__pushButton_clicked();
  void on_tab2_im1Dwn_pushButton_clicked();
  void on_tab2_cp_pushButton_clicked();

  // define "set" of images, create list of files
  void on_tab3_set_comboBox_currentIndexChanged(int index);
  void on_tab3_setPrev_pushButton_clicked();
  void on_tab3_setNext_pushButton_clicked();
  void on_tab3_imPrev_pushButton_clicked();
  void on_tab3_imNext_pushButton_clicked();


  void on_tab3_im_comboBox_currentIndexChanged(int index);

  void on_tab3_zoom_slider_valueChanged(int value);
  void on_tab3_zoomOut_pushButton_clicked();
  void on_tab3_zoomIn__pushButton_clicked();

  void on_tab4_cp2out_checkBox_toggled(bool checked);

private:
  Ui::MainWindow *ui;

  QStringList phase_;
  QStringList dtype_;
  QString     func_;
  QString     outDir_;
  QString     outName_;
  QImage      imageQt_;
  std::vector<int> image_;
  std::vector<unsigned char> imageChar_;

  void   WIP(void);
  void   tab1_selectStat(void);
  void   tab2_selectFiles(QListWidget *list);
  double tab3_scaleImage(void);
  void   tab3_readImage(void);
  void   tab3_readPhase(void);
  void   tab3_viewImage(void);
  void   tab3_viewPhase(void);
};

#endif // MAINWINDOW_H
