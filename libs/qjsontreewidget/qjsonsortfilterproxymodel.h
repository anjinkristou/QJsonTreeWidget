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
  virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;

public:
  explicit QJsonSortFilterProxyModel(QObject *parent = 0);
  static QModelIndex indexToSourceIndex (const QModelIndex& index);
  static QModelIndex indexToProxyIndex (const QModelIndex& index);
  static const QJsonTreeModel* indexSourceModel (const QModelIndex& index);
};

#endif // QJSONSORTFILTERPROXYMODEL_H
