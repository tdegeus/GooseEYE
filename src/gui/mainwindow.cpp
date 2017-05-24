#include "mainwindow.h"
#include "ui_mainwindow.h"

// =================================================================================================

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  // initialize GUI
  ui->setupUi(this);

  // always start on first tab
  ui->tabWidget->setCurrentIndex(0);

  // minimum number of sets == 1
  data.sets.resize(1);
  // number of plots -> always 2
  data.plot.resize(2);

  // T3: link scroll position of graphicsViews
  QScrollBar *I1h = ui->graphicsViewT3_img->horizontalScrollBar();
  QScrollBar *I2h = ui->graphicsViewT3_seg->horizontalScrollBar();
  QScrollBar *I1v = ui->graphicsViewT3_img->verticalScrollBar  ();
  QScrollBar *I2v = ui->graphicsViewT3_seg->verticalScrollBar  ();
  connect(I1h,SIGNAL(valueChanged(int)),I2h,SLOT(setValue(int)));
  connect(I2h,SIGNAL(valueChanged(int)),I1h,SLOT(setValue(int)));
  connect(I1v,SIGNAL(valueChanged(int)),I2v,SLOT(setValue(int)));
  connect(I2v,SIGNAL(valueChanged(int)),I1v,SLOT(setValue(int)));

  // T3: connect zoom buttons to slider (the latter is used to actually zoom)
  QSlider *zoom = ui->sliderT3_zoom;
  connect(ui->pushButtonT3_zoomIn ,&QPushButton::clicked,[=](){zoom->setValue(zoom->value()-1);});
  connect(ui->pushButtonT3_zoomOut,&QPushButton::clicked,[=](){zoom->setValue(zoom->value()+1);});

  // T3: switch set/image
  // - alias variables
  QComboBox *img = ui->comboBoxT3_img;
  QComboBox *set = ui->comboBoxT3_set;
  // - previous image
  connect(ui->pushButtonT3_imgPrev ,&QPushButton::clicked,[=](){
    if (img->currentIndex()>0             ) img->setCurrentIndex(img->currentIndex()-1);});
  // - next image
  connect(ui->pushButtonT3_imgNext ,&QPushButton::clicked,[=](){
    if (img->currentIndex()<img->count()-1) img->setCurrentIndex(img->currentIndex()+1);});
  // - previous set
  connect(ui->pushButtonT3_setPrev,&QPushButton::clicked,[=](){
    if (set->currentIndex()>0             ) set->setCurrentIndex(set->currentIndex()-1);});
  // - next set
  connect(ui->pushButtonT3_setNext,&QPushButton::clicked,[=](){
    if (set->currentIndex()<set->count()-1) set->setCurrentIndex(set->currentIndex()+1);});

  // combine buttons to list, for fast references
  // - T1: radioButton-group to select statistic, and their field-names
  statBtn.push_back(ui->radioButtonT1_S2     ); statKey.push_back("S2"    );
  statBtn.push_back(ui->radioButtonT1_C2     ); statKey.push_back("C2"    );
  statBtn.push_back(ui->radioButtonT1_L      ); statKey.push_back("L"     );
  statBtn.push_back(ui->radioButtonT1_W2     ); statKey.push_back("W2"    );
  statBtn.push_back(ui->radioButtonT1_W2c    ); statKey.push_back("W2c"   );
  // - T1: specific settings controlling the operation of the computation
  statCheck.push_back(ui->checkBoxT1_periodic  );
  statCheck.push_back(ui->checkBoxT1_zeropad   );
  statCheck.push_back(ui->checkBoxT1_maskWeight);
  // - T1: specific settings controlling the operation of the computation
  statCombo.push_back(ui->checkBoxT1_pixelPath );
  // - T1: select number of sets
  nsetBtn.push_back(ui->checkBoxT1_set1      );
  nsetBtn.push_back(ui->checkBoxT1_set0      );
  // - T1: radioButton-groups to select data-types (one for each set), and their field-names
  typeBtn.push_back(ui->radioButtonT1_img0b  ); typeKey.push_back("binary");
  typeBtn.push_back(ui->radioButtonT1_img0i  ); typeKey.push_back("int"   );
  typeBtn.push_back(ui->radioButtonT1_img0f  ); typeKey.push_back("float" );
  typeBtn.push_back(ui->radioButtonT1_img1b  );
  typeBtn.push_back(ui->radioButtonT1_img1i  );
  typeBtn.push_back(ui->radioButtonT1_img1f  );
  // - T2: all file-manipulation buttons, combined and categorized
  fileBtn.push_back(ui->pushButtonT2_set0Add ); fileBtnAdd.push_back(ui->pushButtonT2_set0Add);
  fileBtn.push_back(ui->pushButtonT2_set1Add ); fileBtnAdd.push_back(ui->pushButtonT2_set1Add);
  fileBtn.push_back(ui->pushButtonT2_set0Rmv ); fileBtnRmv.push_back(ui->pushButtonT2_set0Rmv);
  fileBtn.push_back(ui->pushButtonT2_set1Rmv ); fileBtnRmv.push_back(ui->pushButtonT2_set1Rmv);
  fileBtn.push_back(ui->pushButtonT2_set0Up  ); fileBtnUp .push_back(ui->pushButtonT2_set0Up );
  fileBtn.push_back(ui->pushButtonT2_set1Up  ); fileBtnUp .push_back(ui->pushButtonT2_set1Up );
  fileBtn.push_back(ui->pushButtonT2_set0Dwn ); fileBtnDwn.push_back(ui->pushButtonT2_set0Dwn);
  fileBtn.push_back(ui->pushButtonT2_set1Dwn ); fileBtnDwn.push_back(ui->pushButtonT2_set1Dwn);
  fileBtn.push_back(ui->pushButtonT2_set0Srt ); fileBtnSrt.push_back(ui->pushButtonT2_set0Srt);
  fileBtn.push_back(ui->pushButtonT2_set1Srt ); fileBtnSrt.push_back(ui->pushButtonT2_set1Srt);
  fileBtn.push_back(ui->pushButtonT2_cp      );
  // - T2: all file-lists
  fileLst.push_back(ui->listWidgetT2_img0    );
  fileLst.push_back(ui->listWidgetT2_img1    );
  // - T2: all field labels ("phase","weight")
  propLbl.push_back(ui->tab2_im0Phase_label  );
  propLbl.push_back(ui->tab2_im1Phase_label  );
  // - T2: all data-type labels ("binary","int","float")
  typeLbl.push_back(ui->tab2_im0Dtype_label  );
  typeLbl.push_back(ui->tab2_im1Dtype_label  );
  // - T3: image threshold fields, categorized
  imSpnPhase.push_back(ui->spinBoxT3_phaseLow); imSpnPhase.push_back(ui->spinBoxT3_phaseHgh);
  imSpnMask .push_back(ui->spinBoxT3_mask0Low); imSpnMask .push_back(ui->spinBoxT3_mask0Hgh);
  imSpnMask .push_back(ui->spinBoxT3_mask1Low); imSpnMask .push_back(ui->spinBoxT3_mask1Hgh);
  imSpnMask .push_back(ui->spinBoxT3_mask2Low); imSpnMask .push_back(ui->spinBoxT3_mask2Hgh);
  imSpnRow  .push_back(ui->spinBoxT3_rowLow  ); imSpnRow  .push_back(ui->spinBoxT3_rowHgh  );
  imSpnCol  .push_back(ui->spinBoxT3_colLow  ); imSpnCol  .push_back(ui->spinBoxT3_colHgh  );
  // - T3: all image threshold fields
  for ( auto &i : imSpnPhase ) imSpn.push_back(i);
  for ( auto &i : imSpnMask  ) imSpn.push_back(i);
  for ( auto &i : imSpnRow   ) imSpn.push_back(i);
  for ( auto &i : imSpnCol   ) imSpn.push_back(i);
  // - T3: all image threshold selections
  imCheck.push_back(ui->checkBoxT3_phase    );
  imCheck.push_back(ui->checkBoxT3_mask     );
  imCheck.push_back(ui->checkBoxT3_row      );
  imCheck.push_back(ui->checkBoxT3_col      );
  // - T3: all image navigation buttons
  imBtn.push_back(ui->pushButtonT3_setPrev  ); imBtn.push_back(ui->pushButtonT3_setNext);
  imBtn.push_back(ui->pushButtonT3_imgPrev  ); imBtn.push_back(ui->pushButtonT3_imgNext);
  imBtn.push_back(ui->pushButtonT3_clear    );
  imBtn.push_back(ui->pushButtonT3_apply    );
  imBtn.push_back(ui->pushButtonT3_zoomIn   );
  imBtn.push_back(ui->pushButtonT3_zoomOut  );
  // - T3: all comboBoxes on this tab
  imCombo.push_back(ui->comboBoxT3_set      );
  imCombo.push_back(ui->comboBoxT3_img      );
  imCombo.push_back(ui->comboBoxT3_cmap     );
  imCombo.push_back(ui->comboBoxT3_cmapMask );
  // - T3: radioButton-group to select threshold using mouse-click
  imRadio.push_back(ui->radioButtonT3_phase );
  imRadio.push_back(ui->radioButtonT3_mask  );
  imRadio.push_back(ui->radioButtonT3_row   );
  imRadio.push_back(ui->radioButtonT3_col   );
  // - T4: ROI shape
  roiSpin.push_back(ui->spinBoxT4_roiRow    );
  roiSpin.push_back(ui->spinBoxT4_roiCol    );
  // - T4: all colorbar ranges
  resSpin.push_back(ui->doubleSpinBoxT4_climLowRes1);
  resSpin.push_back(ui->doubleSpinBoxT4_climHghRes1);
  resSpin.push_back(ui->doubleSpinBoxT4_climLowRes2);
  resSpin.push_back(ui->doubleSpinBoxT4_climHghRes2);
  // - T4: all colorbars
  resCombo.push_back(ui->comboBoxT4_cmapRes1);
  resCombo.push_back(ui->comboBoxT4_cmapRes2);
  // - T4: all plots
  resPlot .push_back(ui->customPlot_res0);
  resPlot .push_back(ui->customPlot_res1);
  // - all radioButtons-groups (on all tabs)
  btnGroup.push_back(ui->buttonGroupT1_stat );
  btnGroup.push_back(ui->buttonGroupT1_set0 );
  btnGroup.push_back(ui->buttonGroupT1_set1 );
  btnGroup.push_back(ui->buttonGroupT3_mouse);

  // TODO: implement mouse selection
  for ( auto &i : imRadio ) i->setVisible(false);

  // T1/T3: button pressed -> update "data"
  for ( auto &i : statCombo ) connect(i,SIGNAL(activated(int))   ,this,SLOT(tab1_read()));
  for ( auto &i : statBtn   ) connect(i,SIGNAL(clicked(bool))    ,this,SLOT(tab1_read()));
  for ( auto &i : statCheck ) connect(i,SIGNAL(clicked(bool))    ,this,SLOT(tab1_read()));
  for ( auto &i : typeBtn   ) connect(i,SIGNAL(clicked(bool))    ,this,SLOT(tab1_read()));
  for ( auto &i : nsetBtn   ) connect(i,SIGNAL(clicked(bool))    ,this,SLOT(tab1_read()));
  for ( auto &i : imCheck   ) connect(i,SIGNAL(clicked(bool))    ,this,SLOT(tab3_read()));
  for ( auto &i : imSpn     ) connect(i,SIGNAL(editingFinished()),this,SLOT(tab3_read()));
  for ( auto &i : roiSpin   ) connect(i,SIGNAL(editingFinished()),this,SLOT(tab4_read()));

  // T2: file manipulation button pressed -> update "data"
  for ( size_t i=0; i<2; ++i ) connect(fileBtnAdd[i],&QPushButton::clicked,this,[=](){fileAdd(i);});
  for ( size_t i=0; i<2; ++i ) connect(fileBtnRmv[i],&QPushButton::clicked,this,[=](){fileRmv(i);});
  for ( size_t i=0; i<2; ++i ) connect(fileBtnUp [i],&QPushButton::clicked,this,[=](){fileUp (i);});
  for ( size_t i=0; i<2; ++i ) connect(fileBtnDwn[i],&QPushButton::clicked,this,[=](){fileDwn(i);});
  for ( size_t i=0; i<2; ++i ) connect(fileBtnSrt[i],&QPushButton::clicked,this,[=](){fileSrt(i);});

  // refresh file related views when JSON is loaded or "data.path" is changed
  connect(ui->pushButtonT0_path,SIGNAL(clicked(bool)),this,SLOT(tab0_show()));
  connect(ui->pushButtonT0_path,SIGNAL(clicked(bool)),this,SLOT(tab2_show()));
  connect(ui->pushButtonT0_load,SIGNAL(clicked(bool)),this,SLOT(tab0_show()));
  connect(ui->pushButtonT0_load,SIGNAL(clicked(bool)),this,SLOT(tab1_show()));
  connect(ui->pushButtonT0_load,SIGNAL(clicked(bool)),this,SLOT(tab2_show()));

  // T3: mark to apply image defaults / apply settings to all images
  connect(ui->pushButtonT3_clear,SIGNAL(clicked(bool)),this,SLOT(tab3_default ()));
  connect(ui->pushButtonT3_apply,SIGNAL(clicked(bool)),this,SLOT(tab3_applyAll()));

  // T3: zoom changed -> re-render images
  connect(ui->sliderT3_zoom,SIGNAL(valueChanged(int)),this,SLOT(tab3_imag()));

  // T4: save all (JSON auto-saved when "compute" is pressed, and when it is completed)
  connect(ui->actionSave       ,SIGNAL(triggered(bool)),this,SLOT(tab4_save()));
  connect(ui->pushButtonT4_save,SIGNAL(clicked(bool))  ,this,SLOT(tab4_save()));

  // open help
  connect(ui->actionOnline_documentation,SIGNAL(triggered(bool)),this,SLOT(openHelp()));

  // T2/T3: button pressed -> refresh view with new "data"
  for ( auto &i : statCombo  ) connect(i,SIGNAL(activated(int))   ,this,SLOT(tab1_show()));
  for ( auto &i : statBtn    ) connect(i,SIGNAL(clicked(bool))    ,this,SLOT(tab1_show()));
  for ( auto &i : statCheck  ) connect(i,SIGNAL(clicked(bool))    ,this,SLOT(tab1_show()));
  for ( auto &i : typeBtn    ) connect(i,SIGNAL(clicked(bool))    ,this,SLOT(tab1_show()));
  for ( auto &i : nsetBtn    ) connect(i,SIGNAL(clicked(bool))    ,this,SLOT(tab1_show()));
  for ( auto &i : fileBtn    ) connect(i,SIGNAL(clicked(bool))    ,this,SLOT(tab2_show()));
  for ( auto &i : imBtn      ) connect(i,SIGNAL(clicked(bool))    ,this,SLOT(tab3_show()));
  for ( auto &i : imBtn      ) connect(i,SIGNAL(clicked(bool))    ,this,SLOT(tab3_imag()));
  for ( auto &i : imCheck    ) connect(i,SIGNAL(clicked(bool))    ,this,SLOT(tab3_show()));
  for ( auto &i : imCheck    ) connect(i,SIGNAL(clicked(bool))    ,this,SLOT(tab3_imag()));
  for ( auto &i : imCombo    ) connect(i,SIGNAL(activated(int))   ,this,SLOT(tab3_show()));
  for ( auto &i : imCombo    ) connect(i,SIGNAL(activated(int))   ,this,SLOT(tab3_imag()));
  for ( auto &i : resCombo   ) connect(i,SIGNAL(activated(int))   ,this,SLOT(tab4_plot()));
  for ( auto &i : imSpn      ) connect(i,SIGNAL(editingFinished()),this,SLOT(tab3_show()));
  for ( auto &i : imSpn      ) connect(i,SIGNAL(editingFinished()),this,SLOT(tab3_imag()));
  for ( auto &i : roiSpin    ) connect(i,SIGNAL(editingFinished()),this,SLOT(tab4_show()));
  for ( auto &i : resSpin    ) connect(i,SIGNAL(editingFinished()),this,SLOT(tab4_plot()));

  // compute the statistics
  connect(ui->pushButtonT4_compute,SIGNAL(clicked(bool)),this,SLOT(compute  ()));
  connect(ui->pushButtonT4_compute,SIGNAL(clicked(bool)),this,SLOT(tab4_plot()));

  // refresh view with new "data" when tab is changed
  connect(ui->tabWidget,&QTabWidget::currentChanged,[=](){tab0_show();});
  connect(ui->tabWidget,&QTabWidget::currentChanged,[=](){tab1_show();});
  connect(ui->tabWidget,&QTabWidget::currentChanged,[=](){tab2_show();});
  connect(ui->tabWidget,&QTabWidget::currentChanged,[=](){tab3_show();});
  connect(ui->tabWidget,&QTabWidget::currentChanged,[=](){tab3_imag();});
  connect(ui->tabWidget,&QTabWidget::currentChanged,[=](){tab4_show();});
  connect(ui->tabWidget,&QTabWidget::currentChanged,[=](){tab4_plot();});
}

