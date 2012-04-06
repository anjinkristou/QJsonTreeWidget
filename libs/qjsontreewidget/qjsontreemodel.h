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
#include <QApplication>
#include <QStyle>
#include "qjsontreeitem.h"

class QJsonTreeItemDelegate;

/**
 * @brief class to model a tree from a JSON file/buffer coming from QJson
 *
 */
class QJsonTreeModel : public QAbstractItemModel
{
  friend class QJsonTreeItem;
  friend class QJsonTreeWidget;
  friend class QJsonTreeItemDelegate;
  friend class QJsonSortFilterProxyModel;

  Q_OBJECT
public:
  /**
   * @brief constructor
   *
   * @param root the root item
   * @param parent the parent object (optional)
   */
  explicit QJsonTreeModel(QJsonTreeItem* root, QObject *parent = 0);

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
   * @brief clears the model by calling reset and deleting the tree
   *
   */
  void clear();

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
   * @brief returns the modelindex corresponding to a tree item
   *
   * @param item the QJsonTreeWidget item
   * @param column the column
   * return const QModelIndex
   */
  const QModelIndex indexByItem(QJsonTreeItem *item, int column) const;

protected:
  void setSpecialFlags(QJsonTreeItem::SpecialFlags flags);
  QJsonTreeItem::SpecialFlags specialFlags() const { return m_specialFlags; }

 private:
  QJsonTreeItem* parentItem(const QModelIndex& parent) const;
  QJsonTreeItem* m_root;
  QJsonTreeItem::SpecialFlags m_specialFlags;
};

#endif // QJSONTREEMODEL_H
