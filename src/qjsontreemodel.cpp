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

#include "qjsontreemodel.h"

QJsonTreeModel::QJsonTreeModel(QObject *parent) :
  QAbstractItemModel(parent)
{
  m_root = 0;
  m_parser = new QJson::Parser();
  m_serializer = new QJson::Serializer();
  this->clear();
}

QJsonTreeModel::~QJsonTreeModel()
{
  this->clear();
  delete m_parser;
  delete m_serializer;
}

bool QJsonTreeModel::buildModel(const QVariantMap& map)
{
  if (map.isEmpty())
  {
      setNotFoundInvalidOrEmptyError("buildModel","map");
      return false;
  }

  // delete the existing tree and reset the model
  this->clear();
  m_serializer->setIndentMode(QJson::IndentNone);

  int v = jsonVersion(map);
  if (v == -1)
  {
    setNotFoundInvalidOrEmptyError("buildModel","_version_");
    return false;
  }
  if (v > m_maxVersion)
  {
    m_error = tr("buildModel: Unsupported JSON version: %1, maxversion: %2").arg(v).arg(m_maxVersion);
    return false;
  }

  QVariantMap maptouse = map;
  QVariantMap blob = map.value("_blob_",QVariantMap()).toMap();
  if (!blob.isEmpty())
  {
      // we use blob instead (for embedded trees into another map)
      maptouse = blob;
  }

  // create the root item. the root item is invisible, we use it only to store the headers hash.
  // so we need only _headers_ in it
  QString hdrstring = maptouse.value("_headers_",QString()).toString();
  if (hdrstring.isEmpty())
  {
    setNotFoundInvalidOrEmptyError("buildModel","_headers_");
    return false;
  }

  QVariantMap m;
  m["_headers_"]=hdrstring;
  m_root = new QJsonTreeItem(0,m);
  if (!m_root->isValid())
  {
    // something wrong with the invisible root item (probably header)
    QByteArray invalid = m_serializer->serialize(m_root->invalidMap());
    setNotFoundInvalidOrEmptyError("buildModel",invalid);
    this->clear();
    return false;
  }

  QJsonTreeItem* r = new QJsonTreeItem(m_root,maptouse); // this is the real root, 1st child of invisibleroot
  if (!m_root->isValid())
  {
    // something wrong with the real root item (probably header)
    QByteArray invalid = m_serializer->serialize(m_root->invalidMap());
    setNotFoundInvalidOrEmptyError("buildModel",invalid);
    this->clear();
    return false;
  }

  m_root->appendChild(r);
  return true;
}

bool QJsonTreeModel::loadJson(const QString &path)
{
  QFile file(path);
  file.open(QIODevice::ReadOnly);
  bool b = loadJson(file);
  file.close();
  return b;
}

bool QJsonTreeModel::loadJson(QIODevice &dev)
{
  return loadJson(dev.readAll());
}

bool QJsonTreeModel::loadJson(const QByteArray &buf)
{
  if (buf.isEmpty())
  {
    setNotFoundInvalidOrEmptyError("loadJson","buf");
    return false;
  }

  // parse
  bool ok;
  QVariantMap map = m_parser->parse(buf,&ok).toMap();
  if (!ok)
  {
      m_error = tr("loadJson: JSON parser error: line %1, %2").arg(QVariant(m_parser->errorLine()).toString()).arg(m_parser->errorString());
      return false;
  }

  return loadJson(map);
}

bool QJsonTreeModel::loadJson(const QVariantMap &map)
{
    return buildModel(map);
}

bool QJsonTreeModel::saveJson(const QString &path, QJson::IndentMode indentmode, const QVariantMap& additional)
{
  QFile file(path);
  bool b = file.open(QIODevice::WriteOnly);
  b = saveJson(file,indentmode,additional);
  file.close();
  return b;
}

bool QJsonTreeModel::saveJson(QIODevice &dev, QJson::IndentMode indentmode, const QVariantMap& additional)
{
  QByteArray buf = saveJson(indentmode,additional);
  if (buf.isEmpty())
  {
    setNotFoundInvalidOrEmptyError("saveJson","buf");
    return false;
  }
  qint64 sz = dev.write(buf);
  if (sz != buf.size())
  {
    m_error = tr("saveJson: error writing, requested %1, written %2, QIODevice error: %3").arg(QVariant(buf.size()).toString()).arg(QVariant(sz).toString()).arg(dev.errorString());
    return false;
  }
  return true;
}