// =================================================================================================

MainWindow::~MainWindow()
{
  delete ui;
}

// =================================================================================================

void MainWindow::promptWarning(QString msg)
{
  QMessageBox::warning(this,tr("GooseEYE"),msg,QMessageBox::Ok,QMessageBox::Ok);
}

// =================================================================================================

bool MainWindow::promptQuestion(QString msg)
{
  QMessageBox::StandardButton reply;

  reply = QMessageBox::question(this,tr("GooseEYE"),msg,QMessageBox::Yes|QMessageBox::No);

  if (reply == QMessageBox::Yes)
    return true;

  return false;
}

// =================================================================================================

bool MainWindow::exists(QString fname)
{
  QFileInfo finfo(data.path.filePath(fname));

  if ( finfo.exists() && finfo.isFile() )
    return true;
  else
    return false;
}

// =================================================================================================

size_t MainWindow::exists(QString f1, QString f2, QString f3)
{
  size_t out = 0;

  if ( exists(f1) ) { ++out; }
  if ( exists(f2) ) { ++out; }
  if ( exists(f3) ) { ++out; }

  return out;
}

// =================================================================================================

void MainWindow::on_pushButtonT0_load_clicked()
{
  // load file using dialog
  QString fname = QFileDialog::getOpenFileName(
    this,
     tr("Open previously stored state"),
     QStandardPaths::writableLocation(QStandardPaths::HomeLocation),
     tr("JSON Files (*.json *.JSON)")
  );
  if ( !QFileInfo(fname).exists() )
    return;

  // read JSON file
  try {
    data.read(fname);
  }
  catch (...) {
    promptWarning(tr("Unable to read JSON file"));
    return;
  }

  // store file-path / -name
  QFileInfo finfo(fname);
  data.path  = finfo.dir();
  data.fname = finfo.fileName();
}

