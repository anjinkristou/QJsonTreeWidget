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
#include "qjsonsortfilterproxymodel.h"

QJsonSortFilterProxyModel::QJsonSortFilterProxyModel(QObject *parent) :
  QSortFilterProxyModel(parent)
{
}

QJsonSortFilterProxyModel::~QJsonSortFilterProxyModel()
{
}

bool QJsonSortFilterProxyModel::filterAcceptsRow ( int source_row, const QModelIndex & source_parent ) const
{
  if (!source_parent.isValid())
    return true;

  const QJsonTreeModel* model = static_cast<const QJsonTreeModel*>(source_parent.model());
  QJsonTreeItem* item = model->itemByModelIndex(source_parent)->child(source_row);
  int flags = model->specialFlags();
  if (item->map().contains("_template_"))
    return false;

  bool roset = item->map().value("__hasROSet__",false).toBool();
  if (roset && (flags & QJsonTreeItem::ReadOnlyHidesRow) && (flags & QJsonTreeItem::HonorHide))
    return false;

  bool hide = item->map().value("_hide_",false).toBool();
  if (hide && (flags & QJsonTreeItem::HonorHide))
    return false;

  // check regexp on each column
  if (this->filterRegExp().isEmpty())
    return true;
  int cols = item->columnCount();
  for (int i=0; i < cols; i++)
  {
    // select ?
    if (item->text(i).contains(this->filterRegExp()))
    {
      QModelIndex mi = this->mapFromSource(model->indexByItem(item,i));
      item->view()->selectionModel()->select(mi,QItemSelectionModel::Select);
    }
  }
  return true;
}

QModelIndex QJsonSortFilterProxyModel::indexToSourceIndex (const QModelIndex& index)
{
  if (!index.isValid())
    return QModelIndex();
  return static_cast<const QJsonSortFilterProxyModel*>(index.model())->mapToSource(index);
}

QModelIndex QJsonSortFilterProxyModel::indexToProxyIndex (const QModelIndex& index)
{
  if (!index.isValid())
    return QModelIndex();
  return static_cast<const QJsonSortFilterProxyModel*>(index.model())->mapFromSource(index);
}

const QJsonTreeModel* QJsonSortFilterProxyModel::indexSourceModel (const QModelIndex& index)
{
  if (!index.isValid())
    return 0;

  const QJsonSortFilterProxyModel* m = static_cast <const QJsonSortFilterProxyModel*>(index.model());
  return static_cast<const QJsonTreeModel*>(m->sourceModel());
}
