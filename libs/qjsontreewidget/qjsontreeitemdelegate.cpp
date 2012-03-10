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
    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "qjsontreeitemdelegate.h"

QJsonTreeItemDelegate::QJsonTreeItemDelegate(QObject *parent) :
  QStyledItemDelegate(parent)
{
  // initialize hash used to speedup paint() function
  buildWidgetTypes();
}

QWidget *QJsonTreeItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  QJsonTreeModel* model = (QJsonTreeModel*)index.model();

  // get tag for this index
  QJsonTreeItem* item;
  QString tag = model->tagByModelIndex(index,&item);
  if (tag.isEmpty())
    return 0;

  // get widget string
  QString w = item->map().value("_widget_:" % tag,QString()).toString();
  if (w.isEmpty())
    return 0;

  // get value and create the widget
  if (w.compare("QCheckBox",Qt::CaseInsensitive) == 0)
  {
    // create a checkbox
    QCheckBox* w = new QCheckBox(parent);
    return w;
  }
  else if (w.compare("QSpinBox",Qt::CaseInsensitive) == 0)
  {
    // create a spinbox
    QSpinBox* w = new QSpinBox(parent);
    w->setMinimum(item->map().value("_valuemin_:" % tag,0).toInt());
    w->setMaximum(item->map().value("_valuemax_:" % tag,0).toInt());
    return w;
  }
  else if (w.compare("QComboBox",Qt::CaseInsensitive) == 0)
  {
    // create a combobox
    QComboBox* w = new QComboBox(parent);
    QString ls = item->map().value("_valuelist_:" % tag,QStringList()).toString();
    if (!ls.isEmpty())
    {
      // set values
      QStringList l = ls.split(",");
      if (!l.isEmpty())
        w->addItems(l);
    }
    return w;
  }
  else if (w.compare("QLineEdit",Qt::CaseInsensitive) == 0)
  {
    // create a lineedit
    QLineEdit* w = new QLineEdit(parent);

    // check if we have a regexp set
    QString re = item->map().value("_regexp_:" % tag,QString()).toString();
    if (!re.isEmpty())
    {
      // set a validator
      QRegExp rx(re);
      QRegExpValidator* v = new QRegExpValidator(rx,w);
      w->setValidator(v);
    }
    return w;
  }
  return 0;
}

void QJsonTreeItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
  QVariant val = index.model()->data(index,Qt::DisplayRole);
  QString n (editor->metaObject()->className());

  if (n.compare("QCheckBox",Qt::CaseInsensitive) == 0)
  {
    QCheckBox* w = static_cast<QCheckBox*>(editor);
    w->setChecked(val.toBool());
  }
  else if (n.compare("QSpinBox",Qt::CaseInsensitive) == 0)
  {
    QSpinBox* w = static_cast<QSpinBox*>(editor);
    w->setValue(val.toInt());
  }
  else if (n.compare("QComboBox",Qt::CaseInsensitive) == 0)
  {
    QComboBox* w = static_cast<QComboBox*>(editor);
    int idx = w->findText(val.toString(),Qt::MatchExactly);
    if (idx != -1)
      w->setCurrentIndex(idx);
  }
  else if (n.compare("QLineEdit",Qt::CaseInsensitive) == 0)
  {
    QLineEdit* w = static_cast<QLineEdit*>(editor);
    w->setText(val.toString());
  }
}

void QJsonTreeItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
  QString n (editor->metaObject()->className());

  if (n.compare("QCheckBox",Qt::CaseInsensitive) == 0)
  {
    QCheckBox* w = static_cast<QCheckBox*>(editor);
    model->setData(index,w->isChecked(),Qt::EditRole);
  }
  else if (n.compare("QSpinBox",Qt::CaseInsensitive) == 0)
  {
    QSpinBox* w = static_cast<QSpinBox*>(editor);
    model->setData(index,w->value(),Qt::EditRole);
  }
  else if (n.compare("QComboBox",Qt::CaseInsensitive) == 0)
  {
    QComboBox* w = static_cast<QComboBox*>(editor);
    model->setData(index,w->currentText(),Qt::EditRole);
  }
  else if (n.compare("QLineEdit",Qt::CaseInsensitive) == 0)
  {
    QLineEdit* w = static_cast<QLineEdit*>(editor);
    QRegExpValidator* v = (QRegExpValidator*)w->validator();
    if (!v)
    {
      // no validator
      model->setData(index,w->text(),Qt::EditRole);
      return;
    }

    // discard anything not satisfying the regexp
    int pos = 0;
    QString t = w->text();
    if (v->validate(t,pos) == QValidator::Acceptable)
      model->setData(index,w->text(),Qt::EditRole);
  }
}

void QJsonTreeItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  QJsonTreeModel* model = (QJsonTreeModel*)index.model();
  QJsonTreeItem* it;
  QVariantMap m = model->mapByModelIndex(index,&it);
  if (m.isEmpty())
  {
    QStyledItemDelegate::paint(painter,option,index);
    return;
  }

  // get headers hash
  QHash<QString,QVariant> h = it->headerHashByIdx(index.column());

  // we always paint the checkbox
  QString w = m.value("_widget_:" % h["__tag__"].toString(),QString()).toString();
  QStyle::ControlElement ce = m_widgetTypes.value(w,(QStyle::ControlElement)-1);
  if (ce == QStyle::CE_CheckBox)
  {
    // draw a checkbox
    drawButton(option,painter,QStyle::CE_CheckBox,QString(),m.value(h["__tag__"].toString(),false).toBool());
    return;
  }

  // check if we need to paint something in this column
  if (!h["__draw__"].toBool())
  {
    QStyledItemDelegate::paint(painter,option,index);
    return;
  }

  // check if the item has the corresponding tag
  QString tagval = m.value(h["__tag__"].toString(),QVariant()).toString();
  if (tagval.isEmpty())
  {
    QStyledItemDelegate::paint(painter,option,index);
    return;
  }

  // see what we have to draw
  int idx=tagval.indexOf(",");
  if (idx == -1)
  {
    QStyledItemDelegate::paint(painter,option,index);
    return;
  }
  w = tagval.mid(0,idx);
  ce = m_widgetTypes.value(w,(QStyle::ControlElement)-1);
  if (ce == QStyle::CE_PushButton)
  {
      // draw a pushbutton
      drawButton(option,painter,QStyle::CE_PushButton,tagval.mid(idx+1));
      return;
  }
}

void QJsonTreeItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  editor->setGeometry(option.rect);
}

bool QJsonTreeItemDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
  QMouseEvent* me;
  switch (event->type())
  {
    case QEvent::MouseButtonPress:
      me = (QMouseEvent*)event;
      switch (me->button())
      {
        case Qt::LeftButton:
          // left button pressed
          handleLeftMousePress(model,index);
        break;
        case Qt::RightButton:
          // right mouse pressed
          handleRightMousePress(me,model,index);
        break;
        default:
          break;
      }
    break;

    default:
      break;
  }
  // we always return false to let the selection model do its work, without eating the event
  return false;
}

void QJsonTreeItemDelegate::drawButton(const QStyleOptionViewItem &option, QPainter* painter, const QStyle::ControlElement type, const QString &text, bool checked) const
{
  // copy the options from the provided one
  QStyleOptionButton opts;
  opts.features = QStyleOptionButton::None;
  opts.rect = option.rect;
  opts.direction = option.direction;
  opts.state = option.state;
  checked ? opts.state |= QStyle::State_On : opts.state |= QStyle::State_Off;
  opts.text = text;
  QApplication::style()->drawControl(type,&opts,painter);
}

void QJsonTreeItemDelegate::buildWidgetTypes()
{
  m_widgetTypes["QPushButton"] = QStyle::CE_PushButton;
  m_widgetTypes["QCheckBox"] = QStyle::CE_CheckBox;
}

void QJsonTreeItemDelegate::handleLeftMousePress(QAbstractItemModel* model, const QModelIndex &index)
{
  // get item
  QJsonTreeItem* item;
  QJsonTreeModel* md = (QJsonTreeModel*)model;
  QVariantMap m = md->mapByModelIndex(index,&item);
  if (m.isEmpty())
    return;

  // get stuff from item
  QString tag = item->headerTagByIdx(index.column());
  QVariant val = m.value(tag,QVariant());

  // check if there's hide or readonly set for this column
  if ((m.value("_readonly_:" % tag, false).toBool() == true) && (md->specialFlags() & QJsonTreeItem::HonorReadOnly))
    return;

  // emit the generic signal
  emit clicked(index.column(),index.row(),tag,val,item);

  // if it's a button, emit the button clicked signal too. this is just a shortcut for the above signal
  if (val.toString().startsWith("QPushButton,",Qt::CaseInsensitive) && !val.isNull())
  {
    emit clicked(item,tag);
  }
}

