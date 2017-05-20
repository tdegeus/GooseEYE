#include "mainwindow.h"
#include "ui_mainwindow.h"

// =================================================================================================

std::string updatePath (QDir old, QDir cur, std::string fname)
{
  if ( fname.size()==0 )
    return fname;

  return cur.relativeFilePath(old.filePath(QString::fromStdString(fname))).toStdString();
}

// =================================================================================================

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  // initialize GUI
  ui->setupUi(this);

  // always start on first tab
  ui->tabWidget->setCurrentIndex(0);

  // set default data (also useful to reduce the number of checks in the code)
  checkData();

  // tab3: link scroll position of graphicsViews
  QScrollBar *I1h = ui->graphicsViewT3_img->horizontalScrollBar();
  QScrollBar *I2h = ui->graphicsViewT3_seg->horizontalScrollBar();
  QScrollBar *I1v = ui->graphicsViewT3_img->verticalScrollBar  ();
  QScrollBar *I2v = ui->graphicsViewT3_seg->verticalScrollBar  ();
  connect(I1h,SIGNAL(valueChanged(int)),I2h,SLOT(setValue(int)));
  connect(I2h,SIGNAL(valueChanged(int)),I1h,SLOT(setValue(int)));
  connect(I1v,SIGNAL(valueChanged(int)),I2v,SLOT(setValue(int)));
  connect(I2v,SIGNAL(valueChanged(int)),I1v,SLOT(setValue(int)));

  // tab3: switch set/image
  // - alias variables
  QComboBox *img = ui->comboBoxT3_img;
  QComboBox *set = ui->comboBoxT3_set;
  // - previous image
  connect(ui->pushButtonT3_imgPrev ,&QPushButton::clicked,[=](){
    if (img->currentIndex()>0             ) img->setCurrentIndex(img->currentIndex()-1);});
  // - next image
  connect(ui->pushButtonT3_imgNext ,&QPushButton::clicked,[=](){
    if (img->currentIndex()<img->count()-1) img->setCurrentIndex(img->currentIndex()+1);});
  // - previous iset
  connect(ui->pushButtonT3_setPrev,&QPushButton::clicked,[=](){
    if (set->currentIndex()>0             ) set->setCurrentIndex(set->currentIndex()-1);});
  // - next set
  connect(ui->pushButtonT3_setNext,&QPushButton::clicked,[=](){
    if (set->currentIndex()<set->count()-1) set->setCurrentIndex(set->currentIndex()+1);});

  // combine buttons to list, for fast references
  // - tab1: radioButton group to select statistic, and their field-names
  statBtn.push_back(ui->radioButtonT1_S2    ); statKey.push_back("S2"    );
  statBtn.push_back(ui->radioButtonT1_C2    ); statKey.push_back("C2"    );
  statBtn.push_back(ui->radioButtonT1_L     ); statKey.push_back("L"     );
  statBtn.push_back(ui->radioButtonT1_W2    ); statKey.push_back("W2"    );
  statBtn.push_back(ui->radioButtonT1_W2c   ); statKey.push_back("W2c"   );
  // - tab1: checkBox group to select number of sets, and their field-names
  nsetBtn.push_back(ui->checkBoxT1_set1     ); nsetKey.push_back("set0"  );
  nsetBtn.push_back(ui->checkBoxT1_set0     ); nsetKey.push_back("set1"  );
  // - tab1: radioButton group to select data-type (one for each set), and their field-names
  typeBtn.push_back(ui->radioButtonT1_img0b ); typeKey.push_back("binary");
  typeBtn.push_back(ui->radioButtonT1_img0i ); typeKey.push_back("int"   );
  typeBtn.push_back(ui->radioButtonT1_img0f ); typeKey.push_back("float" );
  typeBtn.push_back(ui->radioButtonT1_img1b ); typeKey.push_back("binary");
  typeBtn.push_back(ui->radioButtonT1_img1i ); typeKey.push_back("int"   );
  typeBtn.push_back(ui->radioButtonT1_img1f ); typeKey.push_back("float" );
  // - tab2: pushButton group of all buttons on this tab
  fileBtn.push_back(ui->pushButtonT2_set0Add); fileBtnAdd.push_back(ui->pushButtonT2_set0Add);
  fileBtn.push_back(ui->pushButtonT2_set1Add); fileBtnAdd.push_back(ui->pushButtonT2_set1Add);
  fileBtn.push_back(ui->pushButtonT2_set0Rmv); fileBtnRmv.push_back(ui->pushButtonT2_set0Rmv);
  fileBtn.push_back(ui->pushButtonT2_set1Rmv); fileBtnRmv.push_back(ui->pushButtonT2_set1Rmv);
  fileBtn.push_back(ui->pushButtonT2_set0Up ); fileBtnUp .push_back(ui->pushButtonT2_set0Up );
  fileBtn.push_back(ui->pushButtonT2_set1Up ); fileBtnUp .push_back(ui->pushButtonT2_set1Up );
  fileBtn.push_back(ui->pushButtonT2_set0Dwn); fileBtnDwn.push_back(ui->pushButtonT2_set0Dwn);
  fileBtn.push_back(ui->pushButtonT2_set1Dwn); fileBtnDwn.push_back(ui->pushButtonT2_set1Dwn);
  fileBtn.push_back(ui->pushButtonT2_set0Srt); fileBtnSrt.push_back(ui->pushButtonT2_set0Srt);
  fileBtn.push_back(ui->pushButtonT2_set1Srt); fileBtnSrt.push_back(ui->pushButtonT2_set1Srt);
  fileBtn.push_back(ui->pushButtonT2_cp     );
  // - tab2: listWidget group of all file-lists
  fileLst.push_back(ui->listWidgetT2_img0   );
  fileLst.push_back(ui->listWidgetT2_img1   );
  // - tab2: label group of all field labels ("phase","weight")
  propLbl.push_back(ui->tab2_im0Phase_label );
  propLbl.push_back(ui->tab2_im1Phase_label );
  // - tab2: label group of data-type labels ("binary","int","float")
  typeLbl.push_back(ui->tab2_im0Dtype_label );
  typeLbl.push_back(ui->tab2_im1Dtype_label );
  // - tab3: spinBox group to interpret the image
  imgSpin.push_back(ui->spinBoxT3_phaseLow  ); imgSpin.push_back(ui->spinBoxT3_phaseHgh);
  imgSpin.push_back(ui->spinBoxT3_mask0Low  ); imgSpin.push_back(ui->spinBoxT3_mask0Hgh);
  imgSpin.push_back(ui->spinBoxT3_mask1Low  ); imgSpin.push_back(ui->spinBoxT3_mask1Hgh);
  imgSpin.push_back(ui->spinBoxT3_mask2Low  ); imgSpin.push_back(ui->spinBoxT3_mask2Hgh);
  imgSpin.push_back(ui->spinBoxT3_rowLow    ); imgSpin.push_back(ui->spinBoxT3_rowHgh  );
  imgSpin.push_back(ui->spinBoxT3_colLow    ); imgSpin.push_back(ui->spinBoxT3_colHgh  );
  // - tab3: checkBox to select a modification field, and their field-names
                                                                               imgVal.push_back( 0);
  imgCheck.push_back(ui->checkBoxT3_phase   ); imgCheckLbl.push_back("phase"); imgVal.push_back( 2);
  imgCheck.push_back(ui->checkBoxT3_mask    ); imgCheckLbl.push_back("mask" ); imgVal.push_back( 8);
  imgCheck.push_back(ui->checkBoxT3_row     ); imgCheckLbl.push_back("row"  ); imgVal.push_back(10);
  imgCheck.push_back(ui->checkBoxT3_col     ); imgCheckLbl.push_back("col"  ); imgVal.push_back(12);
  // - tab3: pushButton to navigate through images
  imgBtn.push_back(ui->pushButtonT3_setPrev ); imgBtn.push_back(ui->pushButtonT3_setNext);
  imgBtn.push_back(ui->pushButtonT3_imgPrev ); imgBtn.push_back(ui->pushButtonT3_imgNext);
  imgBtn.push_back(ui->pushButtonT3_clear   );
  imgBtn.push_back(ui->pushButtonT3_apply   );
  imgBtn.push_back(ui->pushButtonT3_zoomIm  );
  imgBtn.push_back(ui->pushButtonT3_zoomOut );
  // - tab3: comboBox to select set, image, and colormaps
  imgCombo.push_back(ui->comboBoxT3_set     );
  imgCombo.push_back(ui->comboBoxT3_img     );
  imgCombo.push_back(ui->comboBoxT3_cmap    );
  imgCombo.push_back(ui->comboBoxT3_cmapMask);
  // - tab3: radioButtons to select pixels using mouse-click
  imgRadio.push_back(ui->radioButtonT3_phase);
  imgRadio.push_back(ui->radioButtonT3_mask );
  imgRadio.push_back(ui->radioButtonT3_row  );
  imgRadio.push_back(ui->radioButtonT3_col  );
  // - groups of radioButtons
  btnGroup.push_back(ui->buttonGroupT1_stat );
  btnGroup.push_back(ui->buttonGroupT1_set0 );
  btnGroup.push_back(ui->buttonGroupT1_set1 );
  btnGroup.push_back(ui->buttonGroupT3_mouse);

  // TODO: implement mouse selection
  for ( auto &i : imgRadio ) i->setVisible(false);

    // tab1: button pressed -> update "data"
  for ( auto &i : statBtn ) connect(i,&QPushButton::clicked,this,[=](){tab1_read();});
  for ( auto &i : typeBtn ) connect(i,&QPushButton::clicked,this,[=](){tab1_read();});
  for ( auto &i : nsetBtn ) connect(i,&QPushButton::clicked,this,[=](){tab1_read();});

  // tab2: connect buttons to file manipulations -> update "data"
  for ( size_t i=0; i<2; ++i ) connect(fileBtnAdd[i],&QPushButton::clicked,this,[=](){fileAdd(i);});
  for ( size_t i=0; i<2; ++i ) connect(fileBtnRmv[i],&QPushButton::clicked,this,[=](){fileRmv(i);});
  for ( size_t i=0; i<2; ++i ) connect(fileBtnUp [i],&QPushButton::clicked,this,[=](){fileUp (i);});
  for ( size_t i=0; i<2; ++i ) connect(fileBtnDwn[i],&QPushButton::clicked,this,[=](){fileDwn(i);});
  for ( size_t i=0; i<2; ++i ) connect(fileBtnSrt[i],&QPushButton::clicked,this,[=](){fileSrt(i);});

    // refresh file related views when JSON is loaded or "out_path" is changed
  connect(ui->pushButtonT0_path,&QPushButton::clicked,this,[=](){tab0_show();});
  connect(ui->pushButtonT0_path,&QPushButton::clicked,this,[=](){tab2_show();});
  connect(ui->pushButtonT0_load,&QPushButton::clicked,this,[=](){tab0_show();});
  connect(ui->pushButtonT0_load,&QPushButton::clicked,this,[=](){tab1_show();});
  connect(ui->pushButtonT0_load,&QPushButton::clicked,this,[=](){tab2_show();});

  // -
  connect(ui->pushButtonT0_load,&QPushButton::clicked,this,[=](){checkData();});
  for ( auto &i : fileBtnAdd ) connect(i,&QPushButton::clicked,this,[=](){checkData();});
  for ( auto &i : fileBtnRmv ) connect(i,&QPushButton::clicked,this,[=](){checkData();});





  // tab3: read from spinBox
  for ( auto &i : imgSpin  ) connect(i,&QSpinBox::editingFinished,this,[=](){tab3_read();});
  for ( auto &i : imgCheck ) connect(i,&QCheckBox::clicked       ,this,[=](){tab3_read();});

  // size_t j,N=imgSpin.size(),M=imgCheck.size();
  // for ( j=0; j<N; ++j ) connect(imgSpin [j],&QSpinBox::editingFinished,this,[=](){tab3_read(j  );});
  // for ( j=1; j<N; ++j ) connect(imgSpin [j],&QSpinBox::editingFinished,this,[=](){tab3_read(j  );});
  // for ( j=0; j<M; ++j ) connect(imgCheck[j],&QCheckBox::clicked       ,this,[=](){tab3_read(j*2);});

  // tab2/tab3: button pressed -> refresh view with new "data"
  for ( auto &i : statBtn  ) connect(i,&QPushButton::clicked     ,this,[=](){tab1_show();});
  for ( auto &i : typeBtn  ) connect(i,&QPushButton::clicked     ,this,[=](){tab1_show();});
  for ( auto &i : nsetBtn  ) connect(i,&QPushButton::clicked     ,this,[=](){tab1_show();});
  for ( auto &i : fileBtn  ) connect(i,&QPushButton::clicked     ,this,[=](){tab2_show();});
  for ( auto &i : imgBtn   ) connect(i,&QPushButton::clicked     ,this,[=](){tab3_show();});
  for ( auto &i : imgSpin  ) connect(i,&QSpinBox::editingFinished,this,[=](){tab3_show();});
  for ( auto &i : imgCheck ) connect(i,&QCheckBox::clicked       ,this,[=](){tab3_show();});
  for ( auto &i : imgCombo ) connect(i,SIGNAL(activated(int)),this,SLOT(tab3_show()));

  // refresh tabs when tab is changed
  connect(ui->tabWidget,&QTabWidget::currentChanged,[=](){tab0_show();});
  connect(ui->tabWidget,&QTabWidget::currentChanged,[=](){tab1_show();});
  connect(ui->tabWidget,&QTabWidget::currentChanged,[=](){tab2_show();});
  connect(ui->tabWidget,&QTabWidget::currentChanged,[=](){tab3_show();});
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
  QFileInfo finfo(out_path.filePath(fname));

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

