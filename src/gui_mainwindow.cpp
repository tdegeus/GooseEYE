#include "gui_mainwindow.h"
#include "ui_gui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  QString cur_dir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
  QString func    = "";

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

// ============================================================================
// selection of statistic -> controls possible images and data types
// (function is back-end of several signals below)
// ============================================================================

void MainWindow::tab2_selectStat(void)
{
  if ( ui->tab2_measure_treeWidget->currentItem()->childCount()>0 )
    return;

  QString category = ui->tab2_measure_treeWidget->currentItem()->parent()->text(0);
  QString measure  = ui->tab2_measure_treeWidget->currentItem()->text(0);
  bool    tb       = false;
  bool    tf       = false;
  bool    ti       = false;
  bool    lnk      = false;
  int     im2      = 0;

  // settings based on selected statistic
  if      ( category=="Spatial correlations" ) {
    if      ( measure=="2-point probability" ) {
      statusBar()->showMessage("Function to use: 'S2'");
      func = "S2";
      tb   = true;
      tf   = true;
      lnk  = true;
    }
    else if ( measure=="2-point cluster" ) {
      statusBar()->showMessage("Function to use: 'S2'");
      func = "S2";
      ti   = true;
      lnk  = true;
    }
    else if ( measure=="Lineal path" ) {
      statusBar()->showMessage("Function to use: 'L'");
      func = "L" ;
      tb   = true;
      ti   = true;
      lnk  = true;
      im2  = -1;
    }
  }
  else if ( category=="Conditional spatial correlations" ) {
    if      ( measure=="2-point probability" ) {
      statusBar()->showMessage("Function to use: 'W2'");
      func = "W2";
      tb   = true;
      tf   = true;
      im2  = 1;
    }
    else if ( measure=="Collapsed 2-point probability" ) {
      statusBar()->showMessage("Function to use: 'W2c'");
      func = "W2c";
      tb   = true;
      tf   = true;
      im2  = 1;
    }
  }
  else if ( category=="Prediction from conditional statistics" ) {
    this->WIP();
  }

  // selections of image 2: -1=disable ; 0=user-select ; 1=force-select
  if      ( im2==-1 ) {
    this->on_tab2_im2_checkBox_toggled(false);
    ui->tab2_im2_checkBox->setChecked (false);
    ui->tab2_im2_checkBox->setEnabled (false);
  }
  else if ( im2==0 ) {
    ui->tab2_im2_checkBox->setEnabled (true);
  }
  else if ( im2==1 ) {
    ui->tab2_im2_checkBox->setEnabled (false);
    ui->tab2_im2_checkBox->setChecked (true );
  }

  // clear type-radioButton if one was selected that is no longer allowed
  if ( !tb && ui->tab2_im1b_radioButton->isChecked() )
    this->on_tab2_im1_checkBox_toggled(false);
  if ( !tb && ui->tab2_im2b_radioButton->isChecked() )
    this->on_tab2_im2_checkBox_toggled(false);
  if ( !tf && ui->tab2_im1f_radioButton->isChecked() )
    this->on_tab2_im1_checkBox_toggled(false);
  if ( !tf && ui->tab2_im2f_radioButton->isChecked() )
    this->on_tab2_im2_checkBox_toggled(false);
  if ( !ti && ui->tab2_im1i_radioButton->isChecked() )
    this->on_tab2_im1_checkBox_toggled(false);
  if ( !ti && ui->tab2_im2i_radioButton->isChecked() )
    this->on_tab2_im2_checkBox_toggled(false);

  // enable/disable type-radioButton based on settings above
  ui->tab2_im1b_radioButton->setEnabled(tb);
  ui->tab2_im2b_radioButton->setEnabled(tb);
  ui->tab2_im1f_radioButton->setEnabled(tf);
  ui->tab2_im2f_radioButton->setEnabled(tf);
  ui->tab2_im1i_radioButton->setEnabled(ti);
  ui->tab2_im2i_radioButton->setEnabled(ti);

  // optionally lnk image 2 to image 1
  if ( !lnk )
    return;

  // - clear radioButtons
  this->on_tab2_im2_checkBox_toggled(false);
  // - disable radioButtons
  ui->tab2_im2b_radioButton->setEnabled(false);
  ui->tab2_im2f_radioButton->setEnabled(false);
  ui->tab2_im2i_radioButton->setEnabled(false);
  // - if image 2 is activated -> copy selection from image 1
  if ( ui->tab2_im2_checkBox->isChecked() ) {
    if      ( ui->tab2_im1b_radioButton->isChecked() )
      ui->tab2_im2b_radioButton->setChecked(true);
    else if ( ui->tab2_im1f_radioButton->isChecked() )
      ui->tab2_im2f_radioButton->setChecked(true);
    else if ( ui->tab2_im1i_radioButton->isChecked() )
      ui->tab2_im2i_radioButton->setChecked(true);
   }

}