QByteArray QJsonTreeModel::saveJson(QJson::IndentMode indentmode, const QVariantMap& additional)
{
  m_serializer->setIndentMode(indentmode);
  QVariantMap m = m_root->child(0)->toMap();
  foreach (QString key, additional.keys())
  {
      m[key]=additional[key];
  }

  return m_serializer->serialize(m);
}

void QJsonTreeModel::setNotFoundInvalidOrEmptyError(const QString &function, const QString &val)
{
  m_error=tr("%1: ERROR not found, empty or invalid:\n%2").arg(function).arg(val);
}

int QJsonTreeModel::jsonVersion(const QVariantMap map) const
{
    int v = map.value("version",-1).toInt();
    if (v == -1)
        return -1;

    return v;
}

QJsonTreeItem *QJsonTreeModel::parentItem(const QModelIndex &parent) const
{
  if (!parent.isValid())
  {
    // invalid, use root item
    return m_root;
  }
  // get from index
  return static_cast<QJsonTreeItem*>(parent.internalPointer());
}

QModelIndex QJsonTreeModel::index(int row, int column, const QModelIndex &parent) const
{
  // if we haven't an index, just return an empty index
  if (!hasIndex(row, column, parent))
    return QModelIndex();

  // get the parent item
  QJsonTreeItem* parentit = parentItem(parent);
  if (parentit == 0)
    return QModelIndex();

  // since we're referring to a child item of the parent, if it exists create an index for it
  QJsonTreeItem* child = parentit->child(row);
  if (!child)
    return QModelIndex();

  // check various item options
  QVariantMap m = child->map();
  QString tag = child->headerTagByIdx(column);
  if (m.keys().contains("__hasROSet__") && (m_specialFlags & QJsonTreeItem::ReadOnlyHidesRow) && (m_specialFlags & QJsonTreeItem::HonorHide))
  {
    // hides the whole row
    if (m["__hasROSet__"].toBool() == true)
      return QModelIndex();
  }

  bool b = m.value("_template_",false).toBool();
  if (b)
  {
    // always hide templates
    return QModelIndex();
  }

  b = m.value("_hide_",false).toBool();
  if (b && (m_specialFlags & QJsonTreeItem::HonorHide))
  {
    // hide the whole row
    return QModelIndex();
  }

  b = m.value("_hide_:" % tag,false).toBool();
  if (b && (m_specialFlags & QJsonTreeItem::HonorHide))
  {
    // hide only this value
    return QModelIndex();
  }

  return createIndex(row, column, child);
}

QModelIndex QJsonTreeModel::parent(const QModelIndex &index) const
{
  if (!index.isValid())
    return QModelIndex();

  // look for child parent
  QJsonTreeItem* child = static_cast<QJsonTreeItem*>(index.internalPointer());
   if (child == 0)
     return QModelIndex();

   QJsonTreeItem* parent = child->parent();

   if (parent == m_root)
     return QModelIndex();

   return createIndex(parent->row(), 0, parent);
}

int QJsonTreeModel::columnCount(const QModelIndex &parent) const
{
  if (m_root)
  {
    // return root item's column count (from the _columns_ descriptor)
    return m_root->columnCount();
  }
  return 0;
}

QVariant QJsonTreeModel::data(const QModelIndex &index, int role) const
{
  QJsonTreeItem* item;
  QString tag = tagByModelIndex(index,&item,role);
  if (tag.isEmpty())
    return QVariant();

  // skip custom drawn items (they could be buttons, they have usually no values... maybe we could change that later)
  QHash<QString, QHash<QString, QVariant> >  h = item->headers();
  if (h[tag].contains("__draw__"))
    return QVariant();

  // get the value that has been set in json
  return item->map().value(tag,QVariant());
}

bool QJsonTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
  QJsonTreeItem* item = itemByModelIndex(index,role);
  if (!item)
    return false;
  if (value.canConvert(QVariant::Map))
  {
    // replace the whole map
    item->fromMap(value.toMap(),item->parent());
  }
  else
  {
    // replace data at the specified JSON tag
    item->setMapValue(index.column(),value);
  }
  emit dataChanged(index,index);

  return true;
}

Qt::ItemFlags QJsonTreeModel::flags(const QModelIndex &index) const
{
  // default
  Qt::ItemFlags f = QAbstractItemModel::flags(index);

  // get tag for this index
  QJsonTreeItem* item;
  QString tag = tagByModelIndex(index,&item);
  if (tag.isEmpty())
    return f;

  // if it's readonly, remove the enabled flag
  bool b = item->map().value("_readonly_:" % tag,false).toBool();
  if (b && (m_specialFlags & QJsonTreeItem::HonorReadOnly))
  {
    f &= ~Qt::ItemIsEnabled;
  }

  // if it's a widget, get flags to apply from the Qt::ItemFlags stored in our hash
  QString w = item->map().value("_widget_:" % tag,QString()).toString();
  if (!w.isEmpty())
  {
    f |= (QJsonTreeItem::widgetFlags[w]);
  }
  return f;
}