void MainWindow::checkData()
{
  if ( !data.count("stat"       ) ) data["stat"       ] = "";
  if ( !data.count("nset"       ) ) data["nset"       ] = 1;
  if ( !data.count("periodic"   ) ) data["periodic"   ] = true;
  if ( !data.count("zeropad"    ) ) data["zeropad"    ] = false;
  if ( !data.count("mask_weight") ) data["mask_weight"] = true;
  if ( !data.count("pixel_path" ) ) data["pixel_path" ] = "Bresenham";
  if ( !data.count("roi"        ) ) data["roi"        ] = {0,0};

  if ( !data.count("output") ) {
    data["output"]["result"] = "";
    data["output"]["interp"] = "";
  }
  if ( !data["output"].count("result") ) data["output"]["result"] = "";
  if ( !data["output"].count("interp") ) data["output"]["interp"] = "";

  std::vector<std::string> sets;
  if ( data["nset"]>=1 ) sets.push_back("set0");
  if ( data["nset"]>=2 ) sets.push_back("set1");

  for ( auto &set : sets ) {
    if ( !data.count(set) ) {
      data[set]["field" ] = "";
      data[set]["dtype" ] = "";
      data[set]["files" ] = {};
      data[set]["config"] = {};
    }
    if ( !data[set].count("field" ) ) data[set]["field" ] = "";
    if ( !data[set].count("dtype" ) ) data[set]["dtype" ] = "";
    if ( !data[set].count("files" ) ) data[set]["files" ] = "";
    if ( !data[set].count("config") ) data[set]["config"] = "";
  }

  for ( auto &set : sets ) {
    // collect files
    // - allocate
    std::vector<std::string> files;
    // - fill
    for ( size_t i=0; i<data[set]["files"].size(); ++i )
      files.push_back(data[set]["files"][i]);
    // collect files stored in "config"
    // - allocate
    std::vector<std::string> config;
    // - fill
    for ( json::iterator it=data[set]["config"].begin(); it!=data[set]["config"].end(); ++it )
      config.push_back(it.key());
    // remove files present in "config", but not in "files"
    for ( auto &fname : config )
      if ( ! ( std::find(files.begin(),files.end(),fname)!=files.end() ) )
        data[set]["config"].erase(fname);
    // initialize defaults
    std::vector<int> phase = {0,256};
    std::vector<int> mask  = {0,0,0,0,0,0};
    std::vector<int> row   = {0,0};
    std::vector<int> col   = {0,0};
    // apply defaults for files that are present in "files" but not in "config"
    for ( auto &fname : files ) {
      if ( ! ( std::find(config.begin(),config.end(),fname)!=config.end() ) ) {
        data[set]["config"][fname]["phase"]["values"] = phase;
        data[set]["config"][fname]["phase"]["active"] = false;
        data[set]["config"][fname]["mask" ]["values"] = mask;
        data[set]["config"][fname]["mask" ]["active"] = false;
        data[set]["config"][fname]["row"  ]["values"] = row;
        data[set]["config"][fname]["row"  ]["active"] = false;
        data[set]["config"][fname]["col"  ]["values"] = col;
        data[set]["config"][fname]["col"  ]["active"] = false;
      }
      else {
        if ( !data[set]["config"][fname].count("phase") ) {
          data[set]["config"][fname]["phase"]["values"] = phase;
          data[set]["config"][fname]["phase"]["active"] = false;
        }
        if ( !data[set]["config"][fname].count("mask") ) {
          data[set]["config"][fname]["mask"]["values"] = mask;
          data[set]["config"][fname]["mask"]["active"] = false;
        }
        if ( !data[set]["config"][fname].count("row") ) {
          data[set]["config"][fname]["row"]["values"] = row;
          data[set]["config"][fname]["row"]["active"] = false;
        }
        if ( !data[set]["config"][fname].count("col") ) {
          data[set]["config"][fname]["col"]["values"] = col;
          data[set]["config"][fname]["col"]["active"] = false;
        }
      }
    }
  }
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

  // try to see if the JSON file is corrupted, is so show error pop-up
  try {
    json tmp;
    std::ifstream inp(fname.toStdString());
    inp >> tmp;
  }
  catch (...) {
    promptWarning(tr("Unable to read JSON file"));
  }

  // convert JSON file to data object
  data.clear();
  std::ifstream inp(fname.toStdString());
  inp >> data;

  // store paths based in the path of the JSON file
  QFileInfo finfo(fname);
  out_path = finfo.dir();
  out_json = finfo.fileName();
}

