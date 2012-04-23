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

#ifndef QJSONSORTFILTERPROXYMODEL_H
#define QJSONSORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QTreeView>
#include "qjsontreemodel.h"
#include "qjsontreeitem.h"

class QJsonSortFilterProxyModel : public QSortFilterProxyModel
{
  friend class QJsonTreeModel;
  friend class QJsonTreeItem;
  Q_OBJECT
protected:
 /**
  * @brief reimplementation of filterAcceptsRow() from the QSortFilterProxyModel interface
  *
  * @param source_row the row index in the source model
  * @param source_parent the parent model index
  * @return bool
  */
  virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;

public:
  /**
   * @brief constructor
   *
   * @param parent the parent object (optional)
   */
  explicit QJsonSortFilterProxyModel(QObject *parent = 0);

  /**
   * @brief destructor
   *
   */
  virtual ~QJsonSortFilterProxyModel();

  /**
   * @brief helper function to map an index to the source model index (calls mapToSource() on the index model)
   *
   * @param index must be a valid proxy model index
   * @return QModelIndex
   */
  static QModelIndex indexToSourceIndex (const QModelIndex& index);

  /**
   * @brief helper function to map an index to the proxy model index (calls mapToSource() on the index model)
   *
   * @param index must be a valid source model index
   * @return QModelIndex
   */
  static QModelIndex indexToProxyIndex (const QModelIndex& index);

  /**
   * @brief helper function to retrieve the index source model
   *
   * @param index must be a valid proxy model index
   * @return const QJsonTreeModel*
   */
  static const QJsonTreeModel* indexSourceModel (const QModelIndex& index);

};

#endif // QJSONSORTFILTERPROXYMODEL_H