// =================================================================================================

void MainWindow::on_pushButtonT0_path_clicked()
{
  // back-up current working directory
  QString dirPrev = data.path.absolutePath();

  // open file-dialog
  QString dirName = QFileDialog::getExistingDirectory(
    this,
    tr("Open Directory"),
    dirPrev,
    QFileDialog::ShowDirsOnly
  );

  // no or non-existing directory selected -> quit function
  if ( dirName.size()==0 )
    return;
  if ( !QDir(dirName).exists() )
    return;

  // update directory
  data.newPath(QDir(dirName));

  // default file-names
  QString f1 = "GooseEYE.json";
  QString f2 = "GooseEYE_result.pdf";
  QString f3 = "GooseEYE_result_interpretation.pdf";

  // if one of default files exists, find a number to append the files
  // (the same number if used for all files)
  if ( exists(f1,f2,f3) ) {
    f1 = "GooseEYE-%1.json";
    f2 = "GooseEYE-%1_result.pdf";
    f3 = "GooseEYE-%1_result_interpretation.pdf";
    int i = 1;
    while (exists(f1.arg(QString::number(i)),f2.arg(QString::number(i)),f3.arg(QString::number(i))))
      ++i;
    f1 = f1.arg(QString::number(i));
    f2 = f2.arg(QString::number(i));
    f3 = f3.arg(QString::number(i));
  }

  // store filenames
  data.fname        = f1;
  data.plot[0].name = f2;
  data.plot[1].name = f3;
}

// =================================================================================================

void MainWindow::on_lineEditT0_json_textEdited(const QString &text)
{
  data.fname = text;
}

// =================================================================================================

void MainWindow::on_lineEditT0_res1_textEdited(const QString &text)
{
  data.plot[0].name = text;
}

// =================================================================================================

void MainWindow::on_lineEditT0_res2_textEdited(const QString &text)
{
  data.plot[1].name = text;
}

// =================================================================================================

void MainWindow::tab0_show()
{
  ui->lineEditT0_path->setText(data.path.absolutePath());
  ui->lineEditT0_json->setText(data.fname);
  ui->lineEditT0_res1->setText(data.plot[0].name);
  ui->lineEditT0_res2->setText(data.plot[1].name);
}

// =================================================================================================

void MainWindow::tab1_read()
{
  // number of sets
  // - initialize
  size_t nset = 0;
  // - count
  for ( auto &i : nsetBtn )
    if ( i->isChecked() )
      ++nset;
  // - adapt size
  data.sets.resize(nset);
  // statistic
 for ( size_t i=0; i<statBtn.size(); ++i )
   if ( statBtn[i]->isChecked() )
     data.stat = statKey[i];
  // data-type
  for ( size_t j=0; j<data.sets.size(); ++j )
    for ( size_t i=0; i<typeKey.size(); ++i )
      if ( typeBtn[j*typeKey.size()+i]->isChecked() )
        data.sets[j].dtype = typeKey[i];
  // miscellaneous settings
  data.periodic    = ui->checkBoxT1_periodic  ->isChecked();
  data.zeropad     = ui->checkBoxT1_zeropad   ->isChecked();
  data.mask_weight = ui->checkBoxT1_maskWeight->isChecked();
  data.pixel_path  = ui->checkBoxT1_pixelPath ->currentText();
}