// =================================================================================================

void MainWindow::on_pushButtonT0_path_clicked()
{
  // back-up current working directory
  QString dirPrev = out_path.absolutePath();

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

  // store directory
  out_path = QDir(dirName);

  // working directory changed -> convert relative paths to new relative paths
  if ( dirPrev.size()>0 && dirPrev!=dirName ) {
    for ( auto &set : nsetKey ) {
      for ( size_t j=0; j<data[set]["files"].size(); ++j ) {
        std::string prev = data[set]["files"][j];
        std::string next = updatePath(QDir(dirPrev),out_path,prev);
        data[set]["files" ][j   ] = next;
        data[set]["config"][next] = data[set]["config"][prev];
        data[set]["config"].erase(prev);
      }
    }
  }

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
  out_json                 = f1;
  data["output"]["result"] = f2.toStdString();
  data["output"]["interp"] = f2.toStdString();
}

// =================================================================================================

void MainWindow::on_lineEditT0_path_textEdited(const QString &arg1)
{
  out_path = QDir(arg1);
}

// =================================================================================================

void MainWindow::on_lineEditT0_json_textEdited(const QString &arg1)
{
  out_json = arg1;
}

// =================================================================================================

void MainWindow::on_lineEditT0_res1_textEdited(const QString &arg1)
{
  data["output"]["result"] = arg1.toStdString();
}

