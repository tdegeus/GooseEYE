#include "mainwindow.h"
#include "ui_mainwindow.h"

QString cur_dir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::WIP()
{
  QMessageBox::warning(this, tr("GooseEYE"),
    tr("Work in progress.\nThis function has not been completed."),
    QMessageBox::Ok,QMessageBox::Ok
  );
}

void MainWindow::on_tab1_load_pushButton_clicked()
{
  this->WIP(); return;

  QString fname = QFileDialog::getOpenFileName(this,tr("Open File"), cur_dir, tr("Image Files (*.json *.JSON)"));
}
