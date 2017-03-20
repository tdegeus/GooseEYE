#ifndef GUI_MAINWINDOW_H
#define GUI_MAINWINDOW_H

#include <QSet>
#include <QMainWindow>
#include <QFileDialog>
#include <QStandardPaths>
#include <QMessageBox>

#include <iostream>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

  void WIP(void);

  void tab1_selectStat(void);

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
  void on_tab1_im1_checkBox_toggled(bool checked);
  void on_tab1_im2_checkBox_toggled(bool checked);

  // toggle "tab1_imX_checkBox"; run "tab1_selectStat()"
  void on_tab1_im1b_radioButton_clicked();
  void on_tab1_im1f_radioButton_clicked();
  void on_tab1_im1i_radioButton_clicked();
  void on_tab1_im2b_radioButton_clicked();
  void on_tab1_im2f_radioButton_clicked();
  void on_tab1_im2i_radioButton_clicked();

  // add files to QListWidget
  void on_tab2_im1Add_pushButton_clicked();
  void on_tab2_im1Rmv_pushButton_clicked();

private:
  Ui::MainWindow *ui;

  QString func;
  QString im1;
  QString im2;
  QString out_dir;
  QString out_name;
};

#endif // MAINWINDOW_H