// =================================================================================================

void MainWindow::on_lineEditT0_res2_textEdited(const QString &arg1)
{
  data["output"]["interp"] = arg1.toStdString();
}

// =================================================================================================

void MainWindow::tab0_show()
{
  ui->lineEditT0_path->setText(out_path.absolutePath());
  ui->lineEditT0_json->setText(out_json);
  ui->lineEditT0_res1->setText(QString::fromStdString(data["output"]["result"]));
  ui->lineEditT0_res2->setText(QString::fromStdString(data["output"]["interp"]));
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
  // - store
  data["nset"] = nset;
  // statistic
  for ( size_t i=0; i<statBtn.size(); ++i )
    if ( statBtn[i]->isChecked() )
      data["stat"] = statKey[i];
  // data-type
  for ( size_t j=0; j<2; ++j )
    for ( size_t i=0; i<3; ++i )
      if ( typeBtn[j*3+i]->isChecked() )
        data[nsetKey[j]]["dtype"] = typeKey[i];
  // miscellaneous settings
  data["periodic"   ] = ui->checkBoxT1_periodic  ->isChecked();
  data["zeropad"    ] = ui->checkBoxT1_zeropad   ->isChecked();
  data["mask_weight"] = ui->checkBoxT1_maskWeight->isChecked();
  data["pixel_path" ] = ui->checkBoxT1_pixelPath->currentText().toStdString();
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
  if      ( data["stat"]=="L"                         ) { data["nset"] = 1;             }
  else if ( data["stat"]=="W2" || data["stat"]=="W2c" ) { data["nset"] = 2;             }
  else                                                  { nsetBtn[1]->setEnabled(true); }
  // fix data types
  int b0,i0,f0,b1,i1,f1;
  if      ( data["stat"]=="S2"  ) { b0=1; i0=0; f0=1; b1=1; i1=0; f1=1; }
  else if ( data["stat"]=="C2"  ) { b0=0; i0=1; f0=0; b1=0; i1=1; f1=0; }
  else if ( data["stat"]=="L"   ) { b0=1; i0=1; f0=0; b1=0; i1=0; f1=0; }
  else if ( data["stat"]=="W2"  ) { b0=1; i0=0; f0=1; b1=1; i1=0; f1=1; }
  else if ( data["stat"]=="W2c" ) { b0=0; i0=1; f0=0; b1=1; i1=0; f1=1; }
  else                            { b0=0; i0=0; f0=0; b1=0; i1=0; f1=0; }
  std::vector<int> dt;
  dt.push_back(b0);
  dt.push_back(i0);
  dt.push_back(f0);
  dt.push_back(b1);
  dt.push_back(i1);
  dt.push_back(f1);
  // set fields
  std::string n0,n1;
  if      ( data["stat"]=="S2"  ) { n0 = "phase" ; n1 = "phase"; }
  else if ( data["stat"]=="C2"  ) { n0 = "phase" ; n1 = "phase"; }
  else if ( data["stat"]=="L"   ) { n0 = "phase" ; n1 = ""     ; }
  else if ( data["stat"]=="W2"  ) { n0 = "weight"; n1 = "phase"; }
  else if ( data["stat"]=="W2c" ) { n0 = "weight"; n1 = "phase"; }
  else                            { n0 = ""      ; n1 = ""     ; }
  data[nsetKey[0]]["field"] = n0;
  data[nsetKey[1]]["field"] = n1;
  // read number of sets
  size_t n = data["nset"];
  // enable data-types
  for ( size_t i=0; i<typeBtn.size(); ++i )
    if ( dt[i] )
      typeBtn[i]->setEnabled(true);
  // read statistic
  for ( size_t i=0; i<statBtn.size(); ++i )
    if ( data["stat"]==statKey[i] )
      statBtn[i]->setChecked(true);
  // read data-type
  for ( size_t j=0; j<n; ++j )
    for ( size_t i=0; i<3; ++i )
      if ( data[nsetKey[j]]["dtype"]==typeKey[j*3+i] )
        if ( typeBtn[j*3+i]->isEnabled() )
          typeBtn[j*3+i]->setChecked(true);
  // set selected images
  for ( size_t j=0; j<n; ++j )
    nsetBtn[j]->setChecked(true);

  // selectively enable settings buttons
  // - defaults
  ui->checkBoxT1_periodic    ->setEnabled(true );
  ui->checkBoxT1_zeropad     ->setEnabled(true );
  ui->checkBoxT1_maskWeight  ->setEnabled(false);
  ui->labelT1_pixelPath      ->setEnabled(false);
  ui->checkBoxT1_pixelPath   ->setEnabled(false);
  // - statistic specific: mask weights
  if ( data["stat"]=="W2" || data["stat"]=="W2c" )
    ui->checkBoxT1_maskWeight->setEnabled(true);
  // - statistic specific: pixel path
  if ( data["stat"]=="L" || data["stat"]=="W2c" ) {
    ui->checkBoxT1_zeropad   ->setEnabled(false);
    ui->labelT1_pixelPath    ->setEnabled(true );
    ui->checkBoxT1_pixelPath ->setEnabled(true );
  }

  // apply settings
  QString pth = QString::fromStdString(data["pixel_path"]);
  ui->checkBoxT1_pixelPath  ->setCurrentIndex(ui->checkBoxT1_pixelPath->findText(pth));
  ui->checkBoxT1_periodic   ->setChecked(data["periodic"   ]);
  ui->checkBoxT1_zeropad    ->setChecked(data["zeropad"    ]);
  ui->checkBoxT1_maskWeight ->setChecked(data["mask_weight"]);
}

