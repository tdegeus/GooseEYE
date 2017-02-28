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

private slots:

  void on_tab1_load_pushButton_clicked();

  void on_tab2_measure_treeWidget_clicked(const QModelIndex &index);

private:
  Ui::MainWindow *ui;
  QString cur_dir;
};

#endif // MAINWINDOW_H
