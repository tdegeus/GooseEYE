#include "gui_mainwindow.h"
#include "ui_gui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  QString cur_dir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);

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


void MainWindow::on_tab2_measure_treeWidget_clicked(const QModelIndex &index)
{
  // if the current item is not the deepest: expand the list
  if ( ui->tab2_measure_treeWidget->currentItem()->childCount()>0 ) {
    if ( !ui->tab2_measure_treeWidget->currentItem()->isExpanded() ) {
      ui->tab2_measure_treeWidget->collapseAll();
      ui->tab2_measure_treeWidget->currentItem()->setExpanded(true);
    }
    return;
  }

  // store selected statistic
  QString category = ui->tab2_measure_treeWidget->currentItem()->parent()->text(0);
  QString measure  = ui->tab2_measure_treeWidget->currentItem()->text(0);
  if      ( category=="Spatial correlations" ) {
    if      ( measure=="2-point probability" ) {
      statusBar()->showMessage("1a");
    }
    else if ( measure=="2-point cluster" ) {
      statusBar()->showMessage("1b");
    }
    else if ( measure=="Lineal path" ) {
      statusBar()->showMessage("1c");
    }
    else {
      throw std::out_of_range("Child not found");
    }
  }
  else if ( category=="Conditional spatial correlations" ) {
    statusBar()->showMessage("2");
  }
  else if ( category=="Predictions" ) {
    statusBar()->showMessage("3");
  }
  else {
    throw std::out_of_range("Parent not found");
  }

//  statusBar()->showMessage(measure);
//  else {
//
//  }

//    case QString(""):
//      statusBar()->showMessage("2");
//    case QString("Predictions"):
//      statusBar()->showMessage("3");
//  }

//  if ( ui->tab1_load_label)
//  stat = ui->tab2_measure_treeWidget->currentItem()->text(0);

//  statusBar()->showMessage(stat);

//  statusBar()->showMessage(QString("Clicked: %1").arg(i));
}