// =================================================================================================

void MainWindow::tab1_show()
{
  // clear all buttons
  for ( auto &i : btnGroup ) { i->setExclusive(false);                       }
  for ( auto &i : statBtn  ) { i->setChecked  (false); i->setEnabled(true ); }
  for ( auto &i : typeBtn  ) { i->setChecked  (false); i->setEnabled(false); }
  for ( auto &i : nsetBtn  ) { i->setChecked  (false); i->setEnabled(false); }
  for ( auto &i : btnGroup ) { i->setExclusive(true );                       }
  // fix number of sets
  if      ( data.stat=="L"                      ) { data.sets.resize(1);          }
  else if ( data.stat=="W2" || data.stat=="W2c" ) { data.sets.resize(2);          }
  else                                            { nsetBtn[1]->setEnabled(true); }
  // fix data types
  int b0,i0,f0,b1,i1,f1;
  if      ( data.stat=="S2"  ) { b0=1; i0=0; f0=1; b1=1; i1=0; f1=1; }
  else if ( data.stat=="C2"  ) { b0=0; i0=1; f0=0; b1=0; i1=1; f1=0; }
  else if ( data.stat=="L"   ) { b0=1; i0=1; f0=0; b1=0; i1=0; f1=0; }
  else if ( data.stat=="W2"  ) { b0=1; i0=0; f0=1; b1=1; i1=0; f1=1; }
  else if ( data.stat=="W2c" ) { b0=0; i0=1; f0=0; b1=1; i1=0; f1=1; }
  else                         { b0=0; i0=0; f0=0; b1=0; i1=0; f1=0; }
  std::vector<int> dt;
  dt.push_back(b0);
  dt.push_back(i0);
  dt.push_back(f0);
  dt.push_back(b1);
  dt.push_back(i1);
  dt.push_back(f1);
  // only one data-type allowed -> select
  for ( size_t j=0; j<data.sets.size(); ++j ) {
    size_t n=0;
    for ( size_t i=0; i<typeKey.size(); ++i )
      n += dt[j*typeKey.size()+i];
    if ( n==1 )
      for ( size_t i=0; i<typeKey.size(); ++i )
        if ( dt[j*typeKey.size()+i] )
          data.sets[j].dtype = typeKey[i];
  }
  // link data-types
  if ( data.sets.size()==2 ) {
    if      ( data.stat=="S2"  ) data.sets[1].dtype = data.sets[0].dtype;
    else if ( data.stat=="C2"  ) data.sets[1].dtype = data.sets[0].dtype;
  }
  // set fields
  QString n0,n1;
  if      ( data.stat=="S2"  ) { n0 = "phase" ; n1 = "phase"; }
  else if ( data.stat=="C2"  ) { n0 = "phase" ; n1 = "phase"; }
  else if ( data.stat=="L"   ) { n0 = "phase" ; n1 = ""     ; }
  else if ( data.stat=="W2"  ) { n0 = "weight"; n1 = "phase"; }
  else if ( data.stat=="W2c" ) { n0 = "weight"; n1 = "phase"; }
  else                         { n0 = ""      ; n1 = ""     ; }
  if ( data.sets.size()>=1 ) data.sets[0].field = n0;
  if ( data.sets.size()>=2 ) data.sets[1].field = n1;
  // enable data-types
  for ( size_t i=0; i<typeBtn.size(); ++i )
    if ( dt[i] )
      typeBtn[i]->setEnabled(true);
  // read statistic
  for ( size_t i=0; i<statBtn.size(); ++i )
    if ( data.stat==statKey[i] )
      statBtn[i]->setChecked(true);
  // read data-type (clear if stored data-type is not allowed)
 for ( size_t j=0; j<data.sets.size(); ++j ) {
   for ( size_t i=0; i<typeKey.size(); ++i ) {
     if ( data.sets[j].dtype==typeKey[i] ) {
       if ( typeBtn[j*typeKey.size()+i]->isEnabled() )
         typeBtn[j*typeKey.size()+i]->setChecked(true);
       else
        data.sets[j].dtype = "";
     }
   }
 }
  // set selected images
  for ( size_t j=0; j<data.sets.size(); ++j )
    nsetBtn[j]->setChecked(true);

  // selectively enable settings buttons
  // - defaults
  ui->checkBoxT1_periodic  ->setEnabled(true );
  ui->checkBoxT1_maskWeight->setEnabled(false);
  ui->labelT1_pixelPath    ->setEnabled(false);
  ui->checkBoxT1_pixelPath ->setEnabled(false);
  // - conditional
  if ( !ui->checkBoxT1_periodic->isChecked() )
    ui->checkBoxT1_zeropad->setEnabled(true );
  // - statistic specific: mask weights
  if ( data.stat=="W2" || data.stat=="W2c" )
    ui->checkBoxT1_maskWeight->setEnabled(true);
  // - statistic specific: pixel path
  if ( data.stat=="L" || data.stat=="W2c" ) {
    ui->checkBoxT1_zeropad  ->setEnabled(false);
    ui->labelT1_pixelPath   ->setEnabled(true );
    ui->checkBoxT1_pixelPath->setEnabled(true );
  }

  // apply settings
  ui->checkBoxT1_pixelPath ->setCurrentIndex(ui->checkBoxT1_pixelPath->findText(data.pixel_path));
  ui->checkBoxT1_periodic  ->setChecked(data.periodic   );
  ui->checkBoxT1_zeropad   ->setChecked(data.zeropad    );
  ui->checkBoxT1_maskWeight->setChecked(data.mask_weight);
}

// =================================================================================================

void MainWindow::fileAdd(size_t iset)
{
  // set filters to be used
  QStringList filters;
  filters << "Image files (*.png *.xpm *.jpg *.jpeg *.tif *.tiff *.bmp)"
          << "Any files (*)";

  // set-up read dialog
  QFileDialog dialog(this);
  dialog.setFileMode   (QFileDialog::ExistingFiles);
  dialog.setOption     (QFileDialog::HideNameFilterDetails,false);
  dialog.setDirectory  (data.path);
  dialog.setNameFilters(filters);
  dialog.setViewMode   (QFileDialog::List);

  // read list of files using dialog
  QStringList fileNames;
  if (dialog.exec())
    fileNames = dialog.selectedFiles();
  else
    return;

  // read current file selection
  std::vector<QString> files = data.sets[iset].fileNames();

  // add selected items to the end of the list, but only if they are new
  for ( QString &file : fileNames ) {
    // - get the relative name of the selected file
    QString fname = data.path.relativeFilePath(file);
    // - check to add and add
    if ( ! ( std::find(files.begin(),files.end(),fname)!=files.end() ) ) {
      File i;
      i.name = fname;
      data.sets[iset].files.push_back(i);
    }
  }
}

// =================================================================================================

void MainWindow::fileRmv(size_t iset)
{
  // list with selected rows
  std::vector<size_t> rows;
  foreach ( QListWidgetItem *item, fileLst[iset]->selectedItems() )
    rows.push_back(static_cast<size_t>(fileLst[iset]->row(item)));

  // remove selected rows
  data.sets[iset].rm(rows);

  // clear selection
  for ( int j=0; j<fileLst[iset]->count(); ++j )
    fileLst[iset]->item(j)->setSelected(false);
}

// =================================================================================================

