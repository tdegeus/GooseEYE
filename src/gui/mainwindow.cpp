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
  // - output images -> default assigned in "on_pushButtonT0_path_clicked"
  data["output"]["result"] = "";
  data["output"]["interp"] = "";
  // - statistics settings
  data["stat"       ] = ""         ;
  data["nset"       ] = 1          ;
  data["periodic"   ] = true       ;
  data["zeropad"    ] = false      ;
  data["mask_weight"] = true       ;
  data["pixel_path" ] = "Bresenham";
  data["roi"        ] = {51,51}    ;
  // - empty file sets
  data["set0"]["field" ] = "";
  data["set0"]["dtype" ] = "";
  data["set0"]["files" ] = {};
  data["set0"]["config"] = {};
  data["set1"]["field" ] = "";
  data["set1"]["dtype" ] = "";
  data["set1"]["files" ] = {};
  data["set1"]["config"] = {};

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
  QComboBox *im  = ui->tab3_im_comboBox;
  QComboBox *set = ui->tab3_set_comboBox;
  // - previous image
  connect(ui->pushButtonT3_imgPrev ,&QPushButton::clicked,[=](){
    if (im ->currentIndex()>0             ) im ->setCurrentIndex(im ->currentIndex()-1);});
  // - next image
  connect(ui->pushButtonT3_imgNext ,&QPushButton::clicked,[=](){
    if (im ->currentIndex()<im ->count()-1) im ->setCurrentIndex(im ->currentIndex()+1);});
  // - previous iset
  connect(ui->pushButtonT3_setPrev,&QPushButton::clicked,[=](){
    if (set->currentIndex()>0             ) set->setCurrentIndex(set->currentIndex()-1);});
  // - next set
  connect(ui->pushButtonT3_setNext,&QPushButton::clicked,[=](){
    if (set->currentIndex()<set->count()-1) set->setCurrentIndex(set->currentIndex()+1);});

  // combine buttons to list, for fast references
  // - tab1: radioButton group to select statistic, and their field-names
  statBtn.push_back(ui->stat_S2_radioButton   ); statKey.push_back("S2"    );
  statBtn.push_back(ui->stat_C2_radioButton   ); statKey.push_back("C2"    );
  statBtn.push_back(ui->stat_L_radioButton    ); statKey.push_back("L"     );
  statBtn.push_back(ui->stat_W2_radioButton   ); statKey.push_back("W2"    );
  statBtn.push_back(ui->stat_W2c_radioButton  ); statKey.push_back("W2c"   );
  // - tab1: checkBox group to select number of sets, and their field-names
  nsetBtn.push_back(ui->tab1_im0_checkBox     ); nsetKey.push_back("set0"  );
  nsetBtn.push_back(ui->checkBoxT1_img1     ); nsetKey.push_back("set1"  );
  // - tab1: radioButton group to select data-type (one for each set), and their field-names
  typeBtn.push_back(ui->tab1_im0b_radioButton ); typeKey.push_back("binary");
  typeBtn.push_back(ui->tab1_im0i_radioButton ); typeKey.push_back("int"   );
  typeBtn.push_back(ui->tab1_im0f_radioButton ); typeKey.push_back("float" );
  typeBtn.push_back(ui->tab1_im1b_radioButton ); typeKey.push_back("binary");
  typeBtn.push_back(ui->tab1_im1i_radioButton ); typeKey.push_back("int"   );
  typeBtn.push_back(ui->tab1_im1f_radioButton ); typeKey.push_back("float" );
  // - tab2: pushButton group of all buttons on this tab
  fileBtn.push_back(ui->tab2_im0Add_pushButton); fileBtnAdd.push_back(ui->tab2_im0Add_pushButton);
  fileBtn.push_back(ui->tab2_im1Add_pushButton); fileBtnAdd.push_back(ui->tab2_im1Add_pushButton);
  fileBtn.push_back(ui->tab2_im0Rmv_pushButton); fileBtnRmv.push_back(ui->tab2_im0Rmv_pushButton);
  fileBtn.push_back(ui->tab2_im1Rmv_pushButton); fileBtnRmv.push_back(ui->tab2_im1Rmv_pushButton);
  fileBtn.push_back(ui->tab2_im0Up__pushButton); fileBtnUp .push_back(ui->tab2_im0Up__pushButton);
  fileBtn.push_back(ui->tab2_im1Up__pushButton); fileBtnUp .push_back(ui->tab2_im1Up__pushButton);
  fileBtn.push_back(ui->tab2_im0Dwn_pushButton); fileBtnDwn.push_back(ui->tab2_im0Dwn_pushButton);
  fileBtn.push_back(ui->tab2_im1Dwn_pushButton); fileBtnDwn.push_back(ui->tab2_im1Dwn_pushButton);
  fileBtn.push_back(ui->tab2_im0Srt_pushButton); fileBtnSrt.push_back(ui->tab2_im0Srt_pushButton);
  fileBtn.push_back(ui->tab2_im1Srt_pushButton); fileBtnSrt.push_back(ui->tab2_im1Srt_pushButton);
  fileBtn.push_back(ui->tab2_cp_pushButton    );
  // - tab2: listWidget group of all file-lists
  fileLst.push_back(ui->tab2_im0_listWidget   );
  fileLst.push_back(ui->tab2_im1_listWidget   );
  // - tab2: label group of all field labels ("phase","weight")
  propLbl.push_back(ui->tab2_im0Phase_label   );
  propLbl.push_back(ui->tab2_im1Phase_label   );
  // - tab2: label group of data-type labels ("binary","int","float")
  typeLbl.push_back(ui->tab2_im0Dtype_label   );
  typeLbl.push_back(ui->tab2_im1Dtype_label   );
  // - tab3: spinBox group to interpret the image
  imgSpin.push_back(ui->tab3_phaseLow_spinBox ); imgSpin.push_back(ui->tab3_phaseHgh_spinBox);
  imgSpin.push_back(ui->tab3_mask1Low_spinBox ); imgSpin.push_back(ui->tab3_mask1Hgh_spinBox);
  imgSpin.push_back(ui->tab3_mask2Low_spinBox ); imgSpin.push_back(ui->tab3_mask2Hgh_spinBox);
  imgSpin.push_back(ui->tab3_mask3Low_spinBox ); imgSpin.push_back(ui->tab3_mask3Hgh_spinBox);
  imgSpin.push_back(ui->tab3_rowLow_spinBox   ); imgSpin.push_back(ui->tab3_rowHgh_spinBox  );
  imgSpin.push_back(ui->tab3_colLow_spinBox   ); imgSpin.push_back(ui->tab3_colHgh_spinBox  );
  // - tab3: checkBox to select a modification field, and their field-names
  imgCheck.push_back(ui->tab3_phase_checkBox  ); imgCheckLbl.push_back("phase");
  imgCheck.push_back(ui->tab3_mask1_checkBox  ); imgCheckLbl.push_back("mask" );
  imgCheck.push_back(ui->tab3_mask2_checkBox  ); imgCheckLbl.push_back("mask" );
  imgCheck.push_back(ui->tab3_mask3_checkBox  ); imgCheckLbl.push_back("mask" );
  imgCheck.push_back(ui->tab3_row_checkBox    ); imgCheckLbl.push_back("row"  );
  imgCheck.push_back(ui->tab3_col_checkBox    ); imgCheckLbl.push_back("col"  );
  // - tab3: pushButton to navigate through images
  imgBtn.push_back(ui->pushButtonT3_setPrev); imgBtn.push_back(ui->pushButtonT3_setNext);
  imgBtn.push_back(ui->pushButtonT3_imgPrev ); imgBtn.push_back(ui->pushButtonT3_imgNext );
  imgBtn.push_back(ui->tab3_imEmpty_pushButton);
  imgBtn.push_back(ui->tab3_imApply_pushButton);
  imgBtn.push_back(ui->tab3_zoomIn__pushButton);
  imgBtn.push_back(ui->tab3_zoomOut_pushButton);

  btnGroup.push_back(ui->buttonGroup          );
  btnGroup.push_back(ui->buttonGroup_tab1_im0 );
  btnGroup.push_back(ui->buttonGroup_tab1_im1 );

  // refresh tabs when tab is changed
  connect(ui->tabWidget,&QTabWidget::currentChanged,[=](){tab0_show();});
  connect(ui->tabWidget,&QTabWidget::currentChanged,[=](){tab1_show();});
  connect(ui->tabWidget,&QTabWidget::currentChanged,[=](){tab2_show();});
  connect(ui->tabWidget,&QTabWidget::currentChanged,[=](){tab3_show();});

  // refresh file related views when JSON is loaded or "out_path" is changed
  connect(ui->pushButtonT0_path,&QPushButton::clicked,this,[=](){tab0_show();});
  connect(ui->pushButtonT0_path,&QPushButton::clicked,this,[=](){tab2_show();});
  connect(ui->pushButtonT0_load    ,&QPushButton::clicked,this,[=](){tab0_show();});
  connect(ui->pushButtonT0_load    ,&QPushButton::clicked,this,[=](){tab1_show();});
  connect(ui->pushButtonT0_load    ,&QPushButton::clicked,this,[=](){tab2_show();});

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

  size_t j;
  for ( j=0; j<imgSpin.size(); ++j ) connect(imgSpin[j],&QSpinBox::editingFinished,this,[=](){tab3_read(j);});
  for ( j=1; j<imgSpin.size(); ++j ) connect(imgSpin[j],&QSpinBox::editingFinished,this,[=](){tab3_read(j);});

  // tab2: button pressed -> refresh view with new "data"
  for ( auto &i : statBtn ) connect(i,&QPushButton::clicked     ,this,[=](){tab1_show();});
  for ( auto &i : typeBtn ) connect(i,&QPushButton::clicked     ,this,[=](){tab1_show();});
  for ( auto &i : nsetBtn ) connect(i,&QPushButton::clicked     ,this,[=](){tab1_show();});
  for ( auto &i : fileBtn ) connect(i,&QPushButton::clicked     ,this,[=](){tab2_show();});
  for ( auto &i : imgBtn  ) connect(i,&QPushButton::clicked     ,this,[=](){tab3_show();});
  for ( auto &i : imgSpin  ) connect(i,&QSpinBox::editingFinished,this,[=](){tab3_show();});
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