// =================================================================================================

QString MainWindow::readFilePath(size_t iset, size_t iimg)
{
  return out_path.absoluteFilePath(readFile(iset,iimg));
}

// =================================================================================================

QString MainWindow::readFile(size_t iset, size_t iimg)
{
  return QString::fromStdString(data[nsetKey[iset]]["files"][iimg]);
}

// =================================================================================================

std::vector<std::string> MainWindow::readFiles(size_t iset)
{
  // local list of files
  std::vector<std::string> files;

  // copy items currently present in the list
  for ( size_t i=0; i<data[nsetKey[iset]]["files"].size(); ++i )
    files.push_back(data[nsetKey[iset]]["files"][i]);

  // output
  return files;
}

// =================================================================================================

void MainWindow::setFiles(size_t iset, std::vector<std::string> files)
{
  data[nsetKey[iset]].erase("files");
  data[nsetKey[iset]]["files"] = files;
}

// =================================================================================================

void MainWindow::fileAdd(size_t set)
{
  // set filters to be used
  QStringList filters;
  filters << "Image files (*.png *.xpm *.jpg *.jpeg *.tif *.tiff *.bmp)"
          << "Any files (*)";

  // set-up read dialog
  QFileDialog dialog(this);
  dialog.setFileMode   (QFileDialog::ExistingFiles);
  dialog.setOption     (QFileDialog::HideNameFilterDetails,false);
  dialog.setDirectory  (out_path);
  dialog.setNameFilters(filters);
  dialog.setViewMode   (QFileDialog::List);

  // read list of files using dialog
  QStringList fileNames;
  if (dialog.exec())
    fileNames = dialog.selectedFiles();
  else
    return;

  // read current file selection
  std::vector<std::string> files = readFiles(set);

  // add selected items to the end of the list, but only if they are new
  for ( QString &file : fileNames ) {
    // - get the relative name of the selected file
    std::string fname = out_path.relativeFilePath(file).toStdString();
    // - check to add and add
    if ( ! ( std::find(files.begin(),files.end(),fname)!=files.end() ) )
      files.push_back(fname);
  }

  // store in data-structure
  setFiles(set,files);
}

