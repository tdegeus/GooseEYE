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

  connect(ui->tab1_im0b_radioButton,SIGNAL(clicked(bool)),this,SLOT(tab1_selectStat()));
  connect(ui->tab1_im0i_radioButton,SIGNAL(clicked(bool)),this,SLOT(tab1_selectStat()));
  connect(ui->tab1_im0f_radioButton,SIGNAL(clicked(bool)),this,SLOT(tab1_selectStat()));
  connect(ui->tab1_im1b_radioButton,SIGNAL(clicked(bool)),this,SLOT(tab1_selectStat()));
  connect(ui->tab1_im1i_radioButton,SIGNAL(clicked(bool)),this,SLOT(tab1_selectStat()));
  connect(ui->tab1_im1f_radioButton,SIGNAL(clicked(bool)),this,SLOT(tab1_selectStat()));

  connect(ui->tab3_im_comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(tab3_syncImage()));
  connect(ui->tab3_im_comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(tab3_syncPhase()));

  QGraphicsView *view1 = ui->tab3_image_graphicsView;
  QGraphicsView *view2 = ui->tab3_phase_graphicsView;

  connect(view1->horizontalScrollBar(), SIGNAL(valueChanged(int)), view2->horizontalScrollBar(), SLOT(setValue(int)));
  connect(view2->horizontalScrollBar(), SIGNAL(valueChanged(int)), view1->horizontalScrollBar(), SLOT(setValue(int)));
  connect(view1->verticalScrollBar  (), SIGNAL(valueChanged(int)), view2->verticalScrollBar  (), SLOT(setValue(int)));
  connect(view2->verticalScrollBar  (), SIGNAL(valueChanged(int)), view1->verticalScrollBar  (), SLOT(setValue(int)));

  connect(ui->tab3_phaseLow_spinBox,SIGNAL(editingFinished()),this,SLOT(tab3_syncPhase()));
  connect(ui->tab3_phaseHgh_spinBox,SIGNAL(editingFinished()),this,SLOT(tab3_syncPhase()));
  connect(ui->tab3_mask1Low_spinBox,SIGNAL(editingFinished()),this,SLOT(tab3_syncPhase()));
  connect(ui->tab3_mask1Hgh_spinBox,SIGNAL(editingFinished()),this,SLOT(tab3_syncPhase()));
  connect(ui->tab3_mask2Low_spinBox,SIGNAL(editingFinished()),this,SLOT(tab3_syncPhase()));
  connect(ui->tab3_mask2Hgh_spinBox,SIGNAL(editingFinished()),this,SLOT(tab3_syncPhase()));
  connect(ui->tab3_mask3Low_spinBox,SIGNAL(editingFinished()),this,SLOT(tab3_syncPhase()));
  connect(ui->tab3_mask3Hgh_spinBox,SIGNAL(editingFinished()),this,SLOT(tab3_syncPhase()));
  connect(ui->tab3_rowLow_spinBox  ,SIGNAL(editingFinished()),this,SLOT(tab3_syncPhase()));
  connect(ui->tab3_rowHgh_spinBox  ,SIGNAL(editingFinished()),this,SLOT(tab3_syncPhase()));
  connect(ui->tab3_colLow_spinBox  ,SIGNAL(editingFinished()),this,SLOT(tab3_syncPhase()));
  connect(ui->tab3_colHgh_spinBox  ,SIGNAL(editingFinished()),this,SLOT(tab3_syncPhase()));

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

  QCheckBox *check[2];
  check[0] = ui->tab1_im0_checkBox;
  check[1] = ui->tab1_im1_checkBox;

  // tab3: selectively enable and fill
  if ( index==3 && func_!="" ) {
    ui->tab3_set_comboBox->clear();
    ui->tab3_set_comboBox->setEnabled(true);

    QString name = "%1: %2 (%3)";
    for ( int i=0 ; i<2 ; i++ )
      if ( check[i]->isChecked() )
        ui->tab3_set_comboBox->addItem(name.arg(QString::number(i+1),phase_[i],dtype_[i]));

    ui->tab3_setNext_pushButton->setEnabled(check[0]->isChecked() && check[1]->isChecked());
    ui->tab3_setPrev_pushButton->setEnabled(check[0]->isChecked() && check[1]->isChecked());

    this->on_tab3_set_comboBox_currentIndexChanged(0);

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

  if ( dir.size()==0 )
    return;

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

  if ( fname.size()==0 )
    return;

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
      if ( !dtype[j] && radio[i][j]->isChecked() )
        (this->*clear[i])(false);

  // auto-fill radioButton if there is only one possibility
  if ( dtype[0]+dtype[1]+dtype[2]==1 )
    for ( int i=0 ; i<2 ; i++ )
      for ( int j=0 ; j<3 ; j++ )
        if ( dtype[j] && check[i]->isChecked() )
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

// =============================================================================
// tab3: define "set" of images, based on statistic; create list of files
// =============================================================================

void MainWindow::on_tab3_set_comboBox_currentIndexChanged(int index)
{
  if ( index<0 )
    return;

  QListWidget *list[2];
  list[0] = ui->tab2_im0_listWidget;
  list[1] = ui->tab2_im1_listWidget;

  ui->tab3_im_comboBox->clear();
  ui->tab3_im_comboBox->setEnabled(true);

  if ( list[index]->count()==0 )
    return;

  for ( int i=0 ; i<list[index]->count() ; i++ )
    ui->tab3_im_comboBox->addItem(list[index]->item(i)->text());

  ui->tab3_imPrev_pushButton ->setEnabled(list[index]->count()>1);
  ui->tab3_imNext_pushButton ->setEnabled(list[index]->count()>1);

  ui->tab3_image_graphicsView->setEnabled(true);
  ui->tab3_phase_graphicsView->setEnabled(true);
  ui->tab3_zoom_slider       ->setEnabled(true);
  ui->tab3_zoomIn__pushButton->setEnabled(true);
  ui->tab3_zoomOut_pushButton->setEnabled(true);
  ui->tab3_phaseLow_spinBox  ->setEnabled(true);
  ui->tab3_phaseHgh_spinBox  ->setEnabled(true);
  ui->tab3_mask1Low_spinBox  ->setEnabled(true);
  ui->tab3_mask1Hgh_spinBox  ->setEnabled(true);
  ui->tab3_mask2Low_spinBox  ->setEnabled(true);
  ui->tab3_mask2Hgh_spinBox  ->setEnabled(true);
  ui->tab3_mask3Low_spinBox  ->setEnabled(true);
  ui->tab3_mask3Hgh_spinBox  ->setEnabled(true);
  ui->tab3_rowHgh_spinBox    ->setEnabled(true);
  ui->tab3_rowLow_spinBox    ->setEnabled(true);
  ui->tab3_colHgh_spinBox    ->setEnabled(true);
  ui->tab3_colLow_spinBox    ->setEnabled(true);

}

// -----------------------------------------------------------------------------

void MainWindow::on_tab3_setPrev_pushButton_clicked()
{
  int i = ui->tab3_set_comboBox->currentIndex();
  if ( i>0 )
    ui->tab3_set_comboBox->setCurrentIndex(i-1);
}

// -----------------------------------------------------------------------------

void MainWindow::on_tab3_setNext_pushButton_clicked()
{
  int i = ui->tab3_set_comboBox->currentIndex();
  if ( i<ui->tab3_set_comboBox->count()-1 )
    ui->tab3_set_comboBox->setCurrentIndex(i+1);
}

// -----------------------------------------------------------------------------

void MainWindow::on_tab3_imPrev_pushButton_clicked()
{
  int i = ui->tab3_im_comboBox->currentIndex();
  if ( i>0 )
    ui->tab3_im_comboBox->setCurrentIndex(i-1);
}

// -----------------------------------------------------------------------------

void MainWindow::on_tab3_imNext_pushButton_clicked()
{
  int i = ui->tab3_im_comboBox->currentIndex();
  if ( i<ui->tab3_im_comboBox->count()-1 )
    ui->tab3_im_comboBox->setCurrentIndex(i+1);
}

// ============================================================================
// tab3: view the original image, set the selection fields
// ============================================================================

double MainWindow::tab3_scaleImage(void)
{
  double wdthView  = (double)ui->tab3_image_graphicsView->width ();
  double hghtView  = (double)ui->tab3_image_graphicsView->height();
  double wdthImage = (double)ui->tab3_colHgh_spinBox->maximum();
  double hghtImage = (double)ui->tab3_rowHgh_spinBox->maximum();
  double zoomScale = pow(1.066,(double)ui->tab3_zoom_slider->sliderPosition());
  double wdth      = zoomScale*wdthView/wdthImage;
  double hght      = zoomScale*hghtView/hghtImage;

  return 0.95*std::min(wdth,hght);
}

void MainWindow::tab3_readImage(void)
{
  // load the image
  QString fname = QDir(outDir_).filePath(ui->tab3_im_comboBox->currentText());
  imageRawQt_.load(fname);

  // read the size
  size_t nrow = imageRawQt_.height();
  size_t ncol = imageRawQt_.width ();

  // allocate data
  imageRaw_ .reshape({nrow,ncol});
  image_    .reshape({nrow,ncol});
  mask_     .reshape({nrow,ncol});
  imageView_.reshape({nrow,ncol});

  // read image
  for ( size_t i=0 ; i<nrow ; i++ )
    for ( size_t j=0 ; j<ncol ; j++ )
      imageRaw_(i,j) = qGray(imageRawQt_.pixel(j,i));

  // set row/column selection wizard
  ui->tab3_rowHgh_spinBox->setValue  (nrow);
  ui->tab3_rowHgh_spinBox->setMaximum(nrow);
  ui->tab3_colHgh_spinBox->setValue  (ncol);
  ui->tab3_colHgh_spinBox->setMaximum(ncol);
}

void MainWindow::tab3_readPhase(void)
{
  int min = ui->tab3_phaseLow_spinBox->value();
  int max = ui->tab3_phaseHgh_spinBox->value();

  if ( dtype_[ui->tab3_set_comboBox->currentIndex()]=="float" ) {
    for ( size_t i=0 ; i<imageRaw_.size() ; i++ ) {
      if ( imageRaw_[i]>=min && imageRaw_[i]<=max ) {
        image_    [i] = imageRaw_[i];
        mask_     [i] = 0;
        imageView_[i] = std::min(imageRaw_[i],254);
      }
      else {
        mask_     [i] = 1;
        imageView_[i] = 255;
      }
    }
  }

  QSpinBox *maskLow[3];
  QSpinBox *maskHgh[3];

  maskLow[0] = ui->tab3_mask1Low_spinBox;
  maskLow[1] = ui->tab3_mask2Low_spinBox;
  maskLow[2] = ui->tab3_mask3Low_spinBox;
  maskHgh[0] = ui->tab3_mask1Hgh_spinBox;
  maskHgh[1] = ui->tab3_mask2Hgh_spinBox;
  maskHgh[2] = ui->tab3_mask3Hgh_spinBox;

  for ( int idx=0 ; idx<3 ; idx++ ) {

    min = maskLow[0]->value();
    max = maskHgh[0]->value();

    if ( min!=max )
      for ( size_t i=0 ; i<imageRaw_.size() ; i++ )
        if ( imageRaw_[i]>=min && imageRaw_[i]<=max )
          imageView_[i] = 255;
  }




  size_t nrow = imageRaw_.shape()[0];
  size_t ncol = imageRaw_.shape()[1];
  size_t irow = ui->tab3_rowLow_spinBox->value();
  size_t jrow = ui->tab3_rowHgh_spinBox->value();
  size_t icol = ui->tab3_colLow_spinBox->value();
  size_t jcol = ui->tab3_colHgh_spinBox->value();

  if ( irow>0 || jrow<nrow || icol>0 || jcol<ncol )
  for ( size_t i=0 ; i<nrow ; i++ )
    for ( size_t j=0 ; j<ncol ; j++ )
      if ( i<irow || i>jrow || j<icol || j>jcol )
        imageView_(i,j) = 254;



}

void MainWindow::tab3_viewImage(void)
{
  // // load the image
  // QString fname = QDir(outDir_).filePath(ui->tab3_im_comboBox->currentText());
  // QImage  image(fname);

  // // set row/column selection wizard
  // ui->tab3_rowHgh_spinBox->setValue  (image.height());
  // ui->tab3_rowHgh_spinBox->setMaximum(image.height());
  // ui->tab3_colHgh_spinBox->setValue  (image.width ());
  // ui->tab3_colHgh_spinBox->setMaximum(image.width ());

  // create a "scene" with containing the image
  QGraphicsPixmapItem *item  = new QGraphicsPixmapItem(QPixmap::fromImage(imageRawQt_));
  QGraphicsScene      *scene = new QGraphicsScene;
  scene->addItem(item);
  ui->tab3_image_graphicsView->setScene(scene);

  // show the image with the correct scaling applied
  double scale = this->tab3_scaleImage();
  ui->tab3_image_graphicsView->setTransform(QTransform::fromScale(scale,scale));
  ui->tab3_image_graphicsView->show();
}

// ============================================================================
// tab3: view the interpreted image, gray-scale + red mask
// ============================================================================

void MainWindow::tab3_viewPhase(void)
{
//   // load the image
//   QString fname = QDir(outDir_).filePath(ui->tab3_im_comboBox->currentText());
//   QImage  rawImage(fname);

//   // read cropped-image dimensions
//   int irow = ui->tab3_rowLow_spinBox->value();
//   int jrow = ui->tab3_rowHgh_spinBox->value();
//   int icol = ui->tab3_colLow_spinBox->value();
//   int jcol = ui->tab3_colHgh_spinBox->value();

//   // convert raw image -> Image::Matrix
//   Image::Matrix<int> im({(size_t)(jrow-irow),(size_t)(jcol-icol)});
//   for ( int i=irow ; i<jrow ; i++ )
//     for ( int j=icol ; j<jcol ; j++ )
//       im(i,j) = qGray(rawImage.pixel(j,i));

//   // convert "im" to "unsigned char" data-type, for QImage
//   std::vector<unsigned char> data;
//   data.reserve(im.size());
//   for ( auto i : im )
//     data.push_back(i);
// //  std::vector<unsigned char> data(im.size());
// //  // - apply conversion for each entry
// //  for ( int i=0 ; i<(int)im.size() ; i++ )
// //    data[i] = (unsigned char)(im[i]);

  // colorbar
  QVector<QRgb> grayscale;
  // - gray values
  for (int i = 0; i < 255; ++i)
    grayscale.append(qRgb(i, i, i));
  // - add red to the final value
  grayscale.append(qRgb(255,0,0));

  // convert Image::Matrix -> image to view
  QImage image(imageView_.ptr(),imageView_.shape()[1],imageView_.shape()[0],QImage::QImage::Format_Indexed8);
  image.setColorTable(grayscale);

  // create a "scene" with containing the image
  QGraphicsPixmapItem *item  = new QGraphicsPixmapItem(QPixmap::fromImage(image));
  QGraphicsScene      *scene = new QGraphicsScene;
  scene->addItem(item);
  ui->tab3_phase_graphicsView->setScene(scene);

  // show the image with the correct scaling applied
  double scale = this->tab3_scaleImage();
  ui->tab3_phase_graphicsView->setTransform(QTransform::fromScale(scale,scale));
  ui->tab3_phase_graphicsView->show();
}


void MainWindow::tab3_syncImage(void)
{
  this->tab3_readImage();
  this->tab3_viewImage();
}

void MainWindow::tab3_syncPhase(void)
{
  this->tab3_readPhase();
  this->tab3_viewPhase();
}

// ============================================================================
// tab3: act on changes
// ============================================================================



void MainWindow::on_tab3_zoom_slider_valueChanged(int value)
{
  this->tab3_viewImage();
  this->tab3_viewPhase();
}

void MainWindow::on_tab3_zoomOut_pushButton_clicked()
{
  ui->tab3_zoom_slider->setSliderPosition(ui->tab3_zoom_slider->sliderPosition()-1);
}

void MainWindow::on_tab3_zoomIn__pushButton_clicked()
{
  ui->tab3_zoom_slider->setSliderPosition(ui->tab3_zoom_slider->sliderPosition()+1);
}



void MainWindow::on_tab4_cp2out_checkBox_toggled(bool checked)
{
  if ( checked )
    return;

  QString mes;
  mes.append("It is advised to store the images (a.k.a. the raw-data) ");
  mes.append("in the output directory, to facilitate reproducibility.");

  QMessageBox::warning(\
    this,\
    tr("GooseEYE"),\
    mes,\
    QMessageBox::Ok,\
    QMessageBox::Ok\
  );
}

