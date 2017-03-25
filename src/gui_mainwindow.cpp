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
  ui->tabWidget->setCurrentIndex(0);

  data_.reserve(0);

  // tab0: enable buttons
  connect(ui->tab0_result_lineEdit   ,&QLineEdit::textChanged,[=](){ui->tab0_result_lineEdit     ->setEnabled(true);});
  connect(ui->tab0_result_lineEdit   ,&QLineEdit::textChanged,[=](){ui->tab0_result_pushButton   ->setEnabled(true);});
  connect(ui->tab0_pdfRaw_lineEdit   ,&QLineEdit::textChanged,[=](){ui->tab0_pdfRaw_lineEdit     ->setEnabled(true);});
  connect(ui->tab0_pdfRaw_lineEdit   ,&QLineEdit::textChanged,[=](){ui->tab0_pdfRaw_pushButton   ->setEnabled(true);});
  connect(ui->tab0_pdfInterp_lineEdit,&QLineEdit::textChanged,[=](){ui->tab0_pdfInterp_lineEdit  ->setEnabled(true);});
  connect(ui->tab0_pdfInterp_lineEdit,&QLineEdit::textChanged,[=](){ui->tab0_pdfInterp_pushButton->setEnabled(true);});

  // tab1: treeWidget/radioButton/checkBox -> interpret selection, set allowed values
  connect(ui->tab1_measure_treeWidget,SIGNAL(clicked(QModelIndex)),this,SLOT(tab1_selectStat()));
  connect(ui->tab1_im0_checkBox      ,SIGNAL(clicked(bool))       ,this,SLOT(tab1_selectStat()));
  connect(ui->tab1_im1_checkBox      ,SIGNAL(clicked(bool))       ,this,SLOT(tab1_selectStat()));
  connect(ui->tab1_im0b_radioButton  ,SIGNAL(clicked(bool))       ,this,SLOT(tab1_selectStat()));
  connect(ui->tab1_im0i_radioButton  ,SIGNAL(clicked(bool))       ,this,SLOT(tab1_selectStat()));
  connect(ui->tab1_im0f_radioButton  ,SIGNAL(clicked(bool))       ,this,SLOT(tab1_selectStat()));
  connect(ui->tab1_im1b_radioButton  ,SIGNAL(clicked(bool))       ,this,SLOT(tab1_selectStat()));
  connect(ui->tab1_im1i_radioButton  ,SIGNAL(clicked(bool))       ,this,SLOT(tab1_selectStat()));
  connect(ui->tab1_im1f_radioButton  ,SIGNAL(clicked(bool))       ,this,SLOT(tab1_selectStat()));

  // tab2: enable pushButtons / set labels
  connect(ui->tabWidget,&QTabWidget::currentChanged,[=](){ui->tab2_im0Phase_label   ->setText(QString::fromStdString(data_.phase(0)));});
  connect(ui->tabWidget,&QTabWidget::currentChanged,[=](){ui->tab2_im1Phase_label   ->setText(QString::fromStdString(data_.phase(1)));});
  connect(ui->tabWidget,&QTabWidget::currentChanged,[=](){ui->tab2_im0Dtype_label   ->setText(QString::fromStdString(data_.dtype(0)));});
  connect(ui->tabWidget,&QTabWidget::currentChanged,[=](){ui->tab2_im1Dtype_label   ->setText(QString::fromStdString(data_.dtype(1)));});
  connect(ui->tabWidget,&QTabWidget::currentChanged,[=](){ui->tab2_im0_listWidget   ->setEnabled(ui->tab1_im0_checkBox->isChecked());});
  connect(ui->tabWidget,&QTabWidget::currentChanged,[=](){ui->tab2_im0Add_pushButton->setEnabled(ui->tab1_im0_checkBox->isChecked());});
  connect(ui->tabWidget,&QTabWidget::currentChanged,[=](){ui->tab2_im0Rmv_pushButton->setEnabled(ui->tab1_im0_checkBox->isChecked());});
  connect(ui->tabWidget,&QTabWidget::currentChanged,[=](){ui->tab2_im1_listWidget   ->setEnabled(ui->tab1_im1_checkBox->isChecked());});
  connect(ui->tabWidget,&QTabWidget::currentChanged,[=](){ui->tab2_im1Add_pushButton->setEnabled(ui->tab1_im1_checkBox->isChecked());});
  connect(ui->tabWidget,&QTabWidget::currentChanged,[=](){ui->tab2_im1Rmv_pushButton->setEnabled(ui->tab1_im1_checkBox->isChecked());});
  connect(ui->tabWidget,&QTabWidget::currentChanged,[=](){ui->tab2_im1Up__pushButton->setEnabled(ui->tab1_im1_checkBox->isChecked());});
  connect(ui->tabWidget,&QTabWidget::currentChanged,[=](){ui->tab2_im1Dwn_pushButton->setEnabled(ui->tab1_im1_checkBox->isChecked());});
  connect(ui->tabWidget,&QTabWidget::currentChanged,[=](){ui->tab2_cp_pushButton    ->setEnabled(ui->tab1_im1_checkBox->isChecked());});

  // tab2: pushButtons -> add/remove files
  connect(ui->tab2_im0Add_pushButton,&QPushButton::clicked,[=](){this->filesAdd(ui->tab2_im0_listWidget,0);});
  connect(ui->tab2_im1Add_pushButton,&QPushButton::clicked,[=](){this->filesAdd(ui->tab2_im1_listWidget,1);});
  connect(ui->tab2_im0Rmv_pushButton,&QPushButton::clicked,[=](){this->filesRm (ui->tab2_im0_listWidget,0);});
  connect(ui->tab2_im1Rmv_pushButton,&QPushButton::clicked,[=](){this->filesRm (ui->tab2_im1_listWidget,1);});
  connect(ui->tab2_im1Up__pushButton,&QPushButton::clicked,[=](){this->filesUp (ui->tab2_im1_listWidget,1);});
  connect(ui->tab2_im1Dwn_pushButton,&QPushButton::clicked,[=](){this->filesDwn(ui->tab2_im1_listWidget,1);});
  connect(ui->tab2_cp_pushButton    ,&QPushButton::clicked,[=](){this->filesCp (ui->tab2_im0_listWidget,ui->tab2_im1_listWidget,0,1);});

  // tab3: link scroll position of graphicsViews
  QScrollBar *I1h = ui->tab3_image_graphicsView->horizontalScrollBar();
  QScrollBar *I2h = ui->tab3_phase_graphicsView->horizontalScrollBar();
  QScrollBar *I1v = ui->tab3_image_graphicsView->verticalScrollBar  ();
  QScrollBar *I2v = ui->tab3_phase_graphicsView->verticalScrollBar  ();
  connect(I1h,SIGNAL(valueChanged(int)),I2h,SLOT(setValue(int)));
  connect(I2h,SIGNAL(valueChanged(int)),I1h,SLOT(setValue(int)));
  connect(I1v,SIGNAL(valueChanged(int)),I2v,SLOT(setValue(int)));
  connect(I2v,SIGNAL(valueChanged(int)),I1v,SLOT(setValue(int)));

  // tab3: switch set/image
  QComboBox *im  = ui->tab3_im_comboBox;
  QComboBox *set = ui->tab3_set_comboBox;
  connect(ui->tab3_imPrev_pushButton ,&QPushButton::clicked,[=](){if (im ->currentIndex()>0             ) im ->setCurrentIndex(im ->currentIndex()-1);});
  connect(ui->tab3_imNext_pushButton ,&QPushButton::clicked,[=](){if (im ->currentIndex()<im ->count()-1) im ->setCurrentIndex(im ->currentIndex()+1);});
  connect(ui->tab3_setPrev_pushButton,&QPushButton::clicked,[=](){if (set->currentIndex()>0             ) set->setCurrentIndex(set->currentIndex()-1);});
  connect(ui->tab3_setNext_pushButton,&QPushButton::clicked,[=](){if (set->currentIndex()<set->count()-1) set->setCurrentIndex(set->currentIndex()+1);});

  // tab3: load / current image changed -> reinitialize both images
  connect(ui->tab3_load_pushButton   ,SIGNAL(clicked(bool)) ,this,SLOT(tab3_syncImage()));
  connect(ui->tab3_load_pushButton   ,SIGNAL(clicked(bool)) ,this,SLOT(tab3_syncPhase()));
  connect(ui->tab3_imPrev_pushButton ,SIGNAL(clicked(bool)) ,this,SLOT(tab3_syncImage()));
  connect(ui->tab3_imPrev_pushButton ,SIGNAL(clicked(bool)) ,this,SLOT(tab3_syncPhase()));
  connect(ui->tab3_imNext_pushButton ,SIGNAL(clicked(bool)) ,this,SLOT(tab3_syncImage()));
  connect(ui->tab3_imNext_pushButton ,SIGNAL(clicked(bool)) ,this,SLOT(tab3_syncPhase()));
  connect(ui->tab3_setPrev_pushButton,SIGNAL(clicked(bool)) ,this,SLOT(tab3_syncImage()));
  connect(ui->tab3_setPrev_pushButton,SIGNAL(clicked(bool)) ,this,SLOT(tab3_syncPhase()));
  connect(ui->tab3_setNext_pushButton,SIGNAL(clicked(bool)) ,this,SLOT(tab3_syncImage()));
  connect(ui->tab3_setNext_pushButton,SIGNAL(clicked(bool)) ,this,SLOT(tab3_syncPhase()));
  connect(ui->tab3_im_comboBox       ,SIGNAL(activated(int)),this,SLOT(tab3_syncImage()));
  connect(ui->tab3_im_comboBox       ,SIGNAL(activated(int)),this,SLOT(tab3_syncPhase()));
  connect(ui->tab3_set_comboBox      ,SIGNAL(activated(int)),this,SLOT(tab3_syncImage()));
  connect(ui->tab3_set_comboBox      ,SIGNAL(activated(int)),this,SLOT(tab3_syncPhase()));

  // tab3: image manipulated -> reinitialize interpreted image
  connect(ui->tab3_phaseLow_spinBox,SIGNAL(editingFinished())       ,this,SLOT(tab3_syncPhase()));
  connect(ui->tab3_phaseHgh_spinBox,SIGNAL(editingFinished())       ,this,SLOT(tab3_syncPhase()));
  connect(ui->tab3_mask1Low_spinBox,SIGNAL(editingFinished())       ,this,SLOT(tab3_syncPhase()));
  connect(ui->tab3_mask1Hgh_spinBox,SIGNAL(editingFinished())       ,this,SLOT(tab3_syncPhase()));
  connect(ui->tab3_mask2Low_spinBox,SIGNAL(editingFinished())       ,this,SLOT(tab3_syncPhase()));
  connect(ui->tab3_mask2Hgh_spinBox,SIGNAL(editingFinished())       ,this,SLOT(tab3_syncPhase()));
  connect(ui->tab3_mask3Low_spinBox,SIGNAL(editingFinished())       ,this,SLOT(tab3_syncPhase()));
  connect(ui->tab3_mask3Hgh_spinBox,SIGNAL(editingFinished())       ,this,SLOT(tab3_syncPhase()));
  connect(ui->tab3_rowLow_spinBox  ,SIGNAL(editingFinished())       ,this,SLOT(tab3_syncPhase()));
  connect(ui->tab3_rowHgh_spinBox  ,SIGNAL(editingFinished())       ,this,SLOT(tab3_syncPhase()));
  connect(ui->tab3_colLow_spinBox  ,SIGNAL(editingFinished())       ,this,SLOT(tab3_syncPhase()));
  connect(ui->tab3_colHgh_spinBox  ,SIGNAL(editingFinished())       ,this,SLOT(tab3_syncPhase()));
  connect(ui->tab3_cmap_comboBox   ,SIGNAL(currentIndexChanged(int)),this,SLOT(tab3_syncPhase()));

  // tab3: zoom
  QSlider *zoom = ui->tab3_zoom_slider;
  connect(ui->tab3_zoomOut_pushButton,&QPushButton::clicked,[=](){zoom->setValue(zoom->value()-1);});
  connect(ui->tab3_zoomIn__pushButton,&QPushButton::clicked,[=](){zoom->setValue(zoom->value()+1);});
  connect(ui->tab3_zoom_slider,SIGNAL(valueChanged(int)),this,SLOT(tab3_viewImage()));
  connect(ui->tab3_zoom_slider,SIGNAL(valueChanged(int)),this,SLOT(tab3_viewPhase()));

}

