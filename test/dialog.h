#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <qjsontreewidget.h>

namespace Ui {
  class Dialog;
}
 class Dialog : public QDialog
 {
   Q_OBJECT

 public:
   explicit Dialog(QWidget *parent = 0);
   ~Dialog();

 private:
   Ui::Dialog *ui;
   void loadCfg(const QString& fname);
   QWidget *m_widget;
   QJsonTreeWidget* m_qjsw;
   QString m_lastLoaded;
 private slots:
   void loadCfg();
   void on_honorReadOnlyCheckBox_toggled(bool checked);
   void on_honorHideCheckBox_toggled(bool checked);
   void on_treatROAsHideCheckBox_toggled(bool checked);
   void on_clearPushButton_clicked();
   void on_reloadPushButton_clicked();

   void onTreeButtonClicked(const QJsonTreeItem* item, const QString& jsontag);
   void on_searchButton_clicked();
   void on_honorChildFontCheckbox_toggled(bool checked);
   void on_honorParentFontCheckbox_toggled(bool checked);
   void on_honorChildFgColorCheckBox_toggled(bool checked);
   void on_honorChildBgColorCheckBox_toggled(bool checked);
   void on_honorParentBgColorCheckBox_toggled(bool checked);
   void on_honorParentFgColorCheckBox_toggled(bool checked);
   void on_honorItemBgColorCheckBox_toggled(bool checked);
   void on_honorItemFgColorCheckBox_toggled(bool checked);
   void on_honorItemFontCheckBox_toggled(bool checked);
   void on_honorColumnFontCheckBox_toggled(bool checked);
   void on_honorColumnFgColorCheckBox_toggled(bool checked);
   void on_honorColumnBgColorCheckBox_toggled(bool checked);
 };

#endif // DIALOG_H
