#include "gui_mainwindow.h"
#include "ui_gui_mainwindow.h"

// ============================================================================
// basic constructor
// ============================================================================

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  QString func     = "";
  QString im1      = "";
  QString im2      = "";
  QString out_dir  = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
  QString out_name = "";
}

// ============================================================================
// destructor
// ============================================================================

MainWindow::~MainWindow()
{
  delete ui;
}

// ============================================================================
// pop-up to signal work in progress
// ============================================================================

void MainWindow::WIP()
{
  QMessageBox::warning(\
    this,\
    tr("GooseEYE"),\
    tr("Work in progress.\nThis function has not been completed."),\
    QMessageBox::Ok,\
    QMessageBox::Ok\
  );
}

// ============================================================================
// tab-select: print message to status-bar
// ============================================================================

void MainWindow::on_tabWidget_tabBarClicked(int index)
{
  QString mes;
  int i;

  if      ( index==0 ) mes = "Overview, load pre-existing state";
  else if ( index==1 ) mes = "Select measure and image type(s)";
  else if ( index==2 ) mes = "Select images";
  else if ( index==3 ) mes = "Check/modify images";
  else if ( index==4 ) mes = "Run computation";

  statusBar()->showMessage(mes);

  if ( index>0 ) {
    out_dir  = ui->tab0_outdir_lineEdit->text();
    out_name = ui->tab0_result_lineEdit->text();
  }

  // tab2: selectively enable, and fill
  // ----------------------------------

  if ( index==2 && func!="" ) {

    QComboBox    *combo;
    QCheckBox    *check;
    QLabel       *label;
    QRadioButton *radio[3];
    QString       tmp   = "binary,float,int";
    QStringList   types = tmp.split(",");

    for ( int im=1 ; im<=2 ; im++ )
    {
      if      ( im==1 ) {
        combo    = ui->tab2_im1_comboBox;
        label    = ui->tab2_im1_label;
        check    = ui->tab1_im1_checkBox;
        radio[0] = ui->tab1_im1b_radioButton;
        radio[1] = ui->tab1_im1f_radioButton;
        radio[2] = ui->tab1_im1i_radioButton;
      }
      else if ( im==2 ) {
        combo    = ui->tab2_im2_comboBox;
        label    = ui->tab2_im2_label;
        check    = ui->tab1_im2_checkBox;
        radio[0] = ui->tab1_im2b_radioButton;
        radio[1] = ui->tab1_im2f_radioButton;
        radio[2] = ui->tab1_im2i_radioButton;
      }

      if ( check->isChecked() )
      {
        combo->clear();
        combo->setEnabled(true);
        label->setText(check->text());

        int j=0;
        for ( int i=0 ; i<types.size() ; i++ ) {
          if ( radio[i]->isEnabled() ) {
            combo->addItem(types[i]);
            if ( radio[i]->isChecked() )
              combo->setCurrentIndex(j);
            j++;
          }
        }
      }
    }
  }

}

// ============================================================================
// tab0: load pre-existing state
// ============================================================================

void MainWindow::on_tab0_load_pushButton_clicked()
{
  this->WIP(); return;

  QString fname = QFileDialog::getOpenFileName(\
    this,\
    tr("Open File"),\
    QStandardPaths::writableLocation(QStandardPaths::HomeLocation),\
    tr("State Files (*.json *.JSON)")\
  );

}

// =============================================================================
// tab0: select output folder / file
// =============================================================================

void MainWindow::on_tab0_outdir_pushButton_clicked()
{
  QString dir = QFileDialog::getExistingDirectory(\
    this,\
    tr("Open Directory"),\
    QStandardPaths::writableLocation(QStandardPaths::HomeLocation),\
    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks\
  );

  ui->tab0_outdir_lineEdit->setText(dir);

  if ( ui->tab0_result_lineEdit->text()=="" )
    ui->tab0_result_lineEdit->setText("GooseEYE.json");
}

// -----------------------------------------------------------------------------

void MainWindow::on_tab0_result_pushButton_clicked()
{
  QString fname = QFileDialog::getOpenFileName(\
    this,\
    tr("Open File"),\
    QStandardPaths::writableLocation(QStandardPaths::HomeLocation),\
    tr("State Files (*.json *.JSON)")\
  );

  if ( ui->tab0_outdir_lineEdit->text()!="" )
    return;

  QFileInfo finfo(fname);
  QDir      fdir = finfo.dir();

  ui->tab0_outdir_lineEdit->setText(fdir.dirName());
}

// ============================================================================
// tab1: selection of statistic -> controls possible images and data types
//       (function is back-end of several signals below)
// ============================================================================