void MainWindow::on_tab0_res1path_lineEdit_textEdited(const QString &arg1)
{
  data["output"]["result"] = arg1.toStdString();
}

// =================================================================================================

void MainWindow::on_tab0_res2path_lineEdit_textEdited(const QString &arg1)
{
  data["output"]["interp"] = arg1.toStdString();
}

// =================================================================================================

void MainWindow::tab0_show()
{
  ui->lineEditT0_path->setText(out_path.absolutePath());
  ui->lineEditT0_json->setText(out_json);
  ui->tab0_res1path_lineEdit->setText(QString::fromStdString(data["output"]["result"]));
  ui->tab0_res2path_lineEdit->setText(QString::fromStdString(data["output"]["interp"]));
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
  data["periodic"   ] = ui->tab1_periodic_checkBox ->isChecked();
  data["zeropad"    ] = ui->tab1_zeropad_checkBox  ->isChecked();
  data["mask_weight"] = ui->tab1_maskW_checkBox    ->isChecked();
  data["pixel_path" ] = ui->tab1_pixelpath_comboBox->currentText().toStdString();
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
  ui->tab1_periodic_checkBox   ->setEnabled(true );
  ui->tab1_zeropad_checkBox    ->setEnabled(true );
  ui->tab1_maskW_checkBox      ->setEnabled(false);
  ui->tab1_pixelpath_label     ->setEnabled(false);
  ui->tab1_pixelpath_comboBox  ->setEnabled(false);
  // - statistic specific: mask weights
  if ( data["stat"]=="W2" || data["stat"]=="W2c" )
    ui->tab1_maskW_checkBox    ->setEnabled(true);
  // - statistic specific: pixel path
  if ( data["stat"]=="L" || data["stat"]=="W2c" ) {
    ui->tab1_zeropad_checkBox  ->setEnabled(false);
    ui->tab1_pixelpath_label   ->setEnabled(true );
    ui->tab1_pixelpath_comboBox->setEnabled(true );
  }

  // apply settings
  QString pth = QString::fromStdString(data["pixel_path"]);
  ui->tab1_pixelpath_comboBox  ->setCurrentIndex(ui->tab1_pixelpath_comboBox->findText(pth));
  ui->tab1_periodic_checkBox   ->setChecked(data["periodic"   ]);
  ui->tab1_zeropad_checkBox    ->setChecked(data["zeropad"    ]);
  ui->tab1_maskW_checkBox      ->setChecked(data["mask_weight"]);
}

