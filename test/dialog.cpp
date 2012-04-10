#include <QtCore>
#include <QtGui>
#include "dialog.h"
#include "ui_dialog.h"

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
  m_qjsw->resizeColumnToContent(1);
  m_qjsw->resizeColumnToContent(2);
  m_qjsw->resizeColumnToContent(0);
  m_qjsw->setStretchLastSection(false);
  m_qjsw->setLoadFromFileEnabled(true);
  m_qjsw->setSortingEnabled(true);
  m_qjsw->setColumnForegroundColor("_desc_",QApplication::style()->standardPalette().mid().color());
  QFont f;
  f.setItalic(true);
  m_qjsw->setColumnFont("_desc_",f);
  f.setBold(true);
  f.setItalic(false);
  m_qjsw->setParentsFont(f);
  m_lastLoaded = fname;
}

void Dialog::loadCfg()
{
  QString fname (QApplication::applicationDirPath() % "/test.json");
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
  QString fname = QFileDialog::getOpenFileName(0, tr("Select path"),QString(),tr("All files (*.*)"));
  if (!fname.isEmpty())
  {
    const_cast<QJsonTreeItem*>(item)->setMapValue("value",fname);
  }
}

void Dialog::on_searchButton_clicked()
{
  m_qjsw->search(ui->searchLineEdit->text());
}

void Dialog::on_honorChildFontCheckbox_toggled(bool checked)
{
  checked ? (m_qjsw->setSpecialFlags(m_qjsw->specialFlags() | QJsonTreeItem::HonorChildsFont)) :
    (m_qjsw->setSpecialFlags(m_qjsw->specialFlags() & ~QJsonTreeItem::HonorChildsFont));
}

void Dialog::on_honorParentFontCheckbox_toggled(bool checked)
{
  checked ? (m_qjsw->setSpecialFlags(m_qjsw->specialFlags() | QJsonTreeItem::HonorParentsFont)) :
    (m_qjsw->setSpecialFlags(m_qjsw->specialFlags() & ~QJsonTreeItem::HonorParentsFont));
}

void Dialog::on_honorChildFgColorCheckBox_toggled(bool checked)
{
  checked ? (m_qjsw->setSpecialFlags(m_qjsw->specialFlags() | QJsonTreeItem::HonorChildsForegroundColor)) :
    (m_qjsw->setSpecialFlags(m_qjsw->specialFlags() & ~QJsonTreeItem::HonorChildsForegroundColor));
}

void Dialog::on_honorChildBgColorCheckBox_toggled(bool checked)
{
  checked ? (m_qjsw->setSpecialFlags(m_qjsw->specialFlags() | QJsonTreeItem::HonorChildsBackgroundColor)) :
    (m_qjsw->setSpecialFlags(m_qjsw->specialFlags() & ~QJsonTreeItem::HonorChildsBackgroundColor));
}

void Dialog::on_honorParentBgColorCheckBox_toggled(bool checked)
{
  checked ? (m_qjsw->setSpecialFlags(m_qjsw->specialFlags() | QJsonTreeItem::HonorParentsBackgroundColor)) :
    (m_qjsw->setSpecialFlags(m_qjsw->specialFlags() & ~QJsonTreeItem::HonorParentsBackgroundColor));
}

void Dialog::on_honorParentFgColorCheckBox_toggled(bool checked)
{
  checked ? (m_qjsw->setSpecialFlags(m_qjsw->specialFlags() | QJsonTreeItem::HonorParentsForegroundColor)) :
    (m_qjsw->setSpecialFlags(m_qjsw->specialFlags() & ~QJsonTreeItem::HonorParentsForegroundColor));
}

void Dialog::on_honorItemBgColorCheckBox_toggled(bool checked)
{
  checked ? (m_qjsw->setSpecialFlags(m_qjsw->specialFlags() | QJsonTreeItem::HonorItemBackgroundColor)) :
    (m_qjsw->setSpecialFlags(m_qjsw->specialFlags() & ~QJsonTreeItem::HonorItemBackgroundColor));
}

void Dialog::on_honorItemFgColorCheckBox_toggled(bool checked)
{
  checked ? (m_qjsw->setSpecialFlags(m_qjsw->specialFlags() | QJsonTreeItem::HonorItemForegroundColor)) :
    (m_qjsw->setSpecialFlags(m_qjsw->specialFlags() & ~QJsonTreeItem::HonorItemForegroundColor));
}

void Dialog::on_honorItemFontCheckBox_toggled(bool checked)
{
  checked ? (m_qjsw->setSpecialFlags(m_qjsw->specialFlags() | QJsonTreeItem::HonorItemFont)) :
    (m_qjsw->setSpecialFlags(m_qjsw->specialFlags() & ~QJsonTreeItem::HonorItemFont));
}

void Dialog::on_honorColumnFontCheckBox_toggled(bool checked)
{
  checked ? (m_qjsw->setSpecialFlags(m_qjsw->specialFlags() | QJsonTreeItem::HonorColumnFont)) :
    (m_qjsw->setSpecialFlags(m_qjsw->specialFlags() & ~QJsonTreeItem::HonorColumnFont));
}

void Dialog::on_honorColumnFgColorCheckBox_toggled(bool checked)
{
  checked ? (m_qjsw->setSpecialFlags(m_qjsw->specialFlags() | QJsonTreeItem::HonorColumnForegroundColor)) :
    (m_qjsw->setSpecialFlags(m_qjsw->specialFlags() & ~QJsonTreeItem::HonorColumnForegroundColor));

}

void Dialog::on_honorColumnBgColorCheckBox_toggled(bool checked)
{
  checked ? (m_qjsw->setSpecialFlags(m_qjsw->specialFlags() | QJsonTreeItem::HonorColumnBackgroundColor)) :
    (m_qjsw->setSpecialFlags(m_qjsw->specialFlags() & ~QJsonTreeItem::HonorColumnBackgroundColor));

}
