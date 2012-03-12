/*
    This file is part of QJsonTreeWidget.

    Copyright (C) 2012 valerino <valerio.lupi@te4i.com>

    QJsonTreeWidget is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QJsonTreeWidget is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QJsonTreeWidget.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef QJSONTREEWIDGET_H
#define QJSONTREEWIDGET_H

#include <QtCore>
#include <QtGui>
#include <parser.h>
#include "qjsontreemodel.h"
#include "qjsontreeitemdelegate.h"

/**
  * @brief class to represent a JSON file using a tree widget and viceversa.
  * the format used to define the tree as a JSON is outlined here: https://www.te4i.com/confluence/display/H21/Configuration+JSON+format+%28rkmodv2%29
  *
  */
 class QJsonTreeWidget : public QWidget{
 Q_OBJECT
 public:

   /**
    * @brief
    *
    * @param parent the parent object (optional)
    * @param f window flags (optional)
    */
   QJsonTreeWidget(QWidget * parent = 0, Qt::WindowFlags f = 0 );

   /**
    * @brief destructor
    *
    */
   ~QJsonTreeWidget();

   /**
    * @brief returns the QTreeView
    *
    * @return QTreeView
    */
   QTreeView* view() const { return m_view; }

   /**
    * @brief returns the optional QGridLayout where the user can add its own controls
    *
    * @return QGridLayout
    */
   QGridLayout* optLayout() const { return m_optLayout; }

   /**
    * @brief returns detailed error from the underlying model
    *
    * @return const QString
    */
   const QString error() const { return m_model->error(); }

   /**
    * @brief
    *
    * @param path path to the JSON file
    * @return bool false on parser error, look at error() for detailed error string
    */
   bool loadJson(const QString& path);

   /**
    * @brief
    *
    * @param dev a QIODevice (i.e. QFile) to read the JSON from
    * @return bool false on parser error, look at error() for detailed error string
    */
   bool loadJson (QIODevice& dev);

   /**
    * @brief
    *
    * @param buf a QByteArray containing the JSON buffer
    * @return bool false on parser error, look at error() for detailed error string
    */
   bool loadJson (const QByteArray& buf);

   /**
    * @brief serializes the tree to a JSON file
    *
    * @param path path to the JSON file to be saved
    * @param indentmode one of the indentation mode defined in QJson::IndentMode
    * @return bool
    */
   bool saveJson(const QString& path, QJson::IndentMode indentmode) { return m_model->saveJson(path, indentmode); }

   /**
    * @brief serializes the tree to a QIODevice file
    *
    * @param dev a QIODevice (i.e. QFile)
    * @param indentmode one of the indentation mode defined in QJson::IndentMode
    * @return bool
    */
   bool saveJson (QIODevice& dev, QJson::IndentMode indentmode) { return m_model->saveJson(dev,indentmode); }

   /**
    * @brief serializes the tree to a JSON buffer
    *
    * @param indentmode one of the indentation mode defined in QJson::IndentMode
    * @return QByteArray
    */
   QByteArray saveJson (QJson::IndentMode indentmode) { return m_model->saveJson(indentmode); }

   /**
    * @brief expands all the items in the tree (warning: if the view contains lot of items, it may take time)
    *
    */
   void expandAll() { m_view->expandAll(); }

   /**
    * @brief expands all the items in the tree to the given depth
    *
    * @param depth depths to expands the tree to
    */
   void expandToDepth(int depth) { m_view->expandToDepth(depth); }

   /**
    * @brief collapse all items in the tree
    *
    */
   void collapseAll() { m_view->collapseAll(); }

   /**
    * @brief resize all columns to their content size
    *
    */
   void resizeColumnsToContents();

   /**
    * @brief resize the given column to content size
    *
    * @param column the column to resize
    */
   void resizeColumnToContent(int column) { m_view->resizeColumnToContents(column); }

   /**
    * @brief returns the root tree item (the whole tree itself), which is used only to store private data.
    * all the displayed items by the view are children of this item. this is to be consistent with other widget-based QT api.
    * @return QJsonTreeItem
    */
   QJsonTreeItem* invisibleRootItem() { return m_model->root(); }

   /**
    * @brief clears the widget
    *
    */
   void clear ();

   /**
    * @brief sets the model special flags to control how the view display the items
    *
    * @param QJsonTreeItem::SpecialFlag one or more special flags
    */
   void setSpecialFlags(QJsonTreeItem::SpecialFlags flags) { m_model->setSpecialFlags(flags); }

   /**
    * @brief returns the current value of special flags
    *
    * @return QJsonTreeItem::SpecialFlags
    */
   QJsonTreeItem::SpecialFlags specialFlags() const { return m_model->specialFlags(); }

   /**
    * @brief set the last view's header section stretched or not
    * @param stretch true to stretch
    */
   void setStretchLastSection(bool stretch) { m_view->header()->setStretchLastSection(stretch); }

   /**
    * @brief returns the view header
    *
    * return QHeaderView
    */
   QHeaderView* header() { return m_view->header(); }

   signals:
   /**
    * @brief connect to this signal to be notified of generic mouseclicks on the view
    *
    * @param column the clicked column index
    * @param row the clicked row index
    * @param jsontag the corresponding JSON tag at column,row
    * @param value the value displayed at column,row if any
    * @param item the corresponding QJsonTreeItem* at row (you can get the full map calling item->map())
    */
   void clicked (int column, int row, const QString& jsontag, const QVariant& value, const QJsonTreeItem* item);

   /**
    * @brief connect to this signal to be notified of mouseclicks on a pushbutton in the view (_button_ JSON tag)
    * essentially this is a shortcut for the above more complete signal
    * @param item the corresponding QJsonTreeItem* (you can get the full map calling item->map())
    * @param item the corresponding JSON tag at the position identified by column and row in the tree
    */
   void clicked (const QJsonTreeItem* item, const QString& jsontag);

 private slots:
   void onDataChanged (const QModelIndex & topLeft, const QModelIndex & bottomRight );

 private:
   QTreeView* m_view;
   QGridLayout* m_optLayout;
   QJsonTreeModel* m_model;
   QJsonTreeItemDelegate* m_delegate;
   QString m_error;
 };

#endif // QJSONTREEWIDGET_H