void MainWindow::fileUp(size_t iset)
{
  // list with selected rows
  std::vector<int> rows;
  foreach ( QListWidgetItem *item, fileLst[iset]->selectedItems() )
    rows.push_back(fileLst[iset]->row(item));

  // any items at the top of the list -> do nothing (quit function)
  for ( auto &row : rows )
    if ( row<=0 )
      return;

  // sort row numbers, to ensure correct switching of items
  std::sort(rows.begin(),rows.end());

  // move selected rows one previous (up in the widget)
  for ( auto &row : rows )
    std::reverse(data.sets[iset].files.begin()+row-1,data.sets[iset].files.begin()+row+1);

  // restore file selection
  // - clear entire selection
  for ( int j=0; j<fileLst[iset]->count(); ++j )
    fileLst[iset]->item(j)->setSelected(false);
  // - apply previous selection, moved one up
  for ( auto &row : rows )
    fileLst[iset]->item(row-1)->setSelected(true);
}

// =================================================================================================

void MainWindow::fileDwn(size_t iset)
{
  // list with selected rows
  std::vector<int> rows;
  foreach ( QListWidgetItem *item, fileLst[iset]->selectedItems() )
    rows.push_back(fileLst[iset]->row(item));

  // any items at the bottom of the list -> do nothing (quit function)
  for ( auto &row : rows )
    if ( row>=fileLst[iset]->count()-1 )
      return;

  // sort row numbers in descending order, to ensure correct switching of items
  std::sort(rows.begin(),rows.end(),[](int i, int j){return i>j;});

  // move selected rows one forward (down in the widget)
  for ( auto &row : rows )
    std::reverse(data.sets[iset].files.begin()+row,data.sets[iset].files.begin()+row+2);

  // restore file selection
  // - clear entire selection
  for ( int j=0; j<fileLst[iset]->count(); ++j )
    fileLst[iset]->item(j)->setSelected(false);
  // - apply previous selection, moved one up
  for ( auto &row : rows )
    fileLst[iset]->item(row+1)->setSelected(true);
}

// =================================================================================================

void MainWindow::fileSrt(size_t iset)
{
  // sort by name
  std::sort(
    data.sets[iset].files.begin(),
    data.sets[iset].files.end(),
    [](File i, File j){return i.name<j.name;}
  );

  // clear file selection
  for ( int j=0; j<fileLst[iset]->count(); ++j )
    fileLst[iset]->item(j)->setSelected(false);
}

// =================================================================================================

void MainWindow::on_pushButtonT2_cp_clicked()
{
  if ( data.sets.size()<=1 )
    data.sets.resize(2);

  data.sets[1] = data.sets[0];
}

// =================================================================================================

void MainWindow::tab2_show()
{
  // disable all buttons by default (and clear labels), enable selectively below
  for ( auto &i : fileLst ) i->setEnabled(false);
  for ( auto &i : fileBtn ) i->setEnabled(false);
  for ( auto &i : propLbl ) i->setText   (""   );
  for ( auto &i : typeLbl ) i->setText   (""   );

  // for all sets
  for ( size_t iset=0; iset<data.sets.size(); ++iset ) {
    // - set labels
    propLbl[iset]->setText(data.sets[iset].field);
    typeLbl[iset]->setText(data.sets[iset].dtype);
    // - enable file list
    fileLst[iset]->setEnabled(true);
    // - store selected items
    std::vector<int> rows;
    foreach ( QListWidgetItem *item, fileLst[iset]->selectedItems() )
      rows.push_back(fileLst[iset]->row(item));
    // - empty file lists
    while ( fileLst[iset]->count()>0 )
      fileLst[iset]->takeItem(0);
    // - read files
    std::vector<QString> files = data.sets[iset].fileNames();
    // - fill file list
    for ( auto &file : files )
      fileLst[iset]->addItem(file);
    // - restore file selection
    for ( int j=0; j<fileLst[iset]->count(); ++j )
      fileLst[iset]->item(j)->setSelected(false);
    for ( auto &row : rows )
      fileLst[iset]->item(row)->setSelected(true);
    // - enable buttons
    if ( true                                  ) fileBtnAdd[iset]   ->setEnabled(true);
    if ( files.size()>0                        ) fileBtnRmv[iset]   ->setEnabled(true);
    if ( files.size()>0                        ) fileBtnUp [iset]   ->setEnabled(true);
    if ( files.size()>0                        ) fileBtnDwn[iset]   ->setEnabled(true);
    if ( files.size()>0                        ) fileBtnSrt[iset]   ->setEnabled(true);
    if ( files.size()>0 && data.sets.size()==2 ) ui->pushButtonT2_cp->setEnabled(true);
  }
}

// =================================================================================================

void MainWindow::tab3_default()
{
  // store the current indices
  int iset = ui->comboBoxT3_set->currentIndex();
  int iimg = ui->comboBoxT3_img->currentIndex();

  // check
  if ( iset<0 || iimg<0 )
    return;

  // mark to set default
  data.sets[iset].files[iimg].setDefault = true;
}

// =================================================================================================

void MainWindow::tab3_applyAll()
{
  // store the current indices
  int iset = ui->comboBoxT3_set->currentIndex();
  int iimg = ui->comboBoxT3_img->currentIndex();

  // check
  if ( iset<0 || iimg<0 )
    return;

  // mark to set default
  for ( int i=0; i<static_cast<int>(data.sets[iset].files.size()); ++i ) {
    if ( i!=iimg ) {
      QString name                  = data.sets[iset].files[i   ].name;
      data.sets[iset].files[i]      = data.sets[iset].files[iimg];
      data.sets[iset].files[i].name = name;
    }
  }
}

// =================================================================================================

void MainWindow::tab3_read()
{
  int iset = ui->comboBoxT3_set->currentIndex();
  int iimg = ui->comboBoxT3_img->currentIndex();

  if ( iset<0 || iimg<0 )
    return;

  File *file = &data.sets[iset].files[iimg];

  std::vector<size_t> phase(imSpnPhase.size());
  std::vector<size_t> mask (imSpnMask .size());
  std::vector<size_t> row  (imSpnRow  .size());
  std::vector<size_t> col  (imSpnCol  .size());

  for ( size_t i=0; i<imSpnPhase.size(); ++i ) phase[i] = imSpnPhase[i]->value();
  for ( size_t i=0; i<imSpnMask .size(); ++i ) mask [i] = imSpnMask [i]->value();
  for ( size_t i=0; i<imSpnRow  .size(); ++i ) row  [i] = imSpnRow  [i]->value();
  for ( size_t i=0; i<imSpnCol  .size(); ++i ) col  [i] = imSpnCol  [i]->value();

  file->phase.fromlist(phase);
  file->mask .fromlist(mask );
  file->row  .fromlist(row  );
  file->col  .fromlist(col  );

  file->phase.active = ui->checkBoxT3_phase->isChecked();
  file->mask .active = ui->checkBoxT3_mask ->isChecked();
  file->row  .active = ui->checkBoxT3_row  ->isChecked();
  file->col  .active = ui->checkBoxT3_col  ->isChecked();
}

// =================================================================================================