// =================================================================================================



// =================================================================================================

QString MainWindow::readFile(size_t iset, size_t iimg)
{
  QString out = "";

  if ( data.count(nsetKey[iset]) )
    if ( data[nsetKey[iset]].count("files") )
      out = QString::fromStdString(data[nsetKey[iset]]["files"][iimg]);

  return out;
}

// =================================================================================================

std::vector<std::string> MainWindow::readFiles(size_t iset)
{
  // local list of files
  std::vector<std::string> files;

  // copy items currently present in the list
  if ( data.count(nsetKey[iset]) )
    if ( data[nsetKey[iset]].count("files") )
      for ( size_t i=0; i<data[nsetKey[iset]]["files"].size(); ++i )
        files.push_back(data[nsetKey[iset]]["files"][i]);


  // output
  return files;
}

// =================================================================================================

void MainWindow::setFiles(size_t iset, std::vector<std::string> files)
{
  if ( data.count(nsetKey[iset]) )
    if ( data[nsetKey[iset]].count("files") )
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

void MainWindow::on_tab2_cp_pushButton_clicked()
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
    if ( true                              ) fileBtnAdd[set]       ->setEnabled(true);
    if ( files.size()>0                    ) fileBtnRmv[set]       ->setEnabled(true);
    if ( files.size()>0                    ) fileBtnUp [set]       ->setEnabled(true);
    if ( files.size()>0                    ) fileBtnDwn[set]       ->setEnabled(true);
    if ( files.size()>0                    ) fileBtnSrt[set]       ->setEnabled(true);
    if ( files.size()>0 && data["nset"]==2 ) ui->tab2_cp_pushButton->setEnabled(true);
  }
}