void MainWindow::tab1_selectStat(void)
{
  if ( ui->tab1_measure_treeWidget->currentItem()->childCount()>0 )
    return;

  QString category = ui->tab1_measure_treeWidget->currentItem()->parent()->text(0);
  QString measure  = ui->tab1_measure_treeWidget->currentItem()->text(0);
  bool    tb       = false;
  bool    tf       = false;
  bool    ti       = false;
  bool    lnk      = false;
  int     im2      = 0;
  QString lab1     = "Image 1";
  QString lab2     = "Image 2";

  // settings based on selected statistic
  if      ( category=="Spatial correlations" ) {
    if      ( measure=="2-point probability" ) {
      func = "S2";
      lab1 = "Phase";
      lab2 = "Phase";
      tb   = true;
      tf   = true;
      lnk  = true;
    }
    else if ( measure=="2-point cluster" ) {
      func = "S2";
      lab1 = "Clusters";
      lab2 = "Clusters";
      ti   = true;
      lnk  = true;
    }
    else if ( measure=="Lineal path" ) {
      func = "L" ;
      lab1 = "Phase";
      tb   = true;
      ti   = true;
      lnk  = true;
      im2  = -1;
    }
  }
  else if ( category=="Conditional spatial correlations" ) {
    if      ( measure=="2-point probability" ) {
      func = "W2";
      lab1 = "Phase";
      lab2 = "Weights";
      tb   = true;
      tf   = true;
      im2  = 1;
    }
    else if ( measure=="Collapsed 2-point probability" ) {
      func = "W2c";
      lab1 = "Phase";
      lab2 = "Weights";
      tb   = true;
      tf   = true;
      im2  = 1;
    }
  }
  else if ( category=="Prediction from conditional statistics" ) {
    this->WIP();
  }

  // show function in status bar
  QString mes = "Function to use: '%1'";
  statusBar()->showMessage(mes.arg(func));

  // apply label
  ui->tab1_im1_checkBox->setText(lab1);
  ui->tab1_im2_checkBox->setText(lab2);

  // selection of image 1: force-select
  ui->tab1_im1_checkBox->setEnabled(false);
  ui->tab1_im1_checkBox->setChecked(true );

  // selection of image 2: -1=disable ; 0=user-select ; 1=force-select
  if      ( im2==-1 ) {
    this->on_tab1_im2_checkBox_toggled(false);
    ui->tab1_im2_checkBox->setChecked (false);
    ui->tab1_im2_checkBox->setEnabled (false);
  }
  else if ( im2==0 ) {
    ui->tab1_im2_checkBox->setEnabled (true);
  }
  else if ( im2==1 ) {
    ui->tab1_im2_checkBox->setEnabled (false);
    ui->tab1_im2_checkBox->setChecked (true );
  }

  // clear type-radioButton if one was selected that is no longer allowed
  if ( !tb && ui->tab1_im1b_radioButton->isChecked() )
    this->on_tab1_im1_checkBox_toggled(false);
  if ( !tb && ui->tab1_im2b_radioButton->isChecked() )
    this->on_tab1_im2_checkBox_toggled(false);
  if ( !tf && ui->tab1_im1f_radioButton->isChecked() )
    this->on_tab1_im1_checkBox_toggled(false);
  if ( !tf && ui->tab1_im2f_radioButton->isChecked() )
    this->on_tab1_im2_checkBox_toggled(false);
  if ( !ti && ui->tab1_im1i_radioButton->isChecked() )
    this->on_tab1_im1_checkBox_toggled(false);
  if ( !ti && ui->tab1_im2i_radioButton->isChecked() )
    this->on_tab1_im2_checkBox_toggled(false);

  // auto-fill radio-button if there is only one possibility
  if ( tb && !(ti || tf) )
    ui->tab1_im1b_radioButton->setChecked(true);
  if ( tf && !(tb || ti) )
    ui->tab1_im1f_radioButton->setChecked(true);
  if ( ti && !(tb || tf) )
    ui->tab1_im1i_radioButton->setChecked(true);

  // enable/disable type-radioButton based on settings above
  ui->tab1_im1b_radioButton->setEnabled(tb);
  ui->tab1_im2b_radioButton->setEnabled(tb);
  ui->tab1_im1f_radioButton->setEnabled(tf);
  ui->tab1_im2f_radioButton->setEnabled(tf);
  ui->tab1_im1i_radioButton->setEnabled(ti);
  ui->tab1_im2i_radioButton->setEnabled(ti);

  // optionally lnk image 2 to image 1
  if ( !lnk )
    return;

  // - clear radioButtons
  this->on_tab1_im2_checkBox_toggled(false);
  // - disable radioButtons
  ui->tab1_im2b_radioButton->setEnabled(false);
  ui->tab1_im2f_radioButton->setEnabled(false);
  ui->tab1_im2i_radioButton->setEnabled(false);
  // - if image 2 is activated -> copy selection from image 1
  if ( ui->tab1_im2_checkBox->isChecked() ) {
    if      ( ui->tab1_im1b_radioButton->isChecked() )
      ui->tab1_im2b_radioButton->setChecked(true);
    else if ( ui->tab1_im1f_radioButton->isChecked() )
      ui->tab1_im2f_radioButton->setChecked(true);
    else if ( ui->tab1_im1i_radioButton->isChecked() )
      ui->tab1_im2i_radioButton->setChecked(true);
   }

}