// ============================================================================
// destructor
// ============================================================================

MainWindow::~MainWindow()
{
  delete ui;
}

// ============================================================================
// check if a file exists
// ============================================================================

bool MainWindow::fileExists ( QString fname , QString dirname )
{
  if ( dirname!="" )
    fname = QDir(dirname).filePath(fname);

  QFileInfo check_file(fname);

  // check if file exists and if yes: Is it really a file and no directory?
  if (check_file.exists() && check_file.isFile())
    return true;
  else
    return false;
}

// ============================================================================
// prompt the user with a warning or a question
// ============================================================================

void MainWindow::promptWarning ( QString msg )
{
  QMessageBox::warning(\
    this,\
    tr("GooseEYE"),\
    msg,\
    QMessageBox::Ok,\
    QMessageBox::Ok\
  );
}

// -----------------------------------------------------------------------------

void MainWindow::promptWorkInProgress( QString msg )
{
  return this->promptWarning(tr("Work-in-progress:\n\n")+msg);
}

// -----------------------------------------------------------------------------

bool MainWindow::promptQuestion ( QString msg )
{
  QMessageBox::StandardButton reply;

  reply = QMessageBox::question(\
    this,\
    tr("GooseEYE"),\
    msg,\
    QMessageBox::Yes|QMessageBox::No
  );

  if (reply == QMessageBox::Yes)
    return true;

  return false;
}