// =================================================================================================

void MainWindow::fileRmv(size_t set)
{
  // read current file selection
  std::vector<std::string> files = readFiles(set);

  // initialize list that keep track which items have to be removed
  std::vector<int> rm(files.size(),0);

  // mark all select items for removal
  foreach ( QListWidgetItem *item, fileLst[set]->selectedItems() )
    rm[fileLst[set]->row(item)] = 1;

  // remove select items
  size_t j = 0;
  for ( size_t i=0; i<rm.size(); ++i ) {
    if ( rm[i] ) {
      files.erase(files.begin()+i-j);
      ++j;
    }
  }

  // store in data-structure
  setFiles(set,files);
}

// =================================================================================================

void MainWindow::fileUp(size_t set)
{
  // get selected items
  QList<QListWidgetItem*> items = fileLst[set]->selectedItems();

  // list with row-number of select items
  // - allocate
  std::vector<int> row(items.size());
  // - fill
  for ( int i=0; i<items.size(); ++i )
    row[i] = fileLst[set]->row(items[i]);

  // any items at the top of the list -> do nothing (quit function)
  for ( int i=0; i<items.size(); ++i )
    if ( row[i]<=0 )
      return;

  // sort row numbers, to ensure correct switching of items
  std::sort(row.begin(),row.end());

  // read current file list
  std::vector<std::string> files = readFiles(set);
  std::string              fname;

  // move selected rows one previous (up in the widget)
  for ( auto &i : row ) {
    fname      = files[i];
    files[i  ] = files[i-1];
    files[i-1] = fname;
  }

  // store in data-structure
  setFiles(set,files);

  // restore file selection
  // - clear entire selection
  for ( int j=0; j<fileLst[set]->count(); ++j )
    fileLst[set]->item(j)->setSelected(false);
  // - apply previous selection, moved one up
  for ( int j=0; j<items.size(); ++j )
    fileLst[set]->item(row[j]-1)->setSelected(true);
}

// =================================================================================================

void MainWindow::fileDwn(size_t set)
{
  // get selected items
  QList<QListWidgetItem*> items = fileLst[set]->selectedItems();

  // list with row-number of select items
  // - allocate
  std::vector<int> row(items.size());
  // - fill
  for ( int i=0 ; i<items.size() ; i++ )
    row[i] = fileLst[set]->row(items[i]);

  // any items at the bottom of the list -> do nothing (quit function)
  for ( int i=0 ; i<items.size() ; i++ )
    if ( row[i]>=fileLst[set]->count()-1 )
      return;

  // sort row numbers in descending order, to ensure correct switching of items
  std::sort(row.begin(),row.end(), [](int a, int b) { return a>b; });

  // read current file list
  std::vector<std::string> files = readFiles(set);
  std::string              fname;

  // move selected rows one forward (down in the widget)
  for ( auto &i : row ) {
    fname      = files[i+1];
    files[i+1] = files[i  ];
    files[i  ] = fname;
  }

  // store in data-structure
  setFiles(set,files);

  // restore file selection
  // - clear entire selection
  for ( int j=0; j<fileLst[set]->count(); ++j )
    fileLst[set]->item(j)->setSelected(false);
  // - apply previous selection, moved one up
  for ( int j=0; j<items.size(); ++j )
    fileLst[set]->item(row[j]+1)->setSelected(true);
}

// =================================================================================================

void MainWindow::fileSrt(size_t set)
{
  // read current file list
  std::vector<std::string> files = readFiles(set);

  // sort by name
  std::sort(files.begin(),files.end());

  // store in data-structure
  setFiles(set,files);

  // clear file selection
  for ( int j=0; j<fileLst[set]->count(); ++j )
    fileLst[set]->item(j)->setSelected(false);
}

// =================================================================================================

