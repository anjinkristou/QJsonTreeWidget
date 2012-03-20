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

#include "qjsontreeitem.h"

QHash<QString,Qt::ItemFlags> QJsonTreeItem::widgetFlags;

QJsonTreeItem::QJsonTreeItem (QJsonTreeItem *parent, const QVariantMap &map)
{
  this->clear();
  m_parentItem = parent;

  // this is done only once
  if (QJsonTreeItem::widgetFlags.isEmpty())
    QJsonTreeItem::buildWidgetFlags();

  // just create an empty item ?
  if (map.isEmpty())
    return;

  // create item from map
  fromMap(map,parent);
}

bool QJsonTreeItem::fromMap(const QVariantMap &map, QJsonTreeItem *parent)
{
  this->clear();

  m_parentItem = parent;
  m_map = map;

  if (parent == 0)
  {
    m_rootItem = this;

    // this is the root item, must have "_headers_" set, store headers descriptor in m_headers
    if (!setColumnHeaders(map.value("_headers_",QString()).toString()))
    {
      m_error = QJsonTreeItem::JsonMissingOrInvalidHeaders;
      m_invalidMap = m_map;

      // this invalidates the root map too
      m_rootItem->m_error = m_error;
      m_rootItem->m_invalidMap = m_invalidMap;
      return false;
    }
  }
  else
  {
    m_headers = static_cast<QHash<QString, QHash<QString, QVariant> > >(parent->m_headers); // always points to parent
    m_rootItem = parent->rootItem();
  }

  /*// name is mandatory (except in the invisible root item)
  if (!m_map.contains("name") && (m_rootItem != this))
  {
    m_error = QJsonTreeItem::JsonInvalidMap;
    m_invalidMap = m_map;

    // this invalidates the root map too
    m_rootItem->m_error = m_error;
    m_rootItem->m_invalidMap = m_invalidMap;
    return false;
  }
  */
  // this is to optimize model index() function
  foreach (QString k, m_map.keys())
  {
    if (k.contains("_readonly_"))
    {
      // add this value, this will be checked for special flag ReadOnlyHidesRow
      m_map["__hasROSet__"] = m_map[k];
      break;
    }
  }

  if (m_map.contains("_children_"))
  {
    QVariantList l = m_map["_children_"].toList();
    foreach (QVariant mm, l)
    {
      // recurse
      QJsonTreeItem* i = new QJsonTreeItem(this,mm.toMap());
      this->appendChild(i);
    }
  }

  // strip "_children_" (to not waste memory, on save we use the tree structure to regenerate it)
  m_map.remove("_children_");
  m_error = QJsonTreeItem::JsonNoError;

  return true;
}

QJsonTreeItem::~QJsonTreeItem()
{
  this->clear();
}

void QJsonTreeItem::appendChild(QJsonTreeItem *child)
{
  m_childItems.append(child);
  m_rootItem->m_totalTreeItems++;
}

void QJsonTreeItem::removeChild(int row)
{
  QJsonTreeItem* it = this->child(row);
  m_childItems.removeAt(row);
  delete it;
  m_rootItem->m_totalTreeItems--;
}

int QJsonTreeItem::totalChildCount() const
{
  int rowabs = this->rowAbsolute();
  return (m_rootItem->m_totalTreeItems - rowabs);
}

bool QJsonTreeItem::setColumnHeaders (const QString& columns)
{
  m_headers.clear();

  // split columns string
  QStringList l = columns.split(",");
  if (l.isEmpty())
  {
    // something wrong, "_headers_" empty/missing
    return false;
  }
  m_headersCount = l.count();

  foreach (QString s, l)
  {
    QStringList sl = s.split(":");
    QHash <QString,QVariant>h;

    if (sl.count() < 3)
    {
      // something wrong in the "_headers_" tag,
      return false;
    }

    // save name, jsontag and header idx in an hash
    h["__name__"]=sl.at(0);
    h["__tag__"]=sl.at(1);
    h["__idx__"]=sl.at(2).toInt();
    if (sl.count() == 4)
    {
      // this column item need to be drawn. there must be an option at the corresponding item map with i.e. tag=QPushButton,buttontext
      h["__draw__"]=(sl.at(3).compare("draw",Qt::CaseInsensitive) == 0) ? true:false;
    }

    // use the same values as keys in the parent hash
    m_headers[sl.at(0)] = h;
    m_headers[sl.at(1)] = h;
    m_headers[sl.at(2)] = h;
  }
  return true;
}

