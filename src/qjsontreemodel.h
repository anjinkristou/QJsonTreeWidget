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

#ifndef QJSONTREEMODEL_H
#define QJSONTREEMODEL_H

#include <QtCore>
#include "qjsontreeitem.h"
#include <QJson/Serializer>
#include <QJson/Parser>
#include <QJson/QObjectHelper>

#define JSON_TREE_MAX_VERSION 3 // maximum supported JSON version by the library
#define MAX_ROWS_FOR_FETCH 100 // fetch this number of rows per time (for bigger trees)

/**
 * @brief class to model a tree from a JSON file/buffer coming from QJson
 *
 */
class QJsonTreeModel : public QAbstractItemModel
{
  Q_OBJECT
public:
  /**
   * @brief constructor
   *
   * @param parent the parent object (optional)
   */
  explicit QJsonTreeModel(QObject *parent = 0);

  /**
   * @brief destructor
   *
   */
  ~QJsonTreeModel();

  /**
   * @brief returns the root tree item (the whole tree itself), which is used only to store private data.
   * All the displayed items by the view are childs of this item.
   *
   * @return QJsonTreeItem
   */
  QJsonTreeItem* root() const { return m_root; }

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
  int maxSupportedJsonVersion() const { return m_maxVersion; }

  /**
   * @brief implementation of index() from the QAbstractItemModel interface
   *
   * @param row the row
   * @param column the column
   * @param parent the parent index (optional)
   * @return QModelIndex
   */
  QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex()) const;

  /**
   * @brief implementation of parent() from the QAbstractItemModel interface
   *
   * @param index the model index
   * @return QModelIndex
   */
  QModelIndex parent ( const QModelIndex & index ) const;

  /**
   * @brief implementation of rowCount() from the QAbstractItemModel interface
   *
   * @param parent the parent index (optional)
   * @return int
   */
  int rowCount ( const QModelIndex & parent = QModelIndex() ) const;

  /**
   * @brief implementation of columnCount() from the QAbstractItemModel interface
   *
   * @param parent the parent index (optional)
   * @return int
   */
  int columnCount ( const QModelIndex & parent = QModelIndex() ) const;

  /**
   * @brief implementation of data() from the QAbstractItemModel interface
   *
   * @param index the model index
   * @param role the index role (optional)
   * @return QVariant
   */
  QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;

  /**
   * @brief implementation of setData() from the QAbstractItemModel interface, to provide data editing
   *
   * @param index the model index
   * @param value the value to set in the model at index
   * @param role the index role (optional)
   * @return bool
   */
  bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );

  /**
   * @brief implementation of flags() from the QAbstractItemModel interface, to provide data editing
   *
   * @param index the model index
   * @return Qt::ItemFlags
   */
  Qt::ItemFlags flags ( const QModelIndex & index ) const;

  /**
   * @brief implementation of headerData() from the QAbstractItemModel interface, to display the column's header.
   *
   * @param section the header section (= column)
   * @param orientation the orientation
   * @param role the section role (optional)
   * @return QVariant
   */
  QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

  /**
   * @brief implementation of removeRows() from the QAbstractItemModel interface
   *
   * @param row the (initial) row to remove
   * @param count is > 1, remove count rows from row on
   * @param parent the parent index (optional)
   * @return bool
   */
  bool removeRows ( int row, int count, const QModelIndex & parent = QModelIndex());

  /**
   * @brief implementation of removeRow() from the QAbstractItemModel interface (this just calls removeRows with count = 1)
   *
   * @param row the row to remove
   * @param parent the parent index (optional)
   * @return bool
   */
  bool removeRow(int row, const QModelIndex &parent = QModelIndex()) { return removeRows(row,1,parent); }

  /**
   * @brief implementation of insertRows() from the QAbstractItemModel interface
   *
   * @param row the row where to append the rows after
   * @param count number of rows to insert
   * @param parent the parent index (optional)
   * @return bool
   */
  bool insertRows ( int row, int count, const QModelIndex & parent = QModelIndex());

  /**
   * @brief implementation of insertRow() from the QAbstractItemModel interface
   *
   * @param row the row where to insert the rows after
   * @param parent the parent index (optional)
   * @return bool
   */
  bool insertRow (int row, const QModelIndex &parent = QModelIndex());

  /**
   * @brief implementation of hasChildren() from the QAbstractItemModel interface
   *
   * @param parent the parent index (optional)
   * @return bool
   */
  bool hasChildren ( const QModelIndex & parent = QModelIndex() );

  /**
   * @brief implementation of canFetchMore() from the QAbstractItemModel interface
   *
   * @param parent the parent index (optional)
   * @return bool
   */
  bool canFetchMore(const QModelIndex &parent) const;

  /**
   * @brief implementation of fetchMore() from the QAbstractItemModel interface
   *
   * @param parent the parent index
   * @return bool
   */
  void fetchMore(const QModelIndex &parent);

  /**
   * @brief clears the model by calling reset and deleting the tree
   *
   */
  void clear();

  /**
   * @brief sets the model special flags to control how the view how to displays the items
   *
   * @param QJsonTreeItem::SpecialFlag one or more special flags
   */
  void setSpecialFlags(QJsonTreeItem::SpecialFlags flags);

  /**
   * @brief returns the current value of special flags
   *
   * @return QJsonTreeItem::SpecialFlags
   */
  QJsonTreeItem::SpecialFlags specialFlags() const { return m_specialFlags; }

  /**
   * @brief returns the tree item corresponding to the model index
   *
   * @param index the model index
   * @param role the display role (optional)
   * return QJsonTreeItem
   */
  QJsonTreeItem* itemByModelIndex (const QModelIndex& index, int role = Qt::DisplayRole) const;

  /**
   * @brief returns the JSON tag corresponding to the model index
   *
   * @param index the model index
   * @param item on return, the corresponding QJsonTreeItem* (optional)
   * @param role the display role (optional)
   * return const QString
   */
  const QString tagByModelIndex (const QModelIndex &index, QJsonTreeItem** item=0,int role = Qt::DisplayRole) const;

  /**
   * @brief returns the map corresponding to the item at model index
   *
   * @param index the model index
   * @param item on return, the corresponding QJsonTreeItem* (optional)
   * @param role the display role (optional)
   * return QVariantMap
   */
  QVariantMap mapByModelIndex (const QModelIndex& index, QJsonTreeItem** item=0,int role = Qt::DisplayRole) const;

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

  private:
  bool buildModel(const QByteArray& buf);
  void setNotFoundInvalidOrEmptyError(const QString& function,const QString& val);
  QJsonTreeItem* parentItem(const QModelIndex& parent) const;
  QJsonTreeItem* m_root;
  QJson::Parser* m_parser;
  QJson::Serializer* m_serializer;
  QString m_error;
  QJsonTreeItem::SpecialFlags m_specialFlags;
  int m_maxVersion;
  int m_maxRows;
};

#endif // QJSONTREEMODEL_H
