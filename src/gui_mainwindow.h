#ifndef GUI_MAINWINDOW_H
#define GUI_MAINWINDOW_H

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

  void tab2_selectStat(void);

private slots:

  void on_tab1_load_pushButton_clicked();

  void on_tab2_measure_treeWidget_clicked(const QModelIndex &index);

  void on_tab2_im1b_radioButton_clicked();
  void on_tab2_im1f_radioButton_clicked();
  void on_tab2_im1i_radioButton_clicked();
  void on_tab2_im2b_radioButton_clicked();
  void on_tab2_im2f_radioButton_clicked();
  void on_tab2_im2i_radioButton_clicked();

  void on_tab2_im1_checkBox_toggled(bool checked);
  void on_tab2_im2_checkBox_toggled(bool checked);

private:
  Ui::MainWindow *ui;
  QString cur_dir;
  QString func;
  QString type1;
  QString type2;
};

#endif // MAINWINDOW_H