void MainWindow::tab3_show()
{
  // initialization
  //---------------

  // store the current indices
  int iset = ui->comboBoxT3_set->currentIndex();
  int iimg = ui->comboBoxT3_img->currentIndex();

  // clear both comboBoxes (enable below)
  ui->comboBoxT3_set->clear();
  ui->comboBoxT3_img->clear();
  ui->comboBoxT3_img->setEnabled(false);
  ui->comboBoxT3_set->setEnabled(false);

  // store number of sets
  int nset = static_cast<int>(data.sets.size());

  // sets -> comboBox
  // ----------------

  // fill comboBox with set name / field / type
  if ( nset>0 ) {
    // - enable
    ui->comboBoxT3_set->setEnabled(true);
    // - set name format
    QString name = "set%1: %2 (%3)";
    // - add items
    for ( int iset=0; iset<nset; ++iset ) {
      ui->comboBoxT3_set->addItem(name.arg(
        QString::number(iset),
        data.sets[iset].field,
        data.sets[iset].dtype
      ));
    }
    // - enable navigation buttons
    if ( nset>1 && iset>0      ) ui->pushButtonT3_setPrev->setEnabled(true);
    if ( nset>1 && iset<nset-1 ) ui->pushButtonT3_setNext->setEnabled(true);
    // - select the first set
    if ( iset<0 )
      iset = 0;
    // - (re)apply index
    ui->comboBoxT3_set->setCurrentIndex(iset);
  }

  // no set selected -> quit
  if ( iset<0 )
    return;

  // images in set -> comboBox
  // -------------------------

  // read files in current set
  std::vector<QString> files  = data.sets[iset].fileNames();
  int                  nfiles = static_cast<int>(files.size());
  // add files to comboBox
  for ( auto &file : files )
    ui->comboBoxT3_img->addItem(file);
  // enable comboBox and navigation buttons
  if ( files.size()>0                  ) ui->comboBoxT3_img      ->setEnabled(true);
  if ( files.size()>1 && iimg>0        ) ui->pushButtonT3_imgPrev->setEnabled(true);
  if ( files.size()>1 && iimg<nfiles-1 ) ui->pushButtonT3_imgNext->setEnabled(true);
  // select the first set
  if ( iimg<0 && files.size()>0 )
    iimg = 0;
  // reapply index
  ui->comboBoxT3_img->setCurrentIndex(iimg);

  // no image selected -> quit
  if ( iimg<0 )
    return;

  // read image from file -> show
  // ----------------------------

  // read image
  image = data.imageRead(iset,iimg);
  // set maxima for spinBox
  for ( auto &i : imSpnRow ) i->setMaximum(image.nrow);
  for ( auto &i : imSpnCol ) i->setMaximum(image.ncol);

  // show interpretation settings (spinBox)
  // --------------------------------------

  // alias
  File *file = &data.sets[iset].files[iimg];
  // convert settings to list
  std::vector<size_t> phase = file->phase.aslist(imSpnPhase.size());
  std::vector<size_t> mask  = file->mask .aslist(imSpnMask .size());
  std::vector<size_t> row   = file->row  .aslist(imSpnRow  .size());
  std::vector<size_t> col   = file->col  .aslist(imSpnCol  .size());
  // apply list to spinBox
  for ( size_t i=0; i<imSpnPhase.size(); ++i ) imSpnPhase[i]->setValue(phase[i]);
  for ( size_t i=0; i<imSpnMask .size(); ++i ) imSpnMask [i]->setValue(mask [i]);
  for ( size_t i=0; i<imSpnRow  .size(); ++i ) imSpnRow  [i]->setValue(row  [i]);
  for ( size_t i=0; i<imSpnCol  .size(); ++i ) imSpnCol  [i]->setValue(col  [i]);
  // update checkBox
  ui->checkBoxT3_phase->setChecked(file->phase.active);
  ui->checkBoxT3_mask ->setChecked(file->mask .active);
  ui->checkBoxT3_row  ->setChecked(file->row  .active);
  ui->checkBoxT3_col  ->setChecked(file->col  .active);

  // get interpreted image
  if ( file->phase.active || file->mask.active || file->row.active || file->col.active )
  {
    // interpret previously read image, keep the same size, but assign "-1" to pixels out-of-bounds
    data.image(iset,iimg,image,-1);

    // read "view" from "imgBin", "imgFlt", "clsInt", or "cenInt"; rescale range to 0..253
    // - float
    if ( data.sets[iset].dtype=="float" )
    {
      for ( size_t i=0; i<image.data.size(); ++i )
        image.view[i] = static_cast<int>(image.imgFlt[i]*253.);
    }
    // - binary
    if ( data.sets[iset].dtype=="binary" )
    {
      for ( size_t i=0; i<image.data.size(); ++i )
        image.view[i] = image.imgBin[i]*253;
    }
    // - int: distinguish between "L" and "W2c"
    if ( data.sets[iset].dtype=="int" )
    {
      // -- compute scale factor, to rescale to color-scale
      double fac = 255./static_cast<double>(image.clsInt.max());
      // -- cluster centers or clusters
      if ( data.stat=="W2c" ) {
        for ( size_t i=0; i<image.data.size(); ++i )
          image.view[i] = std::min(static_cast<int>(static_cast<double>(image.cenInt[i])*fac),253);
      }
      else {
        for ( size_t i=0; i<image.data.size(); ++i )
          image.view[i] = std::min(static_cast<int>(static_cast<double>(image.clsInt[i])*fac),253);
      }
    }

    // cropped pixels -> 245
    for ( size_t i=0; i<image.data.size(); ++i )
      if ( image.imgBin[i]==-1 )
        image.view[i] = 254;

    // masked pixels -> 255
    for ( size_t i=0; i<image.data.size(); ++i )
      if ( image.msk[i] )
        image.view[i] = 255;

    // mask weights
    if ( data.mask_weight && data.sets[iset].field=="phase" )
    {
      try {
        // - use the 'other' set
        size_t jset;
        if ( iset==0 ) jset = 1;
        if ( iset==1 ) jset = 0;
        // - read image
        QtImage tmp = data.image(jset,iimg,-1);
        // - set 'other' image as mask
        for ( size_t i=0; i<image.data.size(); ++i )
          if ( tmp.imgBin[i]>0 )
            image.view[i] = 255;
      }
      catch (...) {
        promptWarning("The weight must be set");
      }
    }
  }
}

// =================================================================================================

void MainWindow::tab3_imag()
{
  // store the current indices
  int iset = ui->comboBoxT3_set->currentIndex();
  int iimg = ui->comboBoxT3_img->currentIndex();
  // check
  if ( iset<0 || iimg<0 ) return;
  // set alias
  File *file = &data.sets[iset].files[iimg];

  // (re-)compute scale factor
  image.setScale(
    ui->graphicsViewT3_img->width(),
    ui->graphicsViewT3_img->height(),
    ui->sliderT3_zoom->sliderPosition()
  );

  // create a "scene" with containing the image
  QGraphicsPixmapItem *item  = new QGraphicsPixmapItem(QPixmap::fromImage(image.dataQt));
  QGraphicsScene      *scene = new QGraphicsScene;
  scene->addItem(item);
  ui->graphicsViewT3_img->setScene(scene);
  // show the image with the correct scaling applied
  ui->graphicsViewT3_img->setTransform(QTransform::fromScale(image.scale,image.scale));
  ui->graphicsViewT3_img->show();

  // get interpreted image
  if ( file->phase.active || file->mask.active || file->row.active || file->col.active )
  {
    // read colormap
    // - allocate
    std::vector<int> cols,tmp;
    // - read from library
    cols = cppcolormap::colormap(ui->comboBoxT3_cmap->currentText().toStdString(),254);
    // - optionally set white background
    if ( data.sets[iset].dtype=="int" )
      for ( size_t i=0; i<3; ++i )
        cols[0*3+i] = 255;
    // - excluded pixes -> white
    for ( size_t i=0; i<3; ++i )
      cols.push_back(255);
    // - read mask color from library
    tmp = cppcolormap::colormap(ui->comboBoxT3_cmapMask->currentText().toStdString(),1);
    // - copy mask color to the colormap
    for ( size_t i=0; i<3; ++i )
      cols.push_back(tmp[i]);
    // - convert to Qt format
    QVector<QRgb> cmap;
    for ( size_t i=0; i<256;++ i)
      cmap.append(qRgb(cols[i*3+0],cols[i*3+1],cols[i*3+2]));

    // convert mat::matrix -> QImage
    QImage imQt(
      image.view.data(),
      image.view.shape()[1],
      image.view.shape()[0],
      QImage::QImage::Format_Indexed8
    );
    // apply colormap
    imQt.setColorTable(cmap);
    // create a "scene" with the image
    QGraphicsPixmapItem *item  = new QGraphicsPixmapItem(QPixmap::fromImage(imQt));
    QGraphicsScene      *scene = new QGraphicsScene;
    scene->addItem(item);
    ui->graphicsViewT3_seg->setScene(scene);
    // show the image with the correct scaling applied
    ui->graphicsViewT3_seg->setTransform(QTransform::fromScale(image.scale,image.scale));
    ui->graphicsViewT3_seg->show();
  }
}

