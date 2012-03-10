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
   void on_savePushButton_clicked();
   void on_clearPushButton_clicked();
   void on_reloadPushButton_clicked();
   void on_loadPushButton_clicked();

   void onTreeButtonClicked(const QJsonTreeItem* item, const QString& jsontag);
 };

#endif // DIALOG_H
