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

  phase_ << "" << "";
  dtype_ << "" << "";
  func_     = "";
  outDir_   = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
  outName_  = "";
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
// tab-select:
// - print message to status-bar
// - synchronize data between tabs
// ============================================================================

void MainWindow::on_tabWidget_tabBarClicked(int index)
{
  // print message to status-bar
  QString mes;
  if      ( index==0 ) mes = "Overview, load existing state, storage settings";
  else if ( index==1 ) mes = "Select measure and image type(s)";
  else if ( index==2 ) mes = "Select images";
  else if ( index==3 ) mes = "Check/modify images";
  else if ( index==4 ) mes = "Run computation";
  statusBar()->showMessage(mes);

  // read storage directory and name
  if ( index>0 ) {
    outDir_  = ui->tab0_outdir_lineEdit->text();
    outName_ = ui->tab0_result_lineEdit->text();
  }

  // tab2: selectively enable, and fill
  if ( index==2 && func_!="" ) {
    ui->tab2_im0Phase_label   ->setText(phase_[0]);
    ui->tab2_im0Dtype_label   ->setText(dtype_[0]);
    ui->tab2_im0_listWidget   ->setEnabled(ui->tab1_im0_checkBox->isChecked());
    ui->tab2_im0Add_pushButton->setEnabled(ui->tab1_im0_checkBox->isChecked());
    ui->tab2_im0Rmv_pushButton->setEnabled(ui->tab1_im0_checkBox->isChecked());
    ui->tab2_im1Phase_label   ->setText(phase_[1]);
    ui->tab2_im1Dtype_label   ->setText(dtype_[1]);
    ui->tab2_im1_listWidget   ->setEnabled(ui->tab1_im1_checkBox->isChecked());
    ui->tab2_im1Add_pushButton->setEnabled(ui->tab1_im1_checkBox->isChecked());
    ui->tab2_im1Rmv_pushButton->setEnabled(ui->tab1_im1_checkBox->isChecked());
    ui->tab2_im1Up__pushButton->setEnabled(ui->tab1_im1_checkBox->isChecked());
    ui->tab2_im1Dwn_pushButton->setEnabled(ui->tab1_im1_checkBox->isChecked());
    ui->tab2_cp_pushButton    ->setEnabled(ui->tab1_im1_checkBox->isChecked());
  }
}