// ============================================================================
// tab-select:
// - check selection
// - print message to status-bar
// - synchronize data
// ============================================================================

void MainWindow::on_tabWidget_tabBarClicked(int index)
{
  QCheckBox *check[2];
  check[0] = ui->tab1_im0_checkBox;
  check[1] = ui->tab1_im1_checkBox;

  // switching between tabs: check selection
  // ---------------------------------------

  // check linear increase
  if ( index>prevTab_+1 ) {
    this->promptWarning(tr("Please complete/review tab %1 first").arg(QString::number(prevTab_+1)));
    return;
  }

  // leaving tab0: check storage directory
  if ( prevTab_==0 && index>0 ) {
    if ( ui->tab0_outdir_lineEdit->text().size()==0 ) {
      this->promptWarning("Please select a working directory first");
      return;
    }
  }
  // leaving tab1: check selected statistic
  if ( prevTab_==1 && index>1 ) {
    if ( data_.stat().size()==0 ) {
      this->promptWarning("Please select a statistical measure first\n(left dialog)");
      return;
    }
    for ( size_t i=0 ; i<2 ; i++ ) {
      if (check[i]->isChecked() && data_.dtype(i).size()==0 ) {
        this->promptWarning(tr("Please select data-type for image %1").arg(QString::number(i+1)));
        return;
      }
    }
  }
  // leaving tab2: check number of selected files
  if ( prevTab_==2 && index>2 ) {
    if ( ui->tab2_im1_listWidget->isEnabled() )
      if ( ui->tab2_im0_listWidget->count()!=ui->tab2_im1_listWidget->count() ) {
        this->promptWarning("Number of files must be equal");
        return;
      }
    if ( ui->tab2_im0_listWidget->count()<=0 ) {
      this->promptWarning("Please select files before continuing");
      return;
    }
  }
  // leaving tab3:
  // TODO

  // leaving tab4: check to save
  if ( prevTab_==4 && index!=4 ) {
    if ( ui->tab4_save_pushButton->isEnabled() ) {
      this->promptWarning("The modifications in the results have not been saved");
    }
  }

  // tab3: selectively enable and fill QComboBox for set and images
  if ( index==3 && data_.stat()!="" ) {

    ui->tab3_set_comboBox->clear();
    ui->tab3_set_comboBox->setEnabled(true);

    QString name = "%1: %2 (%3)";
    for ( int i=0 ; i<2 ; i++ )
      if ( check[i]->isChecked() )
        ui->tab3_set_comboBox->addItem(name.arg(QString::number(i+1),QString::fromStdString(data_.phase(i)),QString::fromStdString(data_.dtype(i))));

    ui->tab3_setNext_pushButton->setEnabled(check[0]->isChecked() && check[1]->isChecked());
    ui->tab3_setPrev_pushButton->setEnabled(check[0]->isChecked() && check[1]->isChecked());

    this->on_tab3_set_comboBox_currentIndexChanged(0);
  }

  // print message to status-bar
  QString msg;
  if      ( index==0 ) msg = "Overview, load existing state, storage settings";
  else if ( index==1 ) msg = "Select measure and image type(s)";
  else if ( index==2 ) msg = "Select images";
  else if ( index==3 ) msg = "Check/modify images";
  else if ( index==4 ) msg = "Run computation";
  statusBar()->showMessage(msg);

  prevTab_ = index;
}

