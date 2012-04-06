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
#include <QJson/Serializer>
#include <QJson/Parser>
#include <QJson/QObjectHelper>
#include "qjsontree_global.h"
#include "qjsontreemodel.h"
#include "qjsontreeitemdelegate.h"
#include "qjsonsortfilterproxymodel.h"

#define JSON_TREE_MAX_VERSION 3 // maximum supported JSON version by the library

/**
  * @brief class to represent a JSON file using a tree widget and viceversa.
  * the format used to define the tree as a JSON is outlined here: https://www.te4i.com/confluence/display/H21/Configuration+JSON+format+%28rkmodv2%29
  *
  */
 class QJSONTREE_EXPORT QJsonTreeWidget : public QWidget{
 Q_OBJECT
 public:

   friend class QJsonTreeItem;
   friend class QJsonTreeModel;

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
    * @brief returns the optional QGridLayout where the user can add its own controls
    *
    * @return QGridLayout
    */
   QGridLayout* optLayout() const { return m_optLayout; }

   /**
    * @brief returns detailed error
    *
    * @return const QString
    */
   const QString error() const { return m_error; }

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
    * @brief
    *
    * @param buf a QVariant map
    * @return bool false if the map do not contain valid data for QJsonTreeWidget, look at error() for detailed error string
    */
   bool loadJson (const QVariantMap& map);

   /**
    * @brief serializes the tree to a JSON file
    *
    * @param path path to the JSON file to be saved
    * @param indentmode one of the indentation mode defined in QJson::IndentMode
    * @param additional an optional additional map to be added
    * @return bool
    */
   bool saveJson(const QString& path, QJson::IndentMode indentmode, const QVariantMap& additional = QVariantMap());

   /**
    * @brief serializes the tree to a QIODevice file
    *
    * @param dev a QIODevice (i.e. QFile)
    * @param indentmode one of the indentation mode defined in QJson::IndentMode
    * @param additional an optional additional map to be added
    * @return bool
    */
   bool saveJson (QIODevice& dev, QJson::IndentMode indentmode, const QVariantMap& additional = QVariantMap());

   /**
    * @brief serializes the tree to a JSON buffer
    *
    * @param indentmode one of the indentation mode defined in QJson::IndentMode
    * @param additional an optional additional map to be added
    * @return QByteArray
    */
   QByteArray saveJson (QJson::IndentMode indentmode, const QVariantMap& additional = QVariantMap());

   /**
    * @brief saves the tree to a QVariantMap
    *
    * @return QVariantMap
    */
   QVariantMap saveJson () const { return m_model->root()->toMap(); }

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
   QJsonTreeItem* invisibleRootItem() const { return m_model->root(); }

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

   /**
    * @brief returns the QJSON serializer
    *
    * return QJson::Serializer
    */
   QJson::Serializer* serializer() const { return m_serializer; }

   /**
    * @brief returns the QJSON parser
    *
    * return QJson::Parser
    */
   QJson::Parser* parser() const { return m_parser; }

   /**
    * @brief returns the version of JSON map to be represented (tag "version" in the map)
    *
    * @param map the JSON map
    * @return int
    */
   int jsonVersion(const QVariantMap map) const;

   /**
    * @brief returns the maximum supported JSON tree version by the library
    *
    * @return int
    */
   int jsonMaxSupportedVersion() const { return m_maxVersion; }

   /**
    * @brief enable or disable the dynamic sort/filtering
    *
    * @param enable view is sorted on insertion/deletion too, if sorting is enabled
    */
   void setDynamicSortFiltering(bool enable) { m_proxyModel->setDynamicSortFilter(enable); }

   /**
    * @brief enable or disable sorting
    *
    * @param enable enable/disable sorting in the view
    */
   void setSortingEnabled(bool enable);

   /**
    * @brief sets the sort order (ascending/descending)
    *
    * @param column column to set the sort order to
    * @param order the sort order (items are sorted in respect of the tree structure)
    */
   void setSortOrder(int column, Qt::SortOrder order) { m_view->header()->setSortIndicator(column,order); }

   /**
    * @brief recursively search text in the tree
    *
    * @param text the text to find
    */
   void search(const QString& text);

   /**
    * @brief recursively search text in the tree
    *
    * @param regex the regexp to match
    */
   void search(const QRegExp& regex);

   /**
    * @brief enable the right-click "load from file" option
    *
    * @param enable to enable load
    */
   void setLoadFromFileEnabled (bool enable) { m_actionLoad->setVisible(enable); }

   /**
    * @brief returns the state of the load-from-file action (visible/not visible)
    *
    * @return bool
    */
   bool loadFromFileEnabled() const { return m_actionLoad->isVisible(); }

   /**
    * @brief returns the actions set in the view's header (to add custom actions)
    *
    * @return QList<QAction*>
    */
   QList<QAction*> headerActions() const { return m_view->header()->actions(); }

   /**
    * @brief sets the list of tags to be purged when saving the tree
    * @param purgelist if not empty, an hash representing tags to strip off from saved JSON. true strips the item completely, including childs. false just strips the tag leaving the item
    */
   void setPurgeListOnSave (const QHash<QString, bool>& purgelist) { m_purgeList = purgelist; }

   /**
    * @brief returns the purge list to be applied on saving
    *
    * @return QHash<QString,bool>
    */
   const QHash<QString,bool> purgeListOnSave () const { return m_purgeList; }

   /**
    * @brief enable editing on the widget
    *
    * @param enable to enable editing
    */
   void setEditingEnabled(bool enable) { m_editing = enable; }

   /**
    * @brief returns if editing is enabled on the widget
    *
    * @return bool
    */
   bool editingEnabled() const { return m_editing; }

   /**
    * @brief to purge all descriptive tags on save. this is similar to setPurgeListOnSave, but just purges only QJsonTreeWidget descriptive tags without stripping the
    * children of items containing the tags
    *
    * @param enable true to purge
    */
   void setPurgeDescriptiveTagsOnSave(bool enable) { m_purgeDescriptiveTags = enable; }

   /**
    * @brief returns if purge descriptive tags is enabled on the widget
    *
    * @return bool
    */
   bool purgeDescriptiveTags() const { return m_purgeDescriptiveTags; }

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
    * @param jsontag the corresponding JSON tag at the position identified by column and row in the tree
    */
   void clicked (const QJsonTreeItem* item, const QString& jsontag);

 private slots:
   void onDataChanged (const QModelIndex & topLeft, const QModelIndex & bottomRight );
   void nextSelection();
   void onActionLoad();
   void onActionSave();
   void onActionEnableSort();
   void onActionDisableSort();

 protected:
   QJsonSortFilterProxyModel* proxyModel() const { return m_proxyModel; }
   QTreeView* view() const { return m_view; }
   QJsonTreeModel* model() const { return m_model; }
   bool loadJsonInternal(const QVariantMap &map);
   void setNotFoundInvalidOrEmptyError(const QString &function, const QString &val);
   virtual void keyPressEvent(QKeyEvent *event);

 private:
   void searchInternal();

   QTreeView* m_view;
   QGridLayout* m_optLayout;
   QJsonTreeModel* m_model;
   QJsonSortFilterProxyModel* m_proxyModel;
   QJsonTreeItem* m_root;
   QJsonTreeItemDelegate* m_delegate;
   QString m_error;
   QJson::Parser* m_parser;
   QJson::Serializer* m_serializer;
   QModelIndex m_currentSelection;
   QAction* m_actionLoad;
   QAction* m_actionSave;
   QAction* m_actionEnableSort;
   QAction* m_actionDisableSort;
   QHash<QString,bool> m_purgeList;
   bool m_purgeDescriptiveTags;
   bool m_editing;
   int m_maxVersion;
 };

#endif // QJSONTREEWIDGET_H