// ============================================================================
// tab0: load existing state
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

  // local variables
  QString category = ui->tab1_measure_treeWidget->currentItem()->parent()->text(0);
  QString measure  = ui->tab1_measure_treeWidget->currentItem()->text(0);
  bool    lnk      = false;
  int     im1      = 0;
  // lists to signal the allowed data-types
  std::vector<int> dtype { 0      , 0   , 0     };
  QStringList      label {"binary","int","float"};
  // allocate lists of pointers
  typedef void (MainWindow::*ptr)(bool);
  ptr            clear[2];
  QCheckBox     *check[2];
  QRadioButton  *radio[2][3];
  // pointers to dtype checkBoxes
  check[0]    = ui->tab1_im0_checkBox;
  check[1]    = ui->tab1_im1_checkBox;
  // pointers to dtype radioButtons
  radio[0][0] = ui->tab1_im0b_radioButton;
  radio[0][1] = ui->tab1_im0i_radioButton;
  radio[0][2] = ui->tab1_im0f_radioButton;
  radio[1][0] = ui->tab1_im1b_radioButton;
  radio[1][1] = ui->tab1_im1i_radioButton;
  radio[1][2] = ui->tab1_im1f_radioButton;
  // pointers to function that clears radioButtons
  clear[0]    = &MainWindow::on_tab1_im0_checkBox_toggled;
  clear[1]    = &MainWindow::on_tab1_im1_checkBox_toggled;

  // default: no information on function / phase / data-type
  func_ = "";
  for ( int i=0 ; i<2 ; i++ ) {
    phase_[i] = "";
    dtype_[i] = "";
  }

  // settings based on selected statistic
  if      ( category=="Spatial correlations" ) {
    if      ( measure=="2-point probability" ) {
      func_      = "S2";
      phase_[0] = "Phase";
      phase_[1] = "Phase";
      dtype     = {1,0,1};
      lnk       = true;
    }
    else if ( measure=="2-point cluster" ) {
      func_      = "S2";
      phase_[0] = "Clusters";
      phase_[1] = "Clusters";
      dtype     = {0,1,0};
      lnk       = true;
    }
    else if ( measure=="Lineal path" ) {
      func_      = "L" ;
      phase_[0] = "Phase";
      dtype     = {1,1,0};
      lnk       = true;
      im1       = -1;
    }
  }
  else if ( category=="Conditional spatial correlations" ) {
    if      ( measure=="2-point probability" ) {
      func_      = "W2";
      phase_[0] = "Phase";
      phase_[1] = "Weights";
      dtype     = {1,0,1};
      im1       = 1;
    }
    else if ( measure=="Collapsed 2-point probability" ) {
      func_      = "W2c";
      phase_[0] = "Phase";
      phase_[1] = "Weights";
      dtype     = {1,0,1};
      im1       = 1;
    }
  }

  // function not implemented: show warning
  if ( func_=="" )
    this->WIP();

  // show function in status bar
  QString mes = "Function to use: '%1'";
  statusBar()->showMessage(mes.arg(func_));

  // apply phase label
  for ( int i=0 ; i<2 ; i++ )
    check[i]->setText(phase_[i]);

  // selection of image 1: force-select
  check[0]->setEnabled(false);
  check[0]->setChecked(true );

  // selection of image 2: -1=disable ; 0=user-select ; 1=force-select
  if      ( im1==-1 ) {
    this->on_tab1_im1_checkBox_toggled(false);
    check[1]->setChecked (false);
    check[1]->setEnabled (false);
  }
  else if ( im1==0 ) {
    check[1]->setEnabled (true );
  }
  else if ( im1==1 ) {
    check[1]->setEnabled (false);
    check[1]->setChecked (true );
  }

   // clear dtype radioButton if one was selected that is no longer allowed
   for ( int i=0 ; i<2 ; i++ )
     for ( int j=0 ; j<3 ; j++ )
       if ( !dtype[j] && (radio[i][j])->isChecked() )
         (this->*clear[i])(false);

  // auto-fill radioButton if there is only one possibility
  if ( dtype[0]+dtype[1]+dtype[2]==1 )
    for ( int i=0 ; i<2 ; i++ )
      for ( int j=0 ; j<3 ; j++ )
        if ( dtype[j] && check[i] )
          radio[i][j]->setChecked(true);

  // enable/disable dtype radioButton based on allowed dtype(s)
  for ( int i=0 ; i<2 ; i++ )
    for ( int j=0 ; j<3 ; j++ )
      radio[i][j]->setEnabled((bool)dtype[j]);

  // optionally link image 2 to image 1
  if ( lnk ) {
    // - clear radioButtons for image 2
    this->on_tab1_im1_checkBox_toggled(false);
    // - disable radioButtons for image 2
    for ( int j=0 ; j<3 ; j++ )
      radio[1][j]->setEnabled(false);
    // - if image 2 is activated -> copy selection from image 1
    if ( ui->tab1_im1_checkBox->isChecked() )
      for ( int j=0 ; j<3 ; j++ )
        if ( radio[0][j]->isChecked() )
          radio[1][j]->setChecked(true);
  }

  // store dtype
  for ( int i=0 ; i<2 ; i++ )
    for ( int j=0 ; j<3 ; j++ )
      if ( radio[i][j]->isChecked() )
        dtype_.replace(i,label[j]);

}

// -----------------------------------------------------------------------------
// tab1: statistics selection list: optionally expand list
//       if deepest -> all further actions are unified in "tab1_selectStat()"
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
// tab1: type radioButtons: optionally check checkBox of the relevant image,
//       all further actions are unified in "tab1_selectStat()"
// -----------------------------------------------------------------------------

void MainWindow::on_tab1_im0b_radioButton_clicked()
{
  ui->tab1_im0_checkBox->setChecked(true);
  this->tab1_selectStat();
}

void MainWindow::on_tab1_im0f_radioButton_clicked()
{
  ui->tab1_im0_checkBox->setChecked(true);
  this->tab1_selectStat();
}

void MainWindow::on_tab1_im0i_radioButton_clicked()
{
  ui->tab1_im0_checkBox->setChecked(true);
  this->tab1_selectStat();
}

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

// -----------------------------------------------------------------------------
// tab1: select   image -> read allowed types from "tab1_selectStat()"
//       deselect image -> clear
// -----------------------------------------------------------------------------

void MainWindow::on_tab1_im0_checkBox_toggled(bool checked)
{
  if ( checked ) {
    this->tab1_selectStat();
  }
  else {
    ui->buttonGroup_tab1_im0->setExclusive(false);
    ui->tab1_im0b_radioButton->setChecked (false);
    ui->tab1_im0f_radioButton->setChecked (false);
    ui->tab1_im0i_radioButton->setChecked (false);
    ui->buttonGroup_tab1_im0->setExclusive(true );
  }
}

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

