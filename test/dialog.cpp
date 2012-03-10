#include <QtCore>
#include <QtGui>
#include "dialog.h"
#include "ui_dialog.h"

#define _TEST

Dialog::Dialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::Dialog)
{
  ui->setupUi(this);

  // create the widget and add it to this dialog layout
  m_qjsw = new QJsonTreeWidget(this);
  ui->verticalLayout_2->addWidget(m_qjsw);

  // to catch pushbutton clicks
  connect (m_qjsw,SIGNAL(clicked(const QJsonTreeItem*,QString)),this,SLOT(onTreeButtonClicked(const QJsonTreeItem*,QString)));

  // load configuration at startup
  QTimer::singleShot(0,this,SLOT(loadCfg()));
}

Dialog::~Dialog()
{
  delete m_qjsw;
  delete ui;
}

void Dialog::loadCfg(const QString &fname)
{
  bool b = m_qjsw->loadJson(fname);
  if (!b)
  {
      QMessageBox::critical(this,tr("Error"),m_qjsw->error());
  }
  m_qjsw->expandAll();
#ifdef _TEST
  m_qjsw->resizeColumnToContent(1);
#endif
  m_qjsw->resizeColumnToContent(2);
  m_qjsw->resizeColumnToContent(0);
  m_qjsw->setStretchLastSection(false);
  m_lastLoaded = fname;
}

void Dialog::loadCfg()
{
#ifdef _TEST
  QString fname (QApplication::applicationDirPath() % "/test.json");
#else
  QString fname (QApplication::applicationDirPath() % "/rkcfg.json");
#endif
  loadCfg(fname);
}

void Dialog::on_honorReadOnlyCheckBox_toggled(bool checked)
{
  checked ? (m_qjsw->setSpecialFlags(m_qjsw->specialFlags() | QJsonTreeItem::HonorReadOnly)) :
    (m_qjsw->setSpecialFlags(m_qjsw->specialFlags() & ~QJsonTreeItem::HonorReadOnly));
}

void Dialog::on_honorHideCheckBox_toggled(bool checked)
{
  checked ? (m_qjsw->setSpecialFlags(m_qjsw->specialFlags() | QJsonTreeItem::HonorHide)) :
    (m_qjsw->setSpecialFlags(m_qjsw->specialFlags() & ~QJsonTreeItem::HonorHide));
}

void Dialog::on_treatROAsHideCheckBox_toggled(bool checked)
{
  checked ? (m_qjsw->setSpecialFlags(m_qjsw->specialFlags() | QJsonTreeItem::ReadOnlyHidesRow)) :
    (m_qjsw->setSpecialFlags(m_qjsw->specialFlags() & ~QJsonTreeItem::ReadOnlyHidesRow));
}

void Dialog::on_savePushButton_clicked()
{
  // always save to appdir/output.json
  QString fname (QApplication::applicationDirPath() % "/output.json");
  m_qjsw->saveJson(fname,QJson::IndentMinimum);
  QFileInfo fi (fname);
  QDesktopServices::openUrl(fi.path());
}

void Dialog::on_clearPushButton_clicked()
{
    m_qjsw->clear();
}

void Dialog::on_reloadPushButton_clicked()
{
  loadCfg(m_lastLoaded);
}

void Dialog::onTreeButtonClicked(const QJsonTreeItem *item, const QString& jsontag)
{
  QMessageBox::information(this,"clicked",jsontag);
}

void Dialog::on_loadPushButton_clicked()
{
  QString fname = QFileDialog::getOpenFileName(this, tr("Select configuration"), QString(), tr("JSON files (*.*)"));
  if (fname.isEmpty())
    return;
  loadCfg(fname);
}

