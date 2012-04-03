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

#ifndef QJSONTREEITEMDELEGATE_H
#define QJSONTREEITEMDELEGATE_H

#include <QtCore>
#include <QtGui>
#include "qjsontreemodel.h"
#include "qjsonsortfilterproxymodel.h"

class QJsonTreeWidget;

/**
 * @brief class to display/edit data using the QJsonTreeModel model class
 *
 */
class QJsonTreeItemDelegate : public QStyledItemDelegate
{
  friend class QJsonTreeModel;

  Q_OBJECT
public:  
  /**
   * @brief constructor
   *
   * @param proxy the proxy model used for sorting/filtering
   */
  explicit QJsonTreeItemDelegate(QObject *parent = 0);

  /**
   * @brief destructor
   *
   */
  ~QJsonTreeItemDelegate();

   /**
     * @brief implementation of createEditor() from QAbstractItemDelegate interface
     *
     * @param parent the parent widget
     * @param option parameters to draw the widget
     * @param index the model index
     */
    QWidget* createEditor ( QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index ) const;

    /**
     * @brief implementation of setEditorData() from QAbstractItemDelegate interface
     *
     * @param editor the editor
     * @param index the model index
     */
    void	setEditorData ( QWidget * editor, const QModelIndex & index ) const;

    /**
     * @brief implementation of setModelData() from QAbstractItemDelegate interface
     *
     * @param editor the editor
     * @param model the model
     * @param index the model index
     */
    void	setModelData ( QWidget * editor, QAbstractItemModel * model, const QModelIndex & index ) const;

    /**
     * @brief implementation of paint() from QAbstractItemDelegate interface
     *
     * @param painter the painter to render the item
     * @param option the options to render the item
     * @param index the model index
     */
    void paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;

    /**
     * @brief implementation of updateEditorGeometry() from QAbstractItemDelegate interface
     *
     * @param editor the editor
     * @param option the options to render the item
     * @param index the model index
     */
    void	updateEditorGeometry ( QWidget * editor, const QStyleOptionViewItem & option, const QModelIndex & index ) const;

protected:
    /**
     * @brief implementation of editorEvent() from QAbstractItemDelegate interface
     *
     * @param event the event (mouse events, ...)
     * @param model the model
     * @param option the options to render the item
     * @param index the model index
     */
    bool editorEvent ( QEvent * event, QAbstractItemModel * model, const QStyleOptionViewItem & option, const QModelIndex & index );

signals:
    void clicked (int column, int row, const QString& jsontag, const QVariant& value, const QJsonTreeItem* item);
    void clicked (const QJsonTreeItem* item, const QString& jsontag);

private:
    QList<QJsonTreeItem *> templatesByItem(QJsonTreeItem *item) const;
    QJsonTreeItem* templateByName(QJsonTreeItem *item, const QString &name) const;
    int countParentChildsByName(QJsonTreeItem *item, const QString &name) const;
    void handleLeftMousePress(const QModelIndex &index);
    void handleRightMousePress(QMouseEvent *event, const QModelIndex &index);
    void drawButton(const QStyleOptionViewItem &option, QPainter *painter, const QStyle::ControlElement type, const QString &text=QString(), const QString& pixmap=QString(), bool checked=false) const;
    QHash<QString,QStyle::ControlElement> m_widgetTypes;
    void buildWidgetTypes();
    void execMenu(const QModelIndex &index, QJsonTreeItem *item, QMenu *menu, const QPoint &pos) const;
};

#endif // QJSONTREEITEMDELEGATE_H