// =============================================================================
// tab2: add/remove/move files to/in QListWidgets
// =============================================================================

void MainWindow::tab2_selectFiles(QListWidget *list)
{
  QStringList filters;
  filters << "Image files (*.png *.xpm *.jpg *.jpeg *.tif *.tiff *.bmp)"
          << "Archives (*.mat *.npz)"
          << "Any files (*)";

  QFileDialog dialog(this);
  dialog.setFileMode   (QFileDialog::ExistingFiles);
  dialog.setOption     (QFileDialog::HideNameFilterDetails,false);
  dialog.setDirectory  (outDir_);
  dialog.setNameFilters(filters);
  dialog.setViewMode   (QFileDialog::List);

  QStringList fileNames;
  if (dialog.exec())
      fileNames = dialog.selectedFiles();

  QDir dir(outDir_);
  for ( int i=0 ; i<fileNames.size() ; i++ )
    fileNames[i] = dir.relativeFilePath(fileNames[i]);

  list->addItems(fileNames);
}

// -----------------------------------------------------------------------------

void MainWindow::on_tab2_im0Add_pushButton_clicked()
{
  this->tab2_selectFiles(ui->tab2_im0_listWidget);
}

// -----------------------------------------------------------------------------

void MainWindow::on_tab2_im1Add_pushButton_clicked()
{
  this->tab2_selectFiles(ui->tab2_im1_listWidget);
}

// -----------------------------------------------------------------------------

void MainWindow::on_tab2_im0Rmv_pushButton_clicked()
{
  QList<QListWidgetItem*> items = ui->tab2_im0_listWidget->selectedItems();
  foreach (QListWidgetItem *item, items)
    delete ui->tab2_im0_listWidget->takeItem(ui->tab2_im0_listWidget->row(item));
}

// -----------------------------------------------------------------------------

void MainWindow::on_tab2_im1Rmv_pushButton_clicked()
{
  QList<QListWidgetItem*> items = ui->tab2_im1_listWidget->selectedItems();
  foreach (QListWidgetItem *item, items)
    delete ui->tab2_im1_listWidget->takeItem(ui->tab2_im1_listWidget->row(item));
}

// -----------------------------------------------------------------------------

void MainWindow::on_tab2_cp_pushButton_clicked()
{
  if ( ui->tab2_im1_listWidget->count()>0 ) {
    QMessageBox::warning(\
      this,\
      tr("GooseEYE"),\
      tr("Only allowed if right list is empty."),\
      QMessageBox::Ok,\
      QMessageBox::Ok\
    );
  }

  for ( int i=0 ; i<ui->tab2_im0_listWidget->count() ; i++ )
    ui->tab2_im1_listWidget->addItem(ui->tab2_im0_listWidget->item(i)->text());
}

// -----------------------------------------------------------------------------

void MainWindow::on_tab2_im1Up__pushButton_clicked()
{
  QListWidget *list = ui->tab2_im1_listWidget;

  QList<QListWidgetItem*> items = list->selectedItems();

  std::vector<int> row(items.size());

  for ( int i=0 ; i<items.size() ; i++ )
    row[i] = list->row(items[i]);

  for ( int i=0 ; i<items.size() ; i++ )
    if ( row[i]==0 )
      return;

  std::sort(row.begin(),row.end());

  for ( int i=0 ; i<items.size() ; i++ )
    list->insertItem(row[i]-1,list->takeItem(row[i]));

  for ( int i=0 ; i<list->count() ; i++ )
    list->item(i)->setSelected(false);

  for ( int i=0 ; i<items.size() ; i++ )
    list->item(row[i]-1)->setSelected(true);
}

// -----------------------------------------------------------------------------

void MainWindow::on_tab2_im1Dwn_pushButton_clicked()
{
  QListWidget *list = ui->tab2_im1_listWidget;

  QList<QListWidgetItem*> items = list->selectedItems();

  std::vector<int> row(items.size());

  for ( int i=0 ; i<items.size() ; i++ )
    row[i] = list->row(items[i]);

  for ( int i=0 ; i<items.size() ; i++ )
    if ( row[i]==list->count()-1 )
      return;

  std::sort(row.begin(),row.end(), [](int a, int b) { return a>b; });

  for ( int i=0 ; i<items.size() ; i++ )
    list->insertItem(row[i]+1,list->takeItem(row[i]));

  for ( int i=0 ; i<list->count() ; i++ )
    list->item(i)->setSelected(false);

  for ( int i=0 ; i<items.size() ; i++ )
    list->item(row[i]+1)->setSelected(true);
}