void MainWindow::on_pushButtonT2_cp_clicked()
{
  setFiles(1,readFiles(0));
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
  for ( size_t set=0; set<data["nset"]; ++set ) {
    // - set labels
    try {
      propLbl[set]->setText(QString::fromStdString(data[nsetKey[set]]["field"]));
      typeLbl[set]->setText(QString::fromStdString(data[nsetKey[set]]["dtype"]));
    }
    catch (...) { }
    // - enable file list
    fileLst[set]->setEnabled(true);
    // - store selected items
    //   - read selected files
    QList<QListWidgetItem*> items = fileLst[set]->selectedItems();
    //   - allocate list with selected rows
    std::vector<int> row(items.size());
    //   - file list with selected rows
    for ( int j=0; j<items.size(); ++j )
      row[j] = fileLst[set]->row(items[j]);
    // - empty file lists
    while ( fileLst[set]->count()>0 )
      fileLst[set]->takeItem(0);
    // - read files
    std::vector<std::string> files = readFiles(set);
    // - fill file list
    for ( auto &file : files )
      fileLst[set]->addItem(QString::fromStdString(file));
    // - restore file selection
    for ( int j=0; j<fileLst[set]->count(); ++j )
      fileLst[set]->item(j)->setSelected(false);
    for ( int j=0; j<items.size(); ++j )
      fileLst[set]->item(row[j])->setSelected(true);
    // - enable buttons
    if ( true                              ) fileBtnAdd[set]    ->setEnabled(true);
    if ( files.size()>0                    ) fileBtnRmv[set]    ->setEnabled(true);
    if ( files.size()>0                    ) fileBtnUp [set]    ->setEnabled(true);
    if ( files.size()>0                    ) fileBtnDwn[set]    ->setEnabled(true);
    if ( files.size()>0                    ) fileBtnSrt[set]    ->setEnabled(true);
    if ( files.size()>0 && data["nset"]==2 ) ui->pushButtonT2_cp->setEnabled(true);
  }
}

// =================================================================================================