// ============================================================================
// tab0: load existing state
// ============================================================================

void MainWindow::on_tab0_load_pushButton_clicked()
{
  this->promptWorkInProgress("JSON file functionality will become available soon");
  return;

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
  QString dirPrev = ui->tab0_outdir_lineEdit->text();
  QString dirOpen = dirPrev;

  // set default directory to open the file-dialog
  if ( dirOpen.size()==0 )
    dirOpen = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);

  // alias widgets
  QLineEdit   *line[3];
  QListWidget *list[2];
  line[0] = ui->tab0_result_lineEdit;
  line[1] = ui->tab0_pdfRaw_lineEdit;
  line[2] = ui->tab0_pdfInterp_lineEdit;
  list[0] = ui->tab2_im0_listWidget;
  list[1] = ui->tab2_im1_listWidget;

  // open file-dialog
  QString dir = QFileDialog::getExistingDirectory(\
    this,\
    tr("Open Directory"),\
    dirOpen,\
    QFileDialog::ShowDirsOnly\
  );

  // no directory selected -> quit function
  if ( dir.size()==0 )
    return;

  // store directory
  ui->tab0_outdir_lineEdit->setText(dir);

  // working directory changes -> convert relative paths to absolute ones
  // then convert it to new relative paths
  if ( dirPrev.size()>0 ) {
    // - convert type
    QDir Dir(dir);
    // - filenames in QListEdit
    for ( size_t i=0 ; i<3 ; i++ )
      if ( line[i]->text().size()>0 )
        line[i]->setText(Dir.relativeFilePath(QDir(dirPrev).filePath(line[i]->text())));
    // - filenames in QListWidget
    for ( size_t i=0 ; i<2 ; i++ )
      for ( size_t j=0 ; j<list[i]->count() ; j++ )
        list[i]->item(j)->setText(Dir.relativeFilePath(QDir(dirPrev).filePath(list[i]->item(j)->text())));
  }

  // check to set default
  if ( ui->tab0_result_lineEdit->text()!="" && ui->tab0_pdfRaw_lineEdit->text()!="" && ui->tab0_pdfInterp_lineEdit->text()!="" )
    return;

  // default file-names
  QString result    = "GooseEYE.json";
  QString pdfRaw    = "GooseEYE_result.pdf";
  QString pdfInterp = "GooseEYE_result_interpretation.pdf";

  // if one of default files exists, find a number to append the files
  // (the same number if used for all files)
  if ( this->fileExists(result,dir) || this->fileExists(pdfRaw,dir) || this->fileExists(pdfInterp,dir)  ) {
    result    = "GooseEYE-%1.json";
    pdfRaw    = "GooseEYE-%1_result.pdf";
    pdfInterp = "GooseEYE-%1_result_interpretation.pdf";
    int i = 1;
    while ( this->fileExists(result.arg(QString::number(i)),dir) || this->fileExists(pdfRaw.arg(QString::number(i)),dir) || this->fileExists(pdfInterp.arg(QString::number(i)),dir)  )
      i++;
    result    = result   .arg(QString::number(i));
    pdfRaw    = pdfRaw   .arg(QString::number(i));
    pdfInterp = pdfInterp.arg(QString::number(i));
  }

  // store the default filenames
  if ( ui->tab0_result_lineEdit   ->text()=="" ) ui->tab0_result_lineEdit   ->setText(result);
  if ( ui->tab0_pdfRaw_lineEdit   ->text()=="" ) ui->tab0_pdfRaw_lineEdit   ->setText(pdfRaw);
  if ( ui->tab0_pdfInterp_lineEdit->text()=="" ) ui->tab0_pdfInterp_lineEdit->setText(pdfInterp);
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

  QDir dir(ui->tab0_outdir_lineEdit->text());
  ui->tab0_result_lineEdit->setText(dir.relativeFilePath(fname));
}