// =================================================================================================

std::vector<size_t> MainWindow::imageSettings(size_t iset, size_t iimg)
{
  // zero initialize output (phase,mask1,mask2,mask3,row,col) as a single vector (min,max,...)
  std::vector<size_t> out(12,0);
  // read keys
  std::string key   = nsetKey[iset];
  std::string fname = data[key]["files"][iimg];

  std::vector<size_t> phase = {0,255};
  std::vector<size_t> row   = {0,imgRaw_.shape()[0]};
  std::vector<size_t> col   = {0,imgRaw_.shape()[1]};
  // imgRaw_

  // no information specified -> quit
  if ( !data[key].count("config") )
    data[key]["config"][fname]["phase"] = phase;
    data[key]["config"][fname]["row"  ] = row;
    data[key]["config"][fname]["col"  ] = col;
  // no information specified -> quit
  if ( !data[key]["config"].count(fname) ) {
    data[key]["config"][fname]["phase"] = phase;
    data[key]["config"][fname]["row"  ] = row;
    data[key]["config"][fname]["col"  ] = col;
  }

  // read from "data"
  // - phase: lower and upper bound
  if ( data[key]["config"][fname].count("phase") )
    for ( size_t i=0; i<data[key]["config"][fname]["phase"].size(); ++i )
      out[0*2+i] = data[key]["config"][fname]["phase"][i];
  // - mask1,mask2,mask3: lower and upper bound
  if ( data[key]["config"][fname].count("mask") )
    for ( size_t i=0; i<data[key]["config"][fname]["mask"].size(); ++i )
      out[1*2+i] = data[key]["config"][fname]["mask"][i];
  // - rows: lower and upper bound
  if ( data[key]["config"][fname].count("row") )
    for ( size_t i=0; i<data[key]["config"][fname]["row"].size(); ++i )
      out[4*2+i] = data[key]["config"][fname]["row"][i];
  // - columns: lower and upper bound
  if ( data[key]["config"][fname].count("col") )
    for ( size_t i=0; i<data[key]["config"][fname]["col"].size(); ++i )
      out[5*2+i] = data[key]["config"][fname]["col"][i];

  return out;
}