void MainWindow::tab3_show()
{
  // initialization
  //---------------

  // store the current indices
  int iset = ui->comboBoxT3_set->currentIndex();
  int iimg = ui->comboBoxT3_img->currentIndex();

  // read number of sets
  int nset = static_cast<int>(data["nset"]);

  // clear both comboBoxes (enable below)
  ui->comboBoxT3_set->clear();
  ui->comboBoxT3_img->clear();
  ui->comboBoxT3_img->setEnabled(false);
  ui->comboBoxT3_set->setEnabled(false);

  // sets -> comboBox
  // ----------------

  // fill comboBox with set name / field / type
  if ( nset>0 ) {
    // - enable
    ui->comboBoxT3_set->setEnabled(true);
    // - set name format
    QString name = "set%1: %2 (%3)";
    // - add items
    for ( int i=0; i<nset; ++i ) {
      ui->comboBoxT3_set->addItem(name.arg(
        QString::number(i),
        QString::fromStdString(data[nsetKey[i]]["field"]),
        QString::fromStdString(data[nsetKey[i]]["dtype"])
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
  std::vector<std::string> files  = readFiles(static_cast<size_t>(iset));
  int                      nfiles = static_cast<int>(files.size());
  // add files to comboBox
  for ( auto &file : files )
    ui->comboBoxT3_img->addItem(QString::fromStdString(file));
  // enable comboBox and navigation buttons
  if ( files.size()>0                  ) ui->comboBoxT3_img      ->setEnabled(true);
  if ( files.size()>1 && iimg>0        ) ui->pushButtonT3_imgPrev->setEnabled(true);
  if ( files.size()>1 && iimg<nfiles-1 ) ui->pushButtonT3_imgNext->setEnabled(true);
  // reapply index
  ui->comboBoxT3_img->setCurrentIndex(iimg);

  // no image selected -> quit
  if ( iimg<0 )
    return;

  // extract keys
  // ------------

  std::string set      = nsetKey[iset];
  std::string fname    = data[set]["files"][iimg];
  std::string dtype    = data[set]["dtype"];
  bool        periodic = data["periodic"];

  // read image from file -> show
  // ----------------------------

  // read image (nothing done if "iimg" and "iset" are the same as the last time)
  bool read = image.read(readFilePath(iset,iimg),iset,iimg,
    ui->graphicsViewT3_img->width(),
    ui->graphicsViewT3_img->height(),
    ui->sliderT3_zoom->sliderPosition()
  );
  // optionally update view
  if ( read ) {
    // set maxima
    ui->spinBoxT3_rowHgh->setMaximum(image.nrow);
    ui->spinBoxT3_colHgh->setMaximum(image.ncol);
    // store as defaults
    if ( data[set]["config"][fname]["row"]["values"][1]==0 )
      data[set]["config"][fname]["row"]["values"][1] = image.nrow;
    if ( data[set]["config"][fname]["col"]["values"][1]==0 )
      data[set]["config"][fname]["col"]["values"][1] = image.ncol;
    // create a "scene" with containing the image
    QGraphicsPixmapItem *item  = new QGraphicsPixmapItem(QPixmap::fromImage(image.dataQt));
    QGraphicsScene      *scene = new QGraphicsScene;
    scene->addItem(item);
    ui->graphicsViewT3_img->setScene(scene);
    // show the image with the correct scaling applied
    ui->graphicsViewT3_img->setTransform(QTransform::fromScale(image.scale,image.scale));
    ui->graphicsViewT3_img->show();
  }

  // update interpretation settings (spinBox)
  // ----------------------------------------

  // default values to show (only zeros)
  std::vector<int> val(imgVal[imgVal.size()-1],0);
  // alias
  json config = data[set]["config"][fname];
  // loop over fields
  for ( size_t i=0; i<imgCheckLbl.size(); ++i ) {
    // - get field name
    std::string key = imgCheckLbl[i];
    // - get number of entries
    size_t n = std::min(imgVal[i+1]-imgVal[i],config[key]["values"].size());
    // - read entries
    if ( n>0 )
      for ( size_t j=0; j<n; ++j )
        val[imgVal[i]+j] = config[key]["values"][j];
    // - set checkBox
    imgCheck[i]->setChecked(config[key]["active"]);
  }
  // store values
  for ( size_t i=0; i<val.size(); ++i ) imgSpin[i]->setValue(val[i]);

  // interpret image based on settings
  // ---------------------------------

  // get image and mask
  mat::matrix<int> im,mask;
  std::tie(im,mask) = image.config(config,periodic,dtype,254);
  double fac        = 255./static_cast<double>(im.max());

  mat::matrix<unsigned char> imgview(im.shape());

  // float: max -> 254 (to make room for mask)
  if ( dtype=="float" )
    for ( size_t i=0 ; i<im.size() ; i++ )
      imgview[i] = std::min(im[i],253);
  // binary: 1 -> 254
  if ( dtype=="binary" )
    for ( size_t i=0 ; i<im.size() ; i++ )
      imgview[i] = im[i]*253;
  // int: stretch scale
  if ( dtype=="int" )
    for ( size_t i=0 ; i<im.size() ; i++ )
      imgview[i] = std::min(static_cast<int>(static_cast<double>(im[i])*fac),253);
  // mask -> 255
  for ( size_t i=0 ; i<im.size() ; i++ )
    if ( mask[i] )
      imgview[i] = 255;

  // mask weights
  // ------------

  if ( data["mask_weight"] && data[set]["field"]=="phase" && iset==1 ) {
    QtImage image_;
    mat::matrix<int> im_,mask_;
    std::string key_    = nsetKey[0];
    std::string fname_  = data[key_]["files"][iimg];
    std::string dtype_  = data[key_]["dtype"];
    json        config_ = data[key_]["config"][fname_];
    std::tie(im_,mask_) = image_.config(config,periodic,dtype_,0);
    image_.read(readFilePath(0,iimg));
    image_.config(config_,periodic,dtype_,0);
    for ( size_t i=0 ; i<im_.size() ; i++ )
      if ( im_[i] )
        imgview[i] = 255;
  }

  // view interpreted image
  // ----------------------

  // read colormap
  std::vector<int> cols;
  cols = cppcolormap::colormap(ui->comboBoxT3_cmap->currentText().toStdString(),256);
  // excluded pixes -> white
  for ( size_t i=0 ; i<3 ; i++ )
    cols[254*3+i] = 255;
  // optionally set white background
  if ( data[nsetKey[iset]]["dtype"]=="int" )
    for ( size_t i=0 ; i<3 ; i++ )
      cols[0*3+i] = 255;
  // set mask color
  std::vector<int> tmp;
  tmp = cppcolormap::colormap(ui->comboBoxT3_cmapMask->currentText().toStdString(),1);
  for ( size_t i=0 ; i<3 ; i++ )
    cols[255*3+i] = tmp[i];
  // convert to Qt format
  QVector<QRgb> cmap;
  for ( size_t i = 0; i < 256; ++i)
    cmap.append(qRgb(cols[i*3+0],cols[i*3+1],cols[i*3+2]));
  // convert Image::Matrix -> image to view
  QImage imQt(
    imgview.data(),
    imgview.shape()[1],
    imgview.shape()[0],
    QImage::QImage::Format_Indexed8
  );
  imQt.setColorTable(cmap);
  // create a "scene" with containing the image
  QGraphicsPixmapItem *item  = new QGraphicsPixmapItem(QPixmap::fromImage(imQt));
  QGraphicsScene      *scene = new QGraphicsScene;
  scene->addItem(item);
  ui->graphicsViewT3_seg->setScene(scene);
  // show the image with the correct scaling applied
  ui->graphicsViewT3_seg->setTransform(QTransform::fromScale(image.scale,image.scale));
  ui->graphicsViewT3_seg->show();
}

// =================================================================================================

void MainWindow::tab3_read()
{
  int iset = ui->comboBoxT3_set->currentIndex();
  int iimg = ui->comboBoxT3_img->currentIndex();

  if ( iset<0 || iimg<0 )
    return;

  std::string set   = nsetKey[iset];
  std::string fname = data[set]["files"][iimg];

  for ( size_t i=0; i<imgCheckLbl.size(); ++i ) {
    data[set]["config"][fname][imgCheckLbl[i]]["active"] = imgCheck[i]->isChecked();
    size_t k=0;
    for ( size_t j=imgVal[i]; j<imgVal[i+1]; ++j ) {
      data[set]["config"][fname][imgCheckLbl[i]]["values"][k] = imgSpin[j]->value();
      ++k;
    }
  }

  // std::vector<int> range(2);

  // range[0] = imgSpin[idx+0]->value();
  // range[1] = imgSpin[idx+1]->value();

  // if ( imgCheck[idx/2]->isChecked() ) {
  //   data[set]["config"][fname][imgCheckLbl[idx/2]] = range;
  //   return;
  // }

  // if ( idx==0 || idx>=8 ) {
  //   try { data[set]["config"][fname].erase(imgCheckLbl[idx/2]); }
  //   catch (...) {};
  //   return;
  // }

  // std::vector<int> mask;

  // for ( size_t i=1; i<4; i++ ) {
  //   if ( imgCheck[i]->isChecked() ) {
  //     mask.push_back(imgSpin[i*2+0]->value());
  //     mask.push_back(imgSpin[i*2+1]->value());
  //   }
  // }

  // if ( mask.size()>0 ) {
  //   data[set]["config"][fname][imgCheckLbl[idx/2]] = mask;
  //   return;
  // }

  // try { data[set]["config"][fname].erase(imgCheckLbl[idx/2]); }
  // catch (...) {};
}