// =================================================================================================

void MainWindow::tab4_read()
{
  // read ROI-size: make sure that the value is always odd
  for ( size_t i=0; i<roiSpin.size(); ++i )
  {
    // - read value from spinBox
    int val = roiSpin[i]->value();
    // - ignore negative values
    if ( val<0 )
      continue;
    // - make even values odd
    if ( val%2==0 )
      ++val;
    // - store
    data.roi[i] = static_cast<size_t>(val);
  }

  // loop over plots
  for ( size_t iplt=0; iplt<resPlot.size(); ++iplt )
  {
    // - read colormap
    data.plot[iplt].cmap = resCombo[iplt]->currentText();
    // - read limits
    for ( size_t j=0; j<2; ++j ) data.plot[iplt].lim[j] = resSpin[iplt*2+j]->value();
  }

}

// =================================================================================================

void MainWindow::tab4_show()
{
  // ROI
  for ( size_t i=0; i<roiSpin.size(); ++i )
    roiSpin[i]->setValue(data.roi[i]);

  // set colormaps
  for ( size_t iplt=0; iplt<resPlot.size(); ++iplt )
  {
    // - convert "cmap" text to index, and apply index
    int row = resCombo[iplt]->findText(data.plot[iplt].cmap);
    if ( row>=0 ) resCombo[iplt]->setCurrentIndex(row);
    // - set limits
    for ( size_t j=0; j<2; ++j ) resSpin[iplt*2+j]->setValue(data.plot[iplt].lim[j]);
  }

  // default: disable compute button, only enable if conditions are met
  ui->pushButtonT4_compute->setEnabled(false);

  // - check that at least one file is selected
  if ( data.sets[0].files.size()<=0 )
    return;
  // - check file selection consistency, to avoid going out of bounds
  for ( size_t iset=1; iset<data.sets.size(); ++iset )
    if ( data.sets[iset].files.size()!=data.sets[0].files.size() )
      return;
  // - check that the ROI is positive, and odd
  for ( auto &i : data.roi ) {
    if ( i  <=0 ) return;
    if ( i%2==0 ) return;
  }

  // all checks passed: enable compute button
  ui->pushButtonT4_compute->setEnabled(true);
}

// =================================================================================================

void MainWindow::tab4_plot()
{
  // determine to exit
  if ( data.res.size()==0 )
    return;
  if ( data.res.shape()[0]!=data.roi[0] )
    return;
  if ( data.res.shape()[1]!=data.roi[1] )
    return;

  // show the spatial average (volume fraction)
  ui->doubleSpinBox_mean->setValue(data.mean/data.meannorm);

  // extract data for further processing below: normalized result, shape of output, colormap
  mat::matrix<double> D    = data.res/data.resnorm;
  int                 nx   = static_cast<int>(data.roi[0]);
  int                 ny   = static_cast<int>(data.roi[1]);
  size_t              ncol = 256;

  // loop over plots
  for ( size_t iplt=0; iplt<data.plot.size(); ++iplt )
  {
    // read colormap
    std::string         name = resCombo[iplt]->currentText().toStdString();
    std::vector<int>    cmap = cppcolormap::colormap(name,ncol);
    std::vector<float>  x    = cppcolormap::linspace(0.0,1.0,ncol);

    // configure axis
    resPlot[iplt]->axisRect()->setupFullAxesBox(true);
    resPlot[iplt]->xAxis->setLabel("Delta x");
    resPlot[iplt]->yAxis->setLabel("Delta y");

    // initialize the plot
    QCPColorMap *im = new QCPColorMap(resPlot[iplt]->xAxis,resPlot[iplt]->yAxis);
    // set size
    im->data()->setSize(nx,ny);
    // assign numbers of the range
    im->data()->setRange(QCPRange(-(nx-1)/2,+(nx-1)/2),QCPRange(-(ny-1)/2,+(ny-1)/2));
    // assign data
    for ( int i=0; i<nx; i++ )
      for (int j=0; j<ny; j++ )
        im->data()->setCell(i,j,D(i,j));

    // remove previous colorbar
    resPlot[iplt]->plotLayout()->remove(resPlot[iplt]->plotLayout()->element(0,1));
    // initialize new colorbar
    QCPColorScale *cbar = new QCPColorScale(resPlot[iplt]);
    // add colorbar to the plot
    resPlot[iplt]->plotLayout()->addElement(0,1,cbar);
    // set location
    cbar->setType(QCPAxis::atRight);
    // link to plot
    im->setColorScale(cbar);
    // set colors
    // - allocate
    QCPColorGradient qmap;
    qmap.setLevelCount(ncol);
    // - copy from colormap (read from library above)
    for ( size_t i=9; i<ncol; ++i )
      qmap.setColorStopAt(x[i],QColor(cmap[i*3+0],cmap[i*3+1],cmap[i*3+2]));
    // - apply
    im->setGradient(qmap);
    im->rescaleDataRange();
    // - position
    QCPMarginGroup *margin = new QCPMarginGroup(resPlot[iplt]);
    resPlot[iplt]->axisRect()->setMarginGroup(QCP::msBottom|QCP::msTop,margin);
    cbar->setMarginGroup(QCP::msBottom|QCP::msTop,margin);
    // - set the color-scale (from buttons)
    cbar->axis()->setRangeLower(data.plot[iplt].lim[0]);
    cbar->axis()->setRangeUpper(data.plot[iplt].lim[1]);

    // update the plot
    resPlot[iplt]->rescaleAxes();
    resPlot[iplt]->replot();
  }
}

// =================================================================================================

void MainWindow::tab4_save()
{
  // save JSON file
  data.write();

  // results
  for ( size_t iplt=0; iplt<resPlot.size(); ++iplt )
    resPlot[iplt]->savePdf(data.path.filePath(data.plot[iplt].name),0,0,QCP::epNoCosmetic);
}

// =================================================================================================