// -----------------------------------------------------------------------------

void MainWindow::on_tab0_pdfRaw_pushButton_clicked()
{
  QString fname = QFileDialog::getOpenFileName(\
    this,\
    tr("Open File"),\
    QStandardPaths::writableLocation(QStandardPaths::HomeLocation),\
    tr("State Files (*.pdf *.PDF)")\
  );

  if ( fname.size()==0 )
    return;

  QDir dir(ui->tab0_outdir_lineEdit->text());
  ui->tab0_pdfRaw_lineEdit->setText(dir.relativeFilePath(fname));
}

// -----------------------------------------------------------------------------

void MainWindow::on_tab0_pdfInterp_pushButton_clicked()
{
  QString fname = QFileDialog::getOpenFileName(\
    this,\
    tr("Open File"),\
    QStandardPaths::writableLocation(QStandardPaths::HomeLocation),\
    tr("State Files (*.pdf *.PDF)")\
  );

  if ( fname.size()==0 )
    return;

  QDir dir(ui->tab0_outdir_lineEdit->text());
  ui->tab0_pdfInterp_lineEdit->setText(dir.relativeFilePath(fname));
}

// ============================================================================
// tab1: selection of statistic -> controls possible images and data types
//       (function is back-end of several signals below)
// ============================================================================