// -----------------------------------------------------------------------------
// tab1: statistics selection list
//       if the current item is not the deepest: expand the list
// -----------------------------------------------------------------------------

void MainWindow::on_tab1_measure_treeWidget_clicked(const QModelIndex &index)
{

  if ( ui->tab1_measure_treeWidget->currentItem()->childCount()>0 ) {
    if ( !ui->tab1_measure_treeWidget->currentItem()->isExpanded() ) {
      ui->tab1_measure_treeWidget->collapseAll();
      ui->tab1_measure_treeWidget->currentItem()->setExpanded(true);
    }
    return;
  }
  this->tab1_selectStat();
}

// -----------------------------------------------------------------------------
// tab1: type radioButtons: optionally check image to which it belong,
//       then to everything in "tab1_selectStat()"
// -----------------------------------------------------------------------------

void MainWindow::on_tab1_im1b_radioButton_clicked()
{
  ui->tab1_im1_checkBox->setChecked(true);
  this->tab1_selectStat();
}

void MainWindow::on_tab1_im1f_radioButton_clicked()
{
  ui->tab1_im1_checkBox->setChecked(true);
  this->tab1_selectStat();
}

void MainWindow::on_tab1_im1i_radioButton_clicked()
{
  ui->tab1_im1_checkBox->setChecked(true);
  this->tab1_selectStat();
}

void MainWindow::on_tab1_im2b_radioButton_clicked()
{
  ui->tab1_im2_checkBox->setChecked(true);
  this->tab1_selectStat();
}

void MainWindow::on_tab1_im2f_radioButton_clicked()
{
  ui->tab1_im2_checkBox->setChecked(true);
  this->tab1_selectStat();
}

void MainWindow::on_tab1_im2i_radioButton_clicked()
{
  ui->tab1_im2_checkBox->setChecked(true);
  this->tab1_selectStat();
}

// -----------------------------------------------------------------------------
// tab1: select   image -> read allowed types from "tab1_selectStat()"
//       deselect image -> clear
// -----------------------------------------------------------------------------

void MainWindow::on_tab1_im1_checkBox_toggled(bool checked)
{
  if ( checked ) {
    this->tab1_selectStat();
  }
  else {
    ui->buttonGroup_tab1_im1->setExclusive(false);
    ui->tab1_im1b_radioButton->setChecked (false);
    ui->tab1_im1f_radioButton->setChecked (false);
    ui->tab1_im1i_radioButton->setChecked (false);
    ui->buttonGroup_tab1_im1->setExclusive(true );
  }
}

void MainWindow::on_tab1_im2_checkBox_toggled(bool checked)
{
  if ( checked ) {
    this->tab1_selectStat();
  }
  else {
    ui->buttonGroup_tab1_im2->setExclusive(false);
    ui->tab1_im2b_radioButton->setChecked (false);
    ui->tab1_im2f_radioButton->setChecked (false);
    ui->tab1_im2i_radioButton->setChecked (false);
    ui->buttonGroup_tab1_im2->setExclusive(true );
  }
}

// =============================================================================
// tab2: add/remove/move files to/in QListWidget
// =============================================================================

void MainWindow::on_tab2_im1Add_pushButton_clicked()
{
  QFileDialog dialog(this);
  dialog.setFileMode(QFileDialog::ExistingFiles);
  dialog.setNameFilter(tr("Images (*.png *.jpg *.jpeg *.tif *tiff *.mat *.npz)"));
  dialog.setViewMode(QFileDialog::List);
  QStringList fileNames;
  if (dialog.exec())
      fileNames = dialog.selectedFiles();

  ui->tab2_im1_listWidget->addItems(fileNames);
}



void MainWindow::on_tab2_im1Rmv_pushButton_clicked()
{

}