const QString QJsonTreeItem::headerNameOrTagByInt(int column, bool returntag) const
{
  QHash <QString, QVariant> h = headerHashByIdx(column);
  if (h.isEmpty())
    return QString();

  if (returntag)
    return h["__tag__"].toString();
  return h["__name__"].toString();
}

const QString QJsonTreeItem::headerNameOrTagByString (const QString &name, bool returntag, int* column) const
{
  if (column)
    *column = -1;

  QHash <QString, QVariant> h = headerHashByName(name);
  if (h.isEmpty())
    return QString();

  if (column)
    *column = h["__idx__"].toInt();

  if (returntag)
    return h["__tag__"].toString();
  return h["__name__"].toString();
}

const QString QJsonTreeItem::headerTagByIdx(int column) const
{
  return headerNameOrTagByInt(column,true);
}

const QString QJsonTreeItem::headerTagByName(const QString &name, int* column) const
{
  return headerNameOrTagByString(name,true,column);
}

const QString QJsonTreeItem::headerNameByIdx(int column) const
{
  return headerNameOrTagByInt(column,false);
}

const QString QJsonTreeItem::headerNameByTag(const QString &tag, int *column) const
{
  return headerNameOrTagByString(tag,false,column);
}

int QJsonTreeItem::headerIdxByTag(const QString &tag) const
{
  QHash <QString, QVariant> h = m_headers.value(tag,QHash<QString,QVariant>());
  if (h.isEmpty())
    return -1;
  return h["__idx__"].toInt();
}

int QJsonTreeItem::headerIdxByName(const QString &name) const
{
  return headerIdxByTag(name); // since we have the name too as key in the parent hash, this will work
}

int QJsonTreeItem::row() const
{
  // if there's a parent item, this item's corresponding row is taken from the childs index of its parent. either, its the 1st row (row 0, this is a parent item)
  QJsonTreeItem* parent = this->parent();
  if (parent)
    return (parent->children().indexOf(const_cast<QJsonTreeItem*>(this)));
  return 0;
}

int QJsonTreeItem::rowAbsolute() const
{
  if (!this->parent())
  {
    // this is the tree root item, row 0
    return 0;
  }

  // this item relative row
  int n = row();
  QJsonTreeItem* parent = this->parent()->parent();
  while (parent)
  {
    // add this parent's parent childs
    n+=parent->childCount();
    parent = parent->parent();
  }
  return n;
}

void QJsonTreeItem::clear()
{
  m_headers.clear();
  m_totalTreeItems = 0;
  m_headersCount = 0;
  m_parentItem = 0;
  m_fetchedChildren = false;
  m_error = QJsonTreeItem::JsonNoError;
  m_map.clear();
  qDeleteAll(m_childItems);
  m_childItems.clear();
}

void QJsonTreeItem::buildWidgetFlags()
{
  // populate the hash with flags appropriate for the item, this will be read by the model
  QJsonTreeItem::widgetFlags["Tree"] = (Qt::NoItemFlags);
  QJsonTreeItem::widgetFlags["QCheckBox"]=(Qt::ItemIsEditable|Qt::ItemIsUserCheckable);
  QJsonTreeItem::widgetFlags["QComboBox"]=(Qt::ItemIsEditable);
  QJsonTreeItem::widgetFlags["QLineEdit"]=(Qt::ItemIsEditable);
  QJsonTreeItem::widgetFlags["QPushButton"]= (Qt::NoItemFlags);
  QJsonTreeItem::widgetFlags["QSpinBox"]=(Qt::ItemIsEditable);
}

void QJsonTreeItem::setMapValue(int column, const QVariant &value)
{
  QString tag = headerTagByIdx(column);
  setMapValue(tag,value);
}

const QVariantMap QJsonTreeItem::toMap(int depth, QVariantMap intmap, QJsonTreeItem* item) const
{
  const QJsonTreeItem* it;
  if (depth == 0)
  {
    // first call
    intmap = m_map;
    it = this;
  }
  else
  {
    it = item;
    intmap = it->map();
  }

  // this was added for optimization when building the tree
  intmap.remove("__hasROSet__");

  if (it->childCount() > 0)
  {
    QVariantList l;
    foreach (QJsonTreeItem* i, it->children())
    {
      // recurse
      depth++;
      l.append(i->toMap(depth,intmap,i));
      depth--;
    }

    // add list as child
    intmap["_children_"] = l;
  }

  return intmap;
}