void MainWindow::tab1_selectStat(void)
{
  if ( ui->tab1_measure_treeWidget->currentItem()->childCount()>0 ) {
    if ( !ui->tab1_measure_treeWidget->currentItem()->isExpanded() ) {
      ui->tab1_measure_treeWidget->collapseAll();
      ui->tab1_measure_treeWidget->currentItem()->setExpanded(true);
    }
    return;
  }

  // local variables
  QString category = ui->tab1_measure_treeWidget->currentItem()->parent()->text(0);
  QString measure  = ui->tab1_measure_treeWidget->currentItem()->text(0);
  bool    lnk      = false;
  int     im1      = 0;
  // lists to signal the allowed data-types
  int dtype[2][3];
  QStringList      label {"binary","int","float"};
  // allocate lists of pointers
  QButtonGroup *group[2];
  QCheckBox    *check[2];
  QRadioButton *radio[2][3];
  // pointers to dtype checkBoxes
  check[0]     = ui->tab1_im0_checkBox;
  check[1]     = ui->tab1_im1_checkBox;
  // pointers to dtype radioButtons
  radio[0][0]  = ui->tab1_im0b_radioButton;
  radio[0][1]  = ui->tab1_im0i_radioButton;
  radio[0][2]  = ui->tab1_im0f_radioButton;
  radio[1][0]  = ui->tab1_im1b_radioButton;
  radio[1][1]  = ui->tab1_im1i_radioButton;
  radio[1][2]  = ui->tab1_im1f_radioButton;
  // pointer to dtype buttonGroups
  group[0]     = ui->buttonGroup_tab1_im0;
  group[1]     = ui->buttonGroup_tab1_im1;

  // default: no information on function / phase / data-type
  data_.set_stat("");
  for ( int i=0 ; i<2 ; i++ ) {
    data_.set_phase(i,"");
    data_.set_dtype(i,"");
  }

  // settings based on selected statistic
  if      ( category=="Spatial correlations" ) {
    if      ( measure=="2-point probability" ) {
      data_.set_stat (  "S2"   );
      data_.set_phase(0,"Phase");
      data_.set_phase(1,"Phase");
      lnk         = true;
      dtype[0][0] = 1; dtype[0][1] = 0; dtype[0][2] = 1;
      dtype[1][0] = 1; dtype[1][1] = 0; dtype[1][2] = 1;
    }
    else if ( measure=="2-point cluster" ) {
      data_.set_stat (  "S2"      );
      data_.set_phase(0,"Clusters");
      data_.set_phase(1,"Clusters");
      lnk         = true;
      dtype[0][0] = 0; dtype[0][1] = 1; dtype[0][2] = 0;
      dtype[1][0] = 0; dtype[1][1] = 1; dtype[1][2] = 0;

    }
    else if ( measure=="Lineal path" ) {
      data_.set_stat (  "L"    );
      data_.set_phase(0,"Phase");
      lnk         = true;
      im1         = -1;
      dtype[0][0] = 1; dtype[0][1] = 1; dtype[0][2] = 0;
      dtype[1][0] = 1; dtype[1][1] = 1; dtype[1][2] = 0;
    }
  }
  else if ( category=="Conditional spatial correlations" ) {
    if      ( measure=="2-point probability" ) {
      data_.set_stat (  "W2"     );
      data_.set_phase(0,"Weights");
      data_.set_phase(1,"Phase"  );
      im1         = 1;
      dtype[0][0] = 1; dtype[0][1] = 0; dtype[0][2] = 1;
      dtype[1][0] = 1; dtype[1][1] = 0; dtype[1][2] = 1;
    }
    else if ( measure=="Collapsed 2-point probability" ) {
      data_.set_stat (  "W2c"    );
      data_.set_phase(0,"Weights");
      data_.set_phase(1,"Phase"  );
      im1         = 1;
      dtype[0][0] = 0; dtype[0][1] = 1; dtype[0][2] = 0;
      dtype[1][0] = 1; dtype[1][1] = 0; dtype[1][2] = 1;
    }
  }

  // function not implemented: show warning
  if ( data_.stat()=="" ) {
    this->promptWorkInProgress("Function under construction");
    return;
  }

  // show function in status bar
  QString mes = "Function to use: '%1'";
  statusBar()->showMessage(mes.arg(QString::fromStdString(data_.stat())));

  // apply phase label
  for ( int i=0 ; i<2 ; i++ )
    check[i]->setText(QString::fromStdString(data_.phase(i)));

  // selection of image 2: -1=disable ; 0=user-select ; 1=force-select
  if      ( im1==-1 ) {
    group[1]->setExclusive(false);
    for ( int j=0 ; j<3 ; j++ )
      radio[1][j]->setChecked(false);
    group[1]->setExclusive(true);
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
  // - set non-exclusive (otherwise setChecked(false) will have no effect)
  for ( int i=0 ; i<2 ; i++ )
    group[i]->setExclusive(false);
  // - optionally uncheck
  for ( int i=0 ; i<2 ; i++ )
    for ( int j=0 ; j<3 ; j++ )
      if ( !dtype[i][j] && radio[i][j]->isChecked() )
        radio[i][j]->setChecked(false);
  // - reset exclusive to get back the normal behavior
  for ( int i=0 ; i<2 ; i++ )
    group[i]->setExclusive(true);

  // auto-fill radioButton if there is only one possibility
  for ( int i=0 ; i<2 ; i++ ) {
    if ( dtype[i][0]+dtype[i][1]+dtype[i][2]==1 )
      for ( int j=0 ; j<3 ; j++ )
        if ( dtype[i][j] && check[i]->isChecked() )
          radio[i][j]->setChecked(true);
  }

  // enable/disable dtype radioButton based on allowed dtype(s)
  for ( int i=0 ; i<2 ; i++ )
    for ( int j=0 ; j<3 ; j++ )
      radio[i][j]->setEnabled((bool)dtype[i][j]);

  // optionally link image 2 to image 1
  if ( lnk ) {
    // - clear radioButtons for image 2
    group[1]->setExclusive(false);
    for ( int j=0 ; j<3 ; j++ )
      radio[1][j]->setChecked(false);
    group[1]->setExclusive(true);
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
        data_.set_dtype(i,label[j].toStdString());

  // image interpretation settings
  ui->tab1_periodic_checkBox ->setEnabled(true );
  ui->tab1_zeropad_checkBox  ->setEnabled(true );
  ui->tab1_maskW_checkBox    ->setEnabled(false);
  ui->tab1_maskW_checkBox    ->setChecked(false);
  ui->tab1_pixelpath_label   ->setEnabled(false);
  ui->tab1_pixelpath_comboBox->setEnabled(false);

  if ( data_.stat()=="W2" || data_.stat()=="W2c" ) {
    ui->tab1_maskW_checkBox->setEnabled(true);
    ui->tab1_maskW_checkBox->setChecked(true);
  }

  if ( data_.stat()=="L" || data_.stat()=="W2c" ) {
    ui->tab1_zeropad_checkBox  ->setEnabled(false);
    ui->tab1_pixelpath_label   ->setEnabled(true );
    ui->tab1_pixelpath_comboBox->setEnabled(true );
  }

  if ( !ui->tab1_periodic_checkBox->isChecked() && !ui->tab1_zeropad_checkBox->isChecked() )
    ui->tab1_periodic_checkBox->setChecked(true);

}

// =============================================================================
// tab2: add/remove/move files to/in QListWidgets
// =============================================================================

void MainWindow::filesAdd(QListWidget *list, size_t set)
{
  QStringList filters;
  filters << "Image files (*.png *.xpm *.jpg *.jpeg *.tif *.tiff *.bmp)"
          << "Any files (*)";

  QFileDialog dialog(this);
  dialog.setFileMode   (QFileDialog::ExistingFiles);
  dialog.setOption     (QFileDialog::HideNameFilterDetails,false);
  dialog.setDirectory  (ui->tab0_outdir_lineEdit->text());
  dialog.setNameFilters(filters);
  dialog.setViewMode   (QFileDialog::List);

  QStringList fileNames;
  if (dialog.exec())
    fileNames = dialog.selectedFiles();

  QDir dir(ui->tab0_outdir_lineEdit->text());
  for ( int i=0 ; i<fileNames.size() ; i++ )
    fileNames[i] = dir.relativeFilePath(fileNames[i]);

  list->addItems(fileNames);

   for ( size_t i=0 ; i<fileNames.size() ; i++ )
     data_.itemAdd(set,fileNames[i].toStdString());
}

// -----------------------------------------------------------------------------

void MainWindow::filesRm(QListWidget *list, size_t set)
{
  QList<QListWidgetItem*> items = list->selectedItems();

  foreach (QListWidgetItem *item, items) {
    size_t row = list->row(item);
    delete list->takeItem(list->row(item));
    data_.itemRm(set,row);
  }
}

// -----------------------------------------------------------------------------

void MainWindow::filesUp(QListWidget *list, size_t set)
{
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

  for ( int i=0 ; i<items.size() ; i++ )
    data_.itemUp(set,row[i]);
}

// -----------------------------------------------------------------------------

void MainWindow::filesDwn(QListWidget *list, size_t set)
{
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

  for ( int i=0 ; i<items.size() ; i++ )
    data_.itemDown(set,row[i]);
}

// -----------------------------------------------------------------------------

void MainWindow::filesCp(QListWidget *src,QListWidget *dest, size_t src_set, size_t dest_set)
{
  if ( dest->count()>0 ) {
    QMessageBox::warning(\
      this,\
      tr("GooseEYE"),\
      tr("Only allowed if destination is empty."),\
      QMessageBox::Ok,\
      QMessageBox::Ok\
    );
    return;
  }

  for ( int i=0 ; i<src->count() ; i++ )
    dest->addItem(src->item(i)->text());
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
}

// =============================================================================
// tab3: read raw image (view using "tab3_viewImage")
// =============================================================================

void MainWindow::tab3_readImage(void)
{
  // load the image
  QString fname = QDir(ui->tab0_outdir_lineEdit->text()).filePath(ui->tab3_im_comboBox->currentText());
  imgRawQt_.load(fname);

  // read the size
  size_t nrow = imgRawQt_.height();
  size_t ncol = imgRawQt_.width ();

  // allocate data
  imgRaw_ .reshape({nrow,ncol});
  imgView_.reshape({nrow,ncol});

  // read image
  for ( size_t i=0 ; i<nrow ; i++ )
    for ( size_t j=0 ; j<ncol ; j++ )
      imgRaw_(i,j) = qGray(imgRawQt_.pixel(j,i));

  // set row/column selection wizard
  ui->tab3_rowHgh_spinBox->setValue  (nrow);
  ui->tab3_rowHgh_spinBox->setMaximum(nrow);
  ui->tab3_colHgh_spinBox->setValue  (ncol);
  ui->tab3_colHgh_spinBox->setMaximum(ncol);
}

// =============================================================================
// tab3: interpret image (view using "tab3_viewPhase")
// =============================================================================

void MainWindow::tab3_readPhase(void)
{
  if ( imgRawQt_.isNull() )
    return;

  int min,max;
  std::string dtype = data_.dtype(ui->tab3_set_comboBox->currentIndex());

  // apply segmentation
  // ------------------

  min = ui->tab3_phaseLow_spinBox->value();
  max = ui->tab3_phaseHgh_spinBox->value();

  if ( dtype=="float" ) {
    // threshold image, set excluded pixels masked
    for ( size_t i=0 ; i<imgRaw_.size() ; i++ ) {
      if ( imgRaw_[i]>=min && imgRaw_[i]<=max )
        imgView_[i] = std::min(imgRaw_[i],254);
      else
        imgView_[i] = 255;
    }
  }
  else if ( dtype=="binary" ) {
    // threshold image, within range -> white, otherwise -> black
    for ( size_t i=0 ; i<imgRaw_.size() ; i++ ) {
      if ( imgRaw_[i]>=min && imgRaw_[i]<=max )
        imgView_[i] = 254;
      else
        imgView_[i] = 0;
    }
  }
  else if ( dtype=="int" ) {
    // threshold image, within range -> 1, otherwise -> 0
    Image::Matrix<int> im(imgRaw_.shape());
    for ( size_t i=0 ; i<imgRaw_.size() ; i++ ) {
      if ( imgRaw_[i]>=min && imgRaw_[i]<=max )
        im[i] = 1;
      else
        im[i] = 0;
    }
    // compute clusters
    Image::Matrix<int> clusters,centers;
    std::tie(clusters,centers) = Image::clusters(im);
    // scale image for optimizal color contrast
    double fac = 255./(double)clusters.max();
    for ( size_t i=0 ; i<imgRaw_.size() ; i++ )
      imgView_[i] = std::min((int)((double)clusters[i]*fac),254);
  }

  // apply mask
  // ----------

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
      for ( size_t i=0 ; i<imgRaw_.size() ; i++ )
        if ( imgRaw_[i]>=min && imgRaw_[i]<=max )
          imgView_[i] = 255;
  }

  // mask weights
  // ------------

  if ( ui->tab1_maskW_checkBox->isChecked() && ui->tab3_set_comboBox->currentIndex()>0 ) {
    if ( data_.saved(0,ui->tab3_im_comboBox->currentIndex()) ) {
      Image::Matrix<int> W,M;
      std::tie(W,M) = data_.image(0,ui->tab3_im_comboBox->currentIndex(),imgRaw_,false);
      for ( size_t i=0 ; i<imgRaw_.size() ; i++ )
        if ( W[i] )
          imgView_[i] = 255;
    }
  }

  // crop the image
  // --------------

  size_t nrow = imgRaw_.shape()[0];
  size_t ncol = imgRaw_.shape()[1];
  size_t irow = ui->tab3_rowLow_spinBox->value();
  size_t jrow = ui->tab3_rowHgh_spinBox->value();
  size_t icol = ui->tab3_colLow_spinBox->value();
  size_t jcol = ui->tab3_colHgh_spinBox->value();

  if ( irow>0 || jrow<nrow || icol>0 || jcol<ncol )
  for ( size_t i=0 ; i<nrow ; i++ )
    for ( size_t j=0 ; j<ncol ; j++ )
      if ( i<irow || i>jrow || j<icol || j>jcol )
        imgView_(i,j) = 254;
}

// =============================================================================
// tab3: compute image scaling
// =============================================================================

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

// =============================================================================
// tab3: view raw image
// =============================================================================

void MainWindow::tab3_viewImage(void)
{
  // create a "scene" with containing the image
  QGraphicsPixmapItem *item  = new QGraphicsPixmapItem(QPixmap::fromImage(imgRawQt_));
  QGraphicsScene      *scene = new QGraphicsScene;
  scene->addItem(item);
  ui->tab3_image_graphicsView->setScene(scene);

  // show the image with the correct scaling applied
  double scale = this->tab3_scaleImage();
  ui->tab3_image_graphicsView->setTransform(QTransform::fromScale(scale,scale));
  ui->tab3_image_graphicsView->show();
}

// =============================================================================
// tab3: view the interpreted image, gray-scale + red mask
// =============================================================================

void MainWindow::tab3_viewPhase(void)
{
  if ( imgRawQt_.isNull() )
    return;

  // define colormap based on data-type
  std::vector<int> cols;
  if (  data_.dtype(ui->tab3_set_comboBox->currentIndex())=="int" ) {
    cols = cppcolormap::RdOrYl_r(256);
    cols[  0*3+0] = 255; cols[  0*3+1] = 255; cols[  0*3+2] = 255; // white background
    cols[254*3+0] = 255; cols[254*3+1] = 255; cols[254*3+2] = 255; // white excluded pixels
    cols[255*3+0] =   0; cols[255*3+1] =   0; cols[255*3+2] =   0; // black mask
  }
  else {
    cols = cppcolormap::colormap(ui->tab3_cmap_comboBox->currentText().toStdString(),256);
//    cols = cppcolormap::Greys(256);
    cols[255*3+0] = 255; cols[255*3+1] =   0; cols[255*3+2] =   0; // red mask
  }
  // convert to Qt format
  QVector<QRgb> cmap;
  for ( size_t i = 0; i < 256; ++i)
    cmap.append(qRgb(cols[i*3+0],cols[i*3+1],cols[i*3+2]));

  // convert Image::Matrix -> image to view
  QImage image(\
    imgView_.ptr(),\
    imgView_.shape()[1],\
    imgView_.shape()[0],\
    QImage::QImage::Format_Indexed8\
  );
  image.setColorTable(cmap);

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

// =============================================================================
// tab3: fully refresh images
// =============================================================================

void MainWindow::tab3_syncImage(void)
{
  this->tab3_readImage();
  this->tab3_viewImage();
}

// -----------------------------------------------------------------------------

void MainWindow::tab3_syncPhase(void)
{
  this->tab3_readPhase();
  this->tab3_viewPhase();
}


void MainWindow::tab4_plotResult(void)
{
  // configure axis rect:
ui->raw_customPlot->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom); // this will also allow rescaling the color scale by dragging/zooming
ui->raw_customPlot->axisRect()->setupFullAxesBox(true);
ui->raw_customPlot->xAxis->setLabel("x");
ui->raw_customPlot->yAxis->setLabel("y");

// set up the QCPColorMap:
QCPColorMap *colorMap = new QCPColorMap(ui->raw_customPlot->xAxis, ui->raw_customPlot->yAxis);
int nx = 200;
int ny = 200;
colorMap->data()->setSize(nx, ny); // we want the color map to have nx * ny data points
colorMap->data()->setRange(QCPRange(-4, 4), QCPRange(-4, 4)); // and span the coordinate range -4..4 in both key (x) and value (y) dimensions
// now we assign some data, by accessing the QCPColorMapData instance of the color map:
double x, y, z;
for (int xIndex=0; xIndex<nx; ++xIndex)
{
  for (int yIndex=0; yIndex<ny; ++yIndex)
  {
    colorMap->data()->cellToCoord(xIndex, yIndex, &x, &y);
    double r = 3*qSqrt(x*x+y*y)+1e-2;
    z = 2*x*(qCos(r+2)/r-qSin(r+2)/r); // the B field strength of dipole radiation (modulo physical constants)
    colorMap->data()->setCell(xIndex, yIndex, z);
  }
}

// add a color scale:
QCPColorScale *colorScale = new QCPColorScale(ui->raw_customPlot);
ui->raw_customPlot->plotLayout()->addElement(0, 1, colorScale); // add it to the right of the main axis rect
colorScale->setType(QCPAxis::atRight); // scale shall be vertical bar with tick/axis labels right (actually atRight is already the default)
colorMap->setColorScale(colorScale); // associate the color map with the color scale
colorScale->axis()->setLabel("Magnetic Field Strength");

// set the color gradient of the color map to one of the presets:
colorMap->setGradient(QCPColorGradient::gpPolar);
// we could have also created a QCPColorGradient instance and added own colors to
// the gradient, see the documentation of QCPColorGradient for what's possible.

// rescale the data dimension (color) such that all data points lie in the span visualized by the color gradient:
colorMap->rescaleDataRange();

// make sure the axis rect and color scale synchronize their bottom and top margins (so they line up):
QCPMarginGroup *marginGroup = new QCPMarginGroup(ui->raw_customPlot);
ui->raw_customPlot->axisRect()->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);
colorScale->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);

// rescale the key (x) and value (y) axes so the whole color map is visible:
ui->raw_customPlot->rescaleAxes();
}




