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

#ifndef QJSONTREEITEM_H
#define QJSONTREEITEM_H

#include <QtCore>

class QJsonTreeModel;
class QJsonTreeWidget;
class QTreeView;
class QJsonSortFilterProxyModel;

/**
 * @brief class to represent a tree item from a JSON object coming from QJsonTreeModel
 *
 */
 class QJsonTreeItem
 {
   friend class QJsonTreeModel;
   friend class QJsonTreeWidget;
   friend class QJsonSortFilterProxyModel;

   public:

   /**
    * @brief  internal errors returned when building a QJsonTreeItem
    *
    */
   enum JsonMapErrors {
     JsonNoError = 0,
     JsonInvalidMap = 1,
     JsonMissingOrInvalidHeaders = 2
   };

   /**
    * @brief various item special flags
    *
    */
   enum SpecialFlag {
     HonorReadOnly = 1,
     HonorHide = 2,
     ReadOnlyHidesRow = 4
   };
   Q_DECLARE_FLAGS (SpecialFlags, SpecialFlag)

/**
  * @brief constructor
  *
  * @param tree : the QJsonTreeWidget this item belongs to
  * @param parent : the item parent (optional)
  * @param map: the item data as a map coming from QJson (optional)
  */
   QJsonTreeItem (QJsonTreeWidget* tree, QJsonTreeItem *parent = 0, const QVariantMap &map = QVariantMap());

   /**
    * @brief fills an item from a QJson map, replacing previous content
    *
    * @param map: the item data as a map coming from QJson
    * @param parent : the item parent (optional)
    * @return bool
    */
   bool fromMap(const QVariantMap &map, QJsonTreeItem *parent = 0);

   /**
    * @brief destructor
    *
    */
   ~QJsonTreeItem();

   /**
    * @brief adds a child node to this item
    *
    * @param child the child item
    */
   void appendChild(QJsonTreeItem *child);

   /**
    * @brief returns the item at the specified row in the tree view
    *
    * @param row the row number
    * @return QJsonTreeItem
    */
   QJsonTreeItem* child(int row) const { return m_children.value(row); }

   /**
    * @brief removes the specified child from the internal list. the corresponding QJsonTreeItem* is deleted too.
    *
    * @param row the row number (= the nth child)
    */
   void removeChild (int row);

   /**
    * @brief returns this item's children
    *
    * @return QList<QJsonTreeItem *>
    */
   QList<QJsonTreeItem*> children() const { return m_children; }

   /**
    * @brief returns whether this item has children or not
    *
    * @return bool
    */
   bool hasChildren() const { return !m_children.isEmpty(); }

   /**
    * @brief returns the number of childs this item has (= the number of rows for the parent item)
    *
    * @return int
    */
   int childCount() const { return m_children.count(); }

   /**
    * @brief returns the number of childs (recursive) this item has
    *
    * @return int
    */
   int totalChildCount() const;

   /**
    * @brief get the total number of items the root item has
    *
    * return int
    */
   int totalTreeItems() const { return m_root->m_totalTreeItems; }

   /**
    * @brief returns the number of tree columns this item is represented on (fixed, from JSON root map "_headers_" stored in the root item's columns descriptor)
    *
    * @return int
    */
   int columnCount() const { return m_headersCount; }

   /**
    * @brief returns columns description (with name,jsontag,idx).
    * Note that this always returns a pointer to rootitem's headers descriptor (m_headers), since this is stored in the root only for optimization
    *
    * @return QList<QHash<QString, QVariant> >
    */
   const QHash<QString, QHash<QString, QVariant> > headers() const { return m_headers; }

   /**
    * @brief sets the whole internal item map for this item (replace the JSON object)
    *
    * @param map the new item map
    */
   void setMap (const QVariantMap& map) { m_map = map; }

   /**
    * @brief returns the whole internal map for this item
    *
    */
   const QVariantMap map() const { return m_map; }

   /**
    * @brief sets a new value in the internal item map
    *
    * @param tag the JSON tag name
    * @param value the new value
    * @param applyto parameter for the tag (optional)
    */
   void setMapValue (const QString& tag, const QVariant& value) { m_map[tag] = value;}

   /**
    * @brief sets a new value in the internal item map
    *
    * @param int the header column index
    * @param value the new value
    * @param applyto parameter for the tag (optional)
    */
   void setMapValue (int column, const QVariant& value);

   /**
    * @brief recursively rebuilds the JSON map from the tree structure
    *
    * @param depth internal (always 0)
    * @param intmap internal (always 0)
    * @param item internal (always 0)
    * @return QVariantMap
    */
   const QVariantMap toMap(int depth=0, QVariantMap intmap = QVariantMap(), QJsonTreeItem* item = 0) const;

   /**
    * @brief returns the row at which this item is in the tree view, relative to its parent if any
    *
    * @return int
    */
   int row() const;

   /**
    * @brief returns the row at which this item is in the tree view, relative to the main root item
    *
    * @return int
    */
   int rowAbsolute() const;

   /**
    * @brief returns this item's parent
    *
    * @return QJsonTreeItem
    */
   QJsonTreeItem* parent() const { return m_parent; }

   /**
    * @brief returns whether this item has parent or not (if not, this is the root item)
    *
    * @return bool
    */
   bool hasParent() const { return (m_parent == 0 ? false : true); }

   /**
    * @brief returns the tree root item
    *
    * @return QJsonTreeItem
    */
   QJsonTreeItem* rootItem() const { return m_root; }

   /**
    * @brief returns wether the item has been constructed succesfully or not
    *
    * @return bool
    */
   bool isValid() const { return (m_error == QJsonTreeItem::JsonNoError); }

   /**
    * @brief returns an error defined in JsonMapErrors
    *
    * @return JsonMapErrors
    */
   QJsonTreeItem::JsonMapErrors error() const { return m_error; }

   /**
    * @brief this is a convenience function which returns the map causing the error.
    * this will be useful usually only through rootItem()->invalidMap() in case of error during loadJson() from the model
    *
    * @return QVariantMap
    */
   QVariantMap invalidMap() const { return m_invalidMap; }

   /**
    * @brief completely clears the item deleting all its child items too. The item can then be reused by calling fromMap()
    *
    */
   void clear();

   /**
    * @brief returns header (column) displayed from the root item's header descriptor providing the column index
    *
    * @param column the column index
    * @return const QString
    */
   const QString headerNameByIdx(int column) const;

   /**
    * @brief returns header (column) displayed name from the root item's header descriptor providing the corresponding JSON tag
    *
    * @param tag the column JSON tag
    * @param column on return, the corresponding header index (optional)
    * @return const QString
    */
   const QString headerNameByTag (const QString& tag, int* column=0) const;

   /**
    * @brief returns header (column) tag (= JSON tag) from the root item's header descriptor providing the column index
    *
    * @param column the column index
    * @return const QString
    */
   const QString headerTagByIdx (int column) const;

   /**
    * @brief returns header (column) tag (= JSON tag) from the root item's header descriptor providing the header displayed name
    *
    * @param name the column displayed name
    * @param column on return, the corresponding header index (optional)
    * @return const QString
    */
   const QString headerTagByName (const QString& name, int* column=0) const;

   /**
    * @brief returns header (column) index from the root item's header descriptor providing the corresponding JSON tag
    *
    * @param tag the column JSON tag
    * @return const QString
    */
   int headerIdxByTag(const QString &tag) const;

   /**
    * @brief returns header (column) index from the root item's header descriptor providing the header displayed name
    *
    * @param name the column displayed name
    * @return const QString
    */
   int headerIdxByName(const QString &name) const;

   /**
    * @brief returns header (column) hash from the root item's header descriptor providing the column index
    *
    * @param column the column index
    * @return const QHash<QString, QVariant>
    */
   const QHash<QString, QVariant> headerHashByIdx (int column) const { return m_headers.value(QVariant(column).toString(),QHash<QString,QVariant>()); }

   /**
    * @brief returns header (column) hash from the root item's header descriptor providing the corresponding JSON tag. due to the nature the hash is stored, this is equivalent to call headerHashByName()
    *
    * @param tag the column JSON tag
    * @return const QHash<QString, QVariant>
    */
   const QHash<QString, QVariant> headerHashByTag (const QString& tag) const { return m_headers.value(tag,QHash<QString,QVariant>()); }

   /**
    * @brief returns header (column) hash from the root item's header descriptor providing the header displayed name. due to the nature the hash is stored, this is equivalent to call headerHashByTag()
    *
    * @param name the column displayed name
    * @return const QHash<QString, QVariant>
    */
   const QHash<QString, QVariant> headerHashByName (const QString& name) const { return m_headers.value(name,QHash<QString,QVariant>()); }

   /**
    * @brief returns the QJsonTreeWidget this item belongs to
    *
    * @return QJsonTreeWidget
    */
   QJsonTreeWidget* widget() const { return m_widget; }

   /**
    * @brief returns the text at the specified column
    *
    * @param idx the column index
    * @return QString
    */
   QString text(int column) const;

 protected:
   static void buildWidgetFlags();
   QJsonTreeModel* model();
   QTreeView *view();
   static QHash<QString, Qt::ItemFlags> widgetFlags;
   bool setColumnHeaders(const QString &headers);
   const QString headerNameOrTagByString(const QString &name, bool returntag, int *column) const;
   const QString headerNameOrTagByInt(int column, bool returntag) const;

 private:
   QJsonTreeItem::JsonMapErrors m_error;
   QModelIndex m_index;
   QList<QJsonTreeItem*> m_children;
   QVariantMap m_map;
   QVariantMap m_invalidMap;
   QJsonTreeItem* m_parent;
   QJsonTreeItem* m_root;
   QJsonTreeWidget* m_widget;
   int m_headersCount; // m_headers.count() to return number of columns wouldnt work, since how we store data in such hash
   int m_totalTreeItems;

   QHash<QString, QHash<QString, QVariant> > m_headers;
 };

 Q_DECLARE_OPERATORS_FOR_FLAGS(QJsonTreeItem::SpecialFlags)

#endif // QJSONTREEITEM_H