void QJsonTreeItemDelegate::handleRightMousePress(QMouseEvent* event, QAbstractItemModel* model, const QModelIndex &index)
{
  QJsonTreeModel* md = (QJsonTreeModel*)model;
  QJsonTreeItem* item;
  QVariantMap m = md->mapByModelIndex(index,&item);
  if (!item)
    return;

  QString name = m["name"].toString();
  bool canadd = true;

  // get templates this item supports
  QList<QJsonTreeItem*> templates = templatesByItem(item);
  if (templates.isEmpty())
  {
    // check if this item parent has a template for this item
    QJsonTreeItem* tp = templateByName(item,name);
    if (!tp)
      return;
    templates.append(tp);
    canadd = false;
  }

  // create a popup menu
  QMenu* menu = new QMenu();
  foreach (QJsonTreeItem* t, templates)
  {
    // check if this item can be added (coming from item templates)
    if (canadd)
    {
      QAction* action = new QAction(tr("Add '") % t->map()["name"].toString() % "'",menu);
      action->setData(t->toMap());
      menu->addAction(action);
    }
    else
    {
      // check if this item is mandatory and cannot be removed
      bool enableaction = true;
      if (t->map().value("_mandatory_",false).toBool() == true)
      {
        if (countParentChildsByName(item,name) == 1)
          enableaction = false;
      }

      // not mandatory, can remove
      QAction* action = new QAction(tr("Remove '") % t->map()["name"].toString() % "'",menu);
      action->setData(QVariant(true));
      action->setEnabled(enableaction);
      menu->addAction(action);
    }
  }

  // finally exec the menu
  execMenu(md, index, item, menu, event->globalPos());
  delete menu;
}

void QJsonTreeItemDelegate::execMenu(QJsonTreeModel* model, const QModelIndex& index, QJsonTreeItem* item, QMenu *menu, const QPoint& pos) const
{
  QAction* action = menu->exec(pos);
  if (action)
  {
    if (action->data().canConvert(QVariant::Bool)) // we've set this before
    {
      // we must remove
      model->removeRow(index.row(),index.parent());
    }
    else
    {
      // we must insert, we've stored a map at action->data
      QVariantMap mm = action->data().toMap();
      mm.remove("_template_");
      mm.remove("_mandatory_");
      model->insertRow(item->childCount(),index);
      QModelIndex newidx = model->index(item->childCount() - 1,0,index);
      model->setData(newidx,mm);
    }
  }
}

QList<QJsonTreeItem*> QJsonTreeItemDelegate::templatesByItem(QJsonTreeItem* item) const
{
  QList<QJsonTreeItem*> l;
  foreach (QJsonTreeItem* c, item->children())
  {
    if (c->map().value("_template_",false).toBool() == true)
    {
      l.append(c);
    }
  }
  return l;
}

QJsonTreeItem* QJsonTreeItemDelegate::templateByName(QJsonTreeItem* item, const QString& name) const
{
  // get parent templates
  QJsonTreeItem* parent = item->parent();
  if (!parent)
    return 0;

  foreach (QJsonTreeItem* c, parent->children())
  {
    if (c->map().value("_template_",false).toBool() == true)
    {
      // get template for this name
      if (c->map()["name"].toString().compare(name,Qt::CaseInsensitive) == 0)
        return c;
    }
  }
  return 0;
}

int QJsonTreeItemDelegate::countParentChildsByName(QJsonTreeItem* item, const QString& name) const
{
  int count = 0;

  // get parent templates
  QJsonTreeItem* parent = item->parent();
  if (!parent)
    return count;

  foreach (QJsonTreeItem* c, parent->children())
  {
    // do not count templates
    if (c->map().value("_template_",false).toBool() == true)
      continue;

    // check if it has the same name
    if (c->map()["name"].toString().compare(name,Qt::CaseInsensitive) == 0)
      count++;
  }
  return count;
}
