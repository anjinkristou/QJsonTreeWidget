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
#include "qjsontreewidget.h"

QHash<QString,Qt::ItemFlags> QJsonTreeItem::widgetFlags;
QStringList QJsonTreeItem::descriptiveTags;

QJsonTreeItem::QJsonTreeItem (QJsonTreeWidget* tree, QJsonTreeItem *parent, const QVariantMap &map, bool ignoreheaders)
{
  m_headersCount = 0;
  m_widget = 0;
  m_error = QJsonTreeItem::JsonNoError;
  m_parent = parent;
  m_widget = tree;
  m_map = map;

  // this is done only once
  if (QJsonTreeItem::widgetFlags.isEmpty())
    QJsonTreeItem::buildWidgetFlags();
  if (QJsonTreeItem::descriptiveTags.isEmpty())
    QJsonTreeItem::buildDescriptiveTags();

  // just create an empty item ?
  if (map.isEmpty())
    return;

  // create item from map
  fromMap(map,parent,ignoreheaders);
}

bool QJsonTreeItem::fromMap(const QVariantMap &map, QJsonTreeItem *parent,bool ignoreheaders)
{
  m_parent = parent;
  m_map = map;

  if (parent == 0)
  {
    m_root = this;

    // this is the root item, must have "_headers_" set, store headers descriptor in m_headers
    if (!setColumnHeaders(map.value("_headers_",QString()).toString()) && !ignoreheaders)
    {
      m_error = QJsonTreeItem::JsonMissingOrInvalidHeaders;
      m_invalidMap = m_map;

      // this invalidates the root map too
      m_root->m_error = m_error;
      m_root->m_invalidMap = m_invalidMap;
      return false;
    }
  }
  else
  {
    m_headers = parent->headers();
    m_root = parent->rootItem();
    m_headersCount = parent->columnCount();
    m_totalTreeItems = parent->totalTreeItems();
  }

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
      QJsonTreeItem* i = new QJsonTreeItem(this->widget(),this,mm.toMap());
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
  qDebug() << "~QJsonTreeItem()";
  this->clear();
}

void QJsonTreeItem::appendChild(QJsonTreeItem *child)
{
  m_children.append(child);
  m_root->m_totalTreeItems++;
}

void QJsonTreeItem::removeChild(int row)
{
  QJsonTreeItem* it = this->child(row);
  m_children.removeAt(row);
  delete it;
  m_root->m_totalTreeItems--;
}

int QJsonTreeItem::totalChildCount() const
{
  int rowabs = this->rowAbsolute();
  return (m_root->m_totalTreeItems - rowabs);
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

QString QJsonTreeItem::text(int column) const
{
  QString tag = headerTagByIdx(column);
  return m_map.value(tag,QString()).toString();
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
  qDeleteAll(m_children);
  m_children.clear();
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

void QJsonTreeItem::buildDescriptiveTags()
{
  // populate the list with descriptive tags
  QJsonTreeItem::descriptiveTags.append("_widget_");
  QJsonTreeItem::descriptiveTags.append("_headers_");
  QJsonTreeItem::descriptiveTags.append("_desc_");
  QJsonTreeItem::descriptiveTags.append("_valuemin_");
  QJsonTreeItem::descriptiveTags.append("_valuemax_");
  QJsonTreeItem::descriptiveTags.append("_valuelist_");
  QJsonTreeItem::descriptiveTags.append("_regexp_");
  QJsonTreeItem::descriptiveTags.append("_readonly_");
  QJsonTreeItem::descriptiveTags.append("_hide_");
  QJsonTreeItem::descriptiveTags.append("_template_");
  QJsonTreeItem::descriptiveTags.append("_mandatory_");
}

void QJsonTreeItem::setMapValue(int column, const QVariant &value)
{
  QString tag = headerTagByIdx(column);
  setMapValue(tag,value);
}

QVariantMap QJsonTreeItem::toMap(int depth, QVariantMap intmap, QJsonTreeItem* item) const
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
  bool returnempty = false;

  // check items to be purged
  QHash<QString,bool> purgelist = it->widget()->purgeListOnSave();
  if (!purgelist.isEmpty())
  {
    foreach (QString k, intmap.keys())
    {
      if (purgelist.contains(k))
      {
        if (purgelist.value(k,false) == true)
        {
          // strip the item completely
          returnempty = true;
        }
        else
        {
          // just remove the tag
          intmap.remove(k);
        }
      }
    }
  }
  if (returnempty)
    return QVariantMap();

  // strip descriptive tags ?
  QVariantMap newmap = intmap;
  if (it->widget()->purgeDescriptiveTags())
  {
    foreach (QString k, QJsonTreeItem::descriptiveTags)
    {
      foreach (QString kk, intmap.keys())
      {
        if (kk.startsWith(k,Qt::CaseInsensitive))
        {
          newmap.remove(kk);
        }
      }
    }
  }
  intmap = newmap;

  if (it->childCount() > 0)
  {
    QVariantList l;
    foreach (QJsonTreeItem* i, it->children())
    {
      // recurse
      depth++;
      QVariantMap mm = i->toMap(depth,intmap,i);
      if (!mm.isEmpty())
      {
          l.append(mm);
      }
      depth--;
    }

    // add list as child
    intmap["_children_"] = l;
  }

  return intmap;
}

QJsonTreeModel* QJsonTreeItem::model()
{
  return m_widget->model();
}

QTreeView* QJsonTreeItem::view()
{
  return m_widget->view();
}