// =================================================================================================

double MainWindow::tab3_scaleImage()
{
  double wdthView  = (double)ui->graphicsViewT3_img->width ();
  double hghtView  = (double)ui->graphicsViewT3_img->height();
  double wdthImage = (double)ui->tab3_colHgh_spinBox->maximum();
  double hghtImage = (double)ui->tab3_rowHgh_spinBox->maximum();
  double zoomScale = pow(1.066,(double)ui->tab3_zoom_slider->sliderPosition());
  double wdth      = zoomScale*wdthView/wdthImage;
  double hght      = zoomScale*hghtView/hghtImage;

  return 0.95*std::min(wdth,hght);
}

// =================================================================================================

std::tuple<mat::matrix<int>,QImage> MainWindow::readImage(size_t iset, size_t iimg)
{
  // load image
  QImage img;
  img.load(out_path.filePath(readFile(iset,iimg)));
  // read the size
  size_t nrow = img.height();
  size_t ncol = img.width ();
  // allocate data
  mat::matrix<int> out({nrow,ncol});
  // read image
  for ( size_t i=0; i<nrow; ++i )
    for ( size_t j=0; j<ncol; ++j )
      out(i,j) = qGray(img.pixel(j,i));

  return std::make_tuple(out,img);
}

// =================================================================================================

std::tuple<mat::matrix<int>,mat::matrix<int>> MainWindow::interpretImage(
  size_t iset, size_t iimg, mat::matrix<int> im, int crop)
{
  // read keys
  std::string key      = nsetKey[iset];
  bool        periodic = data["periodic"];
  std::string fname    = data[key]["files" ][iimg ];
  json        file     = data[key]["config"][fname];
  std::string dtype    = data[key]["dtype" ];

  // default shape of output matrix
  size_t irow = 0;
  size_t jrow = im.shape()[0];
  size_t icol = 0;
  size_t jcol = im.shape()[1];

  // default range of the output matrix
  int min  = 0;
  int max  = 255;

  // crop: modify dimensions of the output matrix
  if ( crop==-1 && file.count("row") ) {
    irow = file["row"][0];
    jrow = file["row"][1];
  }
  if ( crop==-1 && file.count("col") ) {
    icol = file["col"][0];
    jcol = file["col"][1];
  }

  // threshold: read range
  if ( file.count("phase") ) {
    min  = static_cast<int>(file["phase"][0]);
    max  = static_cast<int>(file["phase"][1]);
  }

  // convert "crop" such that it can be used as pre-factor
  if ( crop==-1 )
    crop = 0;
  // allocate output
  mat::matrix<int> out({jrow-irow,jcol-icol}); out.ones (); out *= crop;
  mat::matrix<int> msk({jrow-irow,jcol-icol}); msk.zeros();

  // phase threshold float: retain values; everything outside bounds is masked
  if ( dtype=="float" ) {
    for ( size_t i=0; i<(jrow-irow); ++i ) {
      for ( size_t j=0; j<(jcol-icol); ++j ) {
        if ( im(i+irow,j+icol)>=min && im(i+irow,j+icol)<=max )
          out(i,j) = im(i+irow,j+icol);
        else
          msk(i,j) = 1;
      }
    }
  }
  // phase threshold binary/int: between (min,max)->1, else->0 (nothing masked)
  else {
    for ( size_t i=0; i<(jrow-irow); ++i )
      for ( size_t j=0; j<(jcol-icol); ++j )
        if ( im(i+irow,j+icol)>=min && im(i+irow,j+icol)<=max )
          out(i,j) = 1;
  }

  // int: determine clusters from binary image
  if ( dtype=="int" ) {
    mat::matrix<int> clusters,centers;
    std::tie(clusters,centers) = Image::clusters(out,periodic);
    for ( size_t i=0; i<out.size(); ++i )
      out[i] = clusters[i];
  }

  // mask threshold
  if ( file.count("mask") ) {
    if ( file["mask"].size()%2!=0 )
      throw std::runtime_error("masks must be specified min,max , min,max , ...");
    for ( size_t imsk=0; imsk<file["mask"].size(); imsk+=2 )
      for ( size_t i=0 ; i<(jrow-irow) ; i++ )
        for ( size_t j=0 ; j<(jcol-icol) ; j++ )
          if ( im(i+irow,j+icol)>=file["mask"][imsk] && im(i+irow,j+icol)<=file["mask"][imsk+1] )
            msk(i,j) = 1;
  }

  return std::make_tuple(out,msk);
};

