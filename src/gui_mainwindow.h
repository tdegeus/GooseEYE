#ifndef GUI_MAINWINDOW_H
#define GUI_MAINWINDOW_H

#include <QSet>
#include <QMainWindow>
#include <QFileDialog>
#include <QStandardPaths>
#include <QMessageBox>
#include <QListWidget>

#include <iostream>
#include <string>
#include <vector>

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

private:
  Ui::MainWindow *ui;

  QStringList phase_;
  QStringList dtype_;
  QString func_;
  QString outDir_;
  QString outName_;

  void WIP(void);
  void tab1_selectStat(void);
  void tab2_selectFiles(QListWidget *list);
};

#endif // MAINWINDOW_H