QVariant QJsonTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role == Qt::DisplayRole)
    return m_root->headerNameByIdx(section);
  return QVariant();
}

bool QJsonTreeModel::removeRows(int row, int count, const QModelIndex &parent)
{
  QJsonTreeItem* parentit = itemByModelIndex(parent);
  if (!parentit || count == 0)
    return false;

  // this will remove rows starting from the specified row
  beginRemoveRows(parent,row,row+count-1);
  for (int i=row; i < (row+count); i++)
  {
    // this deletes the child too
    parentit->removeChild(i);
  }
  endRemoveRows();
  return true;
}

bool QJsonTreeModel::insertRows(int row, int count, const QModelIndex &parent)
{
  QJsonTreeItem* parentit = itemByModelIndex(parent);
  if (!parentit || count == 0)
    return false;

  beginInsertRows(parent,row,row+count-1);
  for (int i = 0; i < count; i++)
  {
    QJsonTreeItem* newitem = new QJsonTreeItem(parentit);
    parentit->appendChild(newitem);
  }
  endInsertRows();

  return true;
}

bool QJsonTreeModel::insertRow(int row, const QModelIndex &parent)
{
  return insertRows(row,1,parent);
}

bool QJsonTreeModel::hasChildren(const QModelIndex &parent)
{
  QJsonTreeItem* item = itemByModelIndex(parent);
  if (item && item->childCount() > 0)
    return true;
  return false;
}

int QJsonTreeModel::rowCount(const QModelIndex &parent) const
{
  // since it's a tree, consider column 0 only
  if (parent.column() > 0)
    return 0;

  QJsonTreeItem* item = parentItem(parent);
  if (item == 0)
    return 0;

  // simply return item children count (= rows under that item)
  return item->childCount();
}

bool QJsonTreeModel::canFetchMore(const QModelIndex &parent) const
{
  QJsonTreeItem* item = itemByModelIndex(parent);
  if (!item)
    return false;

  // will call fetchmore only if the item has no fetched children
  if (item->fetchedChildren() == item->childCount())
    return false;
  return true;
}

void QJsonTreeModel::fetchMore(const QModelIndex &parent)
{
  QJsonTreeItem* item = itemByModelIndex(parent);
  if (!item)
    return;
  int tofetch = qMin(m_maxRows,item->childCount() - item->fetchedChildren());
  if (tofetch == 0)
      return;

  beginInsertRows(parent,0,tofetch+1);
  item->setFetchedChildren(item->fetchedChildren()+tofetch);
  endInsertRows();
}

void QJsonTreeModel::clear()
{
  beginResetModel();
  if (m_root)
  {
    delete m_root;
    m_root = 0;
  }
  m_error.clear();
  m_maxRows = MAX_ROWS_FOR_FETCH;
  m_maxVersion = JSON_TREE_MAX_VERSION;
  m_specialFlags = (QJsonTreeItem::HonorReadOnly | QJsonTreeItem::HonorHide);
  endResetModel();
}

void QJsonTreeModel::setSpecialFlags(QJsonTreeItem::SpecialFlags flags)
{
  // special flags is updated, so we need to change the layout and inform the view
  emit layoutAboutToBeChanged();
  m_specialFlags = flags;
  emit layoutChanged();
}

QJsonTreeItem *QJsonTreeModel::itemByModelIndex(const QModelIndex &index, int role) const
{
  // check if index is valid and display role is requested
  if (!index.isValid() || (role != Qt::DisplayRole && role != Qt::EditRole && role != Qt::ToolTipRole))
    return 0;

  // get item
  return static_cast<QJsonTreeItem*>(index.internalPointer());
}

QVariantMap QJsonTreeModel::mapByModelIndex(const QModelIndex &index, QJsonTreeItem** item, int role) const
{
  QJsonTreeItem* it = itemByModelIndex(index,role);
  if (!it)
    return QVariantMap();

  if (item)
    *item = it;

  // get item map
    return it->map();
}

const QString QJsonTreeModel::tagByModelIndex (const QModelIndex &index, QJsonTreeItem** item, int role) const
{
  // get item data
  QJsonTreeItem* it;
  QVariantMap m = mapByModelIndex(index,&it,role);
  if (m.isEmpty())
    return QString();
  if (item)
    *item = it;
  return it->headerTagByIdx(index.column());
}