// =================================================================================================

std::tuple<mat::matrix<int>,mat::matrix<int>> MainWindow::interpretImage(
  size_t iset, size_t iimg, int crop)
{
  QImage imQt;
  mat::matrix<int> im;

  std::tie(im,imQt) = readImage(iset,iimg);

  return interpretImage(iset,iimg,im,crop);
}

// =================================================================================================

void MainWindow::tab3_show()
{
  // initialization
  //---------------

  // store the current indices
  int iset = ui->tab3_set_comboBox->currentIndex();
  int iimg = ui->tab3_im_comboBox ->currentIndex();

  // clear both comboBoxes (enable below)
  ui->tab3_set_comboBox->clear();
  ui->tab3_im_comboBox ->clear();
  ui->tab3_im_comboBox ->setEnabled(false);
  ui->tab3_set_comboBox->setEnabled(false);

  // sets -> comboBox
  // ----------------

  // fill comboBox with set name / field / type
  if ( data["nset"]>0 ) {
    // - enable
    ui->tab3_set_comboBox->setEnabled(true);
    // - set name format
    QString name = "set%1: %2 (%3)";
    // - add items
    for ( int i=0; i<static_cast<int>(data["nset"]); ++i ) {
      ui->tab3_set_comboBox->addItem(name.arg(
        QString::number(i),
        QString::fromStdString(data[nsetKey[i]]["field"]),
        QString::fromStdString(data[nsetKey[i]]["dtype"])
      ));
    }
    // - enable navigation buttons
    if ( data["nset"]>1 ) ui->pushButtonT3_setPrev->setEnabled(true);
    if ( data["nset"]>1 ) ui->pushButtonT3_setNext->setEnabled(true);
    // - select the first set
    if ( iset<0 )
      iset = 0;
    // - (re)apply index
    ui->tab3_set_comboBox->setCurrentIndex(iset);
  }

  // no set selected -> quit
  if ( iset<0 )
    return;

  // images -> comboBox
  // ------------------

  // read files in current set
  std::vector<std::string> files = readFiles(static_cast<size_t>(iset));
  // add files to comboBox
  for ( auto &file : files )
    ui->tab3_im_comboBox->addItem(QString::fromStdString(file));
  // enable comboBox and navigation buttons
  if ( files.size()>0 ) ui->tab3_im_comboBox      ->setEnabled(true);
  if ( files.size()>1 ) ui->pushButtonT3_imgPrev->setEnabled(true);
  if ( files.size()>1 ) ui->pushButtonT3_imgNext->setEnabled(true);
  // reapply index
  ui->tab3_im_comboBox->setCurrentIndex(iimg);

  // no image selected -> quit
  if ( iimg<0 )
    return;



  // read image from file and show
  // -----------------------------

  // read image, if the image is different than last time
  if ( iimg!=iimg_ || iset!=iset_ ) {
    // update "previous index"
    iimg_ = iimg;
    iset_ = iset;
    // load image
    QImage imQt;
    std::tie(imgRaw_,imQt) = readImage(iset,iimg);
    // set maxima
    ui->tab3_rowHgh_spinBox->setMaximum(imgRaw_.shape()[0]);
    ui->tab3_colHgh_spinBox->setMaximum(imgRaw_.shape()[1]);
    // load image

    // create a "scene" with containing the image
    QGraphicsPixmapItem *item  = new QGraphicsPixmapItem(QPixmap::fromImage(imQt));
    QGraphicsScene      *scene = new QGraphicsScene;
    scene->addItem(item);
    ui->graphicsViewT3_img->setScene(scene);
    // show the image with the correct scaling applied
    double scale = this->tab3_scaleImage();
    ui->graphicsViewT3_img->setTransform(QTransform::fromScale(scale,scale));
    ui->graphicsViewT3_img->show();
  }

  // read settings
  std::vector<size_t> values = imageSettings(iset,iimg);
  // apply all image settings
  for ( size_t i=0; i<values.size(); ++i )
    imgSpin[i]->setValue(values[i]);

  // interpret image based on settings
  // ---------------------------------

  // get image and mask
  mat::matrix<int> im,mask;
  std::tie(im,mask) = interpretImage(iset,iimg,imgRaw_,254);
  double fac        = 255./static_cast<double>(im.max());

  mat::matrix<unsigned char> imgview(im.shape());

  std::string key   = nsetKey[iset];
  std::string fname = data[key]["files"][iimg];
  std::string dtype = data[key]["dtype"];

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

  // if ( data["mask_weight"] && data[key]["filed"]=="phase" ) {
  //   if ( data_.saved(0,ui->tab3_im_comboBox->currentIndex()) ) {
  //     std::tie(im,mask) = data_.image(0,idx,imgRaw_,false);
  //     for ( size_t i=0 ; i<im.size() ; i++ )
  //       if ( im[i] )
  //         imgview[i] = 255;
  //   }
  // }

  // crop the image
  // --------------

  // size_t nrow = imgRaw_.shape()[0];
  // size_t ncol = imgRaw_.shape()[1];
  // size_t irow = data[key]["config"][fname][""];
  // size_t jrow = data[key]["config"][fname][""];
  // size_t icol = data[key]["config"][fname][""];
  // size_t jcol = data[key]["config"][fname][""];

  // if ( irow>0 || jrow<nrow || icol>0 || jcol<ncol )
  // for ( size_t i=0 ; i<nrow ; i++ )
  //   for ( size_t j=0 ; j<ncol ; j++ )
  //     if ( i<irow || i>jrow || j<icol || j>jcol )
  //       imgview(i,j) = 254;


  // read colormap
  std::vector<int> cols;
  cols = cppcolormap::colormap(ui->tab3_cmap_comboBox->currentText().toStdString(),256);
  // excluded pixes -> white
  for ( size_t i=0 ; i<3 ; i++ )
    cols[254*3+i] = 255;
  // optionally set white background
  if ( data[nsetKey[iset]]["dtype"]=="int" )
    for ( size_t i=0 ; i<3 ; i++ )
      cols[0*3+i] = 255;
  // set mask color
  std::vector<int> tmp;
  tmp = cppcolormap::colormap(ui->tab3_maskCol_comboBox->currentText().toStdString(),1);
  for ( size_t i=0 ; i<3 ; i++ )
    cols[255*3+i] = tmp[i];
  // convert to Qt format
  QVector<QRgb> cmap;
  for ( size_t i = 0; i < 256; ++i)
    cmap.append(qRgb(cols[i*3+0],cols[i*3+1],cols[i*3+2]));
  // convert Image::Matrix -> image to view
  QImage imQt(\
    imgview.data(),\
    imgview.shape()[1],\
    imgview.shape()[0],\
    QImage::QImage::Format_Indexed8\
  );
  imQt.setColorTable(cmap);
  // create a "scene" with containing the image
  QGraphicsPixmapItem *item  = new QGraphicsPixmapItem(QPixmap::fromImage(imQt));
  QGraphicsScene      *scene = new QGraphicsScene;
  scene->addItem(item);
  ui->graphicsViewT3_seg->setScene(scene);
  // show the image with the correct scaling applied
  double scale = this->tab3_scaleImage();
  ui->graphicsViewT3_seg->setTransform(QTransform::fromScale(scale,scale));
  ui->graphicsViewT3_seg->show();



}

// =================================================================================================

void MainWindow::tab3_read(size_t idx)
{
  int iset = ui->tab3_set_comboBox->currentIndex();
  int iimg = ui->tab3_im_comboBox ->currentIndex();

  if ( iset<0 || iimg<0 )
    return;

  if ( idx%2!=0 )
    --idx;

  std::string key   = nsetKey[iset];
  std::string fname = data[key]["files"][iimg];

  std::vector<int> range(2);

  range[0] = imgSpin[idx+0]->value();
  range[1] = imgSpin[idx+1]->value();

  if ( imgCheck[idx/2]->isChecked() )
    data[key]["config"][fname][imgCheckLbl[idx/2]] = range;
}