// -----------------------------------------------------------------------------
// statistics selection list
// if the current item is not the deepest: expand the list
// -----------------------------------------------------------------------------

void MainWindow::on_tab2_measure_treeWidget_clicked(const QModelIndex &index)
{

  if ( ui->tab2_measure_treeWidget->currentItem()->childCount()>0 ) {
    if ( !ui->tab2_measure_treeWidget->currentItem()->isExpanded() ) {
      ui->tab2_measure_treeWidget->collapseAll();
      ui->tab2_measure_treeWidget->currentItem()->setExpanded(true);
    }
    return;
  }
  this->tab2_selectStat();
}

// -----------------------------------------------------------------------------
// type radioButtons: optionally check image to which it belong,
// then to everything in "tab2_selectStat()"
// -----------------------------------------------------------------------------

void MainWindow::on_tab2_im1b_radioButton_clicked()
{
  ui->tab2_im1_checkBox->setChecked(true);
  this->tab2_selectStat();
}

void MainWindow::on_tab2_im1f_radioButton_clicked()
{
  ui->tab2_im1_checkBox->setChecked(true);
  this->tab2_selectStat();
}

void MainWindow::on_tab2_im1i_radioButton_clicked()
{
  ui->tab2_im1_checkBox->setChecked(true);
  this->tab2_selectStat();
}

void MainWindow::on_tab2_im2b_radioButton_clicked()
{
  ui->tab2_im2_checkBox->setChecked(true);
  this->tab2_selectStat();
}

void MainWindow::on_tab2_im2f_radioButton_clicked()
{
  ui->tab2_im2_checkBox->setChecked(true);
  this->tab2_selectStat();
}

void MainWindow::on_tab2_im2i_radioButton_clicked()
{
  ui->tab2_im2_checkBox->setChecked(true);
  this->tab2_selectStat();
}

// -----------------------------------------------------------------------------
// select   image -> read allowed types from "tab2_selectStat()"
// deselect image -> clear
// -----------------------------------------------------------------------------

void MainWindow::on_tab2_im1_checkBox_toggled(bool checked)
{
  if ( checked ) {
    this->tab2_selectStat();
  }
  else {
    ui->buttonGroup_tab2_im1->setExclusive(false);
    ui->tab2_im1b_radioButton->setChecked (false);
    ui->tab2_im1f_radioButton->setChecked (false);
    ui->tab2_im1i_radioButton->setChecked (false);
    ui->buttonGroup_tab2_im1->setExclusive(true );
  }
}

void MainWindow::on_tab2_im2_checkBox_toggled(bool checked)
{
  if ( checked ) {
    this->tab2_selectStat();
  }
  else {
    ui->buttonGroup_tab2_im2->setExclusive(false);
    ui->tab2_im2b_radioButton->setChecked (false);
    ui->tab2_im2f_radioButton->setChecked (false);
    ui->tab2_im2i_radioButton->setChecked (false);
    ui->buttonGroup_tab2_im2->setExclusive(true );
  }
}