void MainWindow::compute()
{
  // initialize result / average
  data.res    .resize(data.roi); data.res    .zeros(); data.mean     *= 0.0;
  data.resnorm.resize(data.roi); data.resnorm.zeros(); data.meannorm *= 0.0;

  // save JSON file
  data.write();

  // set range progressBar
  ui->progressBarT4->setMinimum(  0);
  ui->progressBarT4->setMaximum(100);
  double N = 100.0/static_cast<double>(data.sets[0].files.size());

  // loop over images
  for ( size_t iimg=0; iimg<data.sets[0].files.size(); ++iimg )
  {
    // - set progress
    ui->progressBarT4->setValue(static_cast<int>(static_cast<double>(iimg)/N));
    // - compute statistic
    try {
      if      ( data.stat=="S2"  ) computeS2 (iimg);
      else if ( data.stat=="C2"  ) computeS2 (iimg);
      else if ( data.stat=="L"   ) computeL  (iimg);
      else if ( data.stat=="W2"  ) computeW2 (iimg);
      else if ( data.stat=="W2c" ) computeW2c(iimg);
    }
    catch (...) {
      promptWarning("Error in computation, see documentation of core functions");
      return;
    }
  }

  // overwrite JSON file (now includes the result)
  data.write();
}

// -------------------------------------------------------------------------------------------------

void MainWindow::computeS2(size_t iimg)
{
  // determine where to read from
  size_t jset=1;
  if ( data.sets.size()==1 ) jset=0;

  // alias
  QtImage f    = data.image(0   ,iimg);
  QtImage g    = data.image(jset,iimg);
  bool    flt  = data.sets[0].dtype=="float";
  bool    cls  = data.sets[0].dtype=="int";
  bool    bin  = data.sets[0].dtype=="binary";
  bool    msk  = f.msk.sum()>0 || g.msk.sum()>0 || !data.periodic || data.zeropad;
  bool    pad  = data.zeropad;
  bool    per  = data.periodic;

  // allocate temporary variables
  double              ns = 0.0;
  mat::matrix<double> r;
  mat::matrix<double> n   (data.roi); n   .zeros();
  mat::matrix<double> ones(data.roi); ones.ones ();

  // compute the statistic
  if      ( flt && msk ) std::tie(r,n ) = Image::S2(f.imgFlt,g.imgFlt,data.roi,f.msk,g.msk,pad,per);
  else if ( cls && msk ) std::tie(r,n ) = Image::S2(f.clsInt,g.clsInt,data.roi,f.msk,g.msk,pad,per);
  else if ( bin && msk ) std::tie(r,n ) = Image::S2(f.imgBin,g.imgBin,data.roi,f.msk,g.msk,pad,per);
  else if ( flt        ) std::tie(r,ns) = Image::S2(f.imgFlt,g.imgFlt,data.roi);
  else if ( cls        ) std::tie(r,ns) = Image::S2(f.clsInt,g.clsInt,data.roi);
  else if ( bin        ) std::tie(r,ns) = Image::S2(f.imgBin,g.imgBin,data.roi);

  // add result to ensemble
  n            += ns*ones; // if normalization is scalar, convert to matrix (does nothing otherwise)
  data.res     += n *r;    // add unnormalized result to ensemble
  data.resnorm += n;       // add normalization to ensemble

  // update ensemble mean
  data.mean += f.mean*f.N; data.meannorm += f.N;
  data.mean += g.mean*g.N; data.meannorm += g.N;
}

// -------------------------------------------------------------------------------------------------

void MainWindow::computeL(size_t iimg)
{
  // alias
  QtImage     f    = data.image(0,iimg);
  bool        cls  = data.sets[0].dtype=="int";
  bool        bin  = data.sets[0].dtype=="binary";
  bool        per  = data.periodic;
  std::string mode = data.pixel_path.toStdString();

  // allocate temporary variables
  mat::matrix<double> r,n;

  // compute the statistic
  if      ( cls ) std::tie(r,n) = Image::L(f.clsInt,data.roi,mode,per);
  else if ( bin ) std::tie(r,n) = Image::L(f.imgBin,data.roi,mode,per);

  // add result to ensemble
  data.res     += n*r;     // add unnormalized result to ensemble
  data.resnorm += n;       // add normalization to ensemble

  // update ensemble mean
  data.mean += f.mean*f.N; data.meannorm += f.N;
}

// -------------------------------------------------------------------------------------------------

void MainWindow::computeW2(size_t iimg)
{
  // alias
  QtImage             W   = data.image(0,iimg);
  QtImage             I   = data.image(1,iimg);
  bool                fl0 = data.sets[0].dtype=="float";
  bool                fl1 = data.sets[1].dtype=="float";
  bool                msk = I.msk.sum()>0 || !data.periodic || data.zeropad;
  bool                pad = data.zeropad;
  bool                per = data.periodic;
  std::vector<size_t> roi = data.roi;

  // allocate temporary variables
  double              ns = 0.0;
  mat::matrix<double> r;
  mat::matrix<double> n   (data.roi); n   .zeros();
  mat::matrix<double> ones(data.roi); ones.ones ();

  // compute the statistic
  if      (  fl0 &&  fl1 && msk ) std::tie(r,n ) = Image::W2(W.imgFlt,I.imgFlt,roi,I.msk,pad,per);
  else if ( !fl0 &&  fl1 && msk ) std::tie(r,n ) = Image::W2(W.imgBin,I.imgFlt,roi,I.msk,pad,per);
  else if (  fl0 && !fl1 && msk ) std::tie(r,n ) = Image::W2(W.imgFlt,I.imgBin,roi,I.msk,pad,per);
  else if ( !fl0 && !fl1 && msk ) std::tie(r,n ) = Image::W2(W.imgBin,I.imgBin,roi,I.msk,pad,per);
  else if (  fl0 &&  fl1        ) std::tie(r,ns) = Image::W2(W.imgFlt,I.imgFlt,roi);
  else if ( !fl0 &&  fl1        ) std::tie(r,ns) = Image::W2(W.imgBin,I.imgFlt,roi);
  else if (  fl0 && !fl1        ) std::tie(r,ns) = Image::W2(W.imgFlt,I.imgBin,roi);
  else if ( !fl0 && !fl1        ) std::tie(r,ns) = Image::W2(W.imgBin,I.imgBin,roi);

  // add result to ensemble
  n            += ns*ones; // if normalization is scalar, convert to matrix (does nothing otherwise)
  data.res     += n *r;    // add unnormalized result to ensemble
  data.resnorm += n;       // add normalization to ensemble

  // update ensemble mean
  data.mean += I.mean*I.N; data.meannorm += I.N;
}

// -------------------------------------------------------------------------------------------------

void MainWindow::computeW2c(size_t iimg)
{
  // alias
  QtImage             W    = data.image(0,iimg);
  QtImage             I    = data.image(1,iimg);
  bool                flt  = data.sets[1].dtype=="float";
  bool                per  = data.periodic;
  std::string         mode = data.pixel_path.toStdString();
  std::vector<size_t> roi  = data.roi;

  // allocate temporary variables
  mat::matrix<double> r,n;

  // compute the statistic
  if ( flt ) std::tie(r,n) = Image::W2c(W.clsInt,W.cenInt,I.imgFlt,roi,I.msk,mode,per);
  else       std::tie(r,n) = Image::W2c(W.clsInt,W.cenInt,I.imgBin,roi,I.msk,mode,per);

  // add result to ensemble
  data.res     += n*r;    // add unnormalized result to ensemble
  data.resnorm += n;       // add normalization to ensemble

  // update ensemble mean
  data.mean += I.mean*I.N; data.meannorm += I.N;
}

// =================================================================================================

void MainWindow::openHelp()
{
  QString link = "http://gooseeye.geus.me";
  QDesktopServices::openUrl(QUrl(link));
}
