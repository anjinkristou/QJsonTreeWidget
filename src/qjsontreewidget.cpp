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

#include "qjsontreewidget.h"

QJsonTreeWidget::QJsonTreeWidget(QWidget *parent, Qt::WindowFlags f) :
  QWidget(parent,f)
{
  // create the view and organize this widget in a vertical layout
  QBoxLayout* l= new QVBoxLayout(this);
  m_view = new QTreeView(this);
  m_view->setHeaderHidden(false);
  l->addWidget(m_view);

  // add an optional layout for the user to put its controls in
  m_optLayout = new QGridLayout(this);
  l->addLayout(m_optLayout);

  // create the model
  m_model = new QJsonTreeModel(this);

  // set the delegate on the view
  m_delegate = new QJsonTreeItemDelegate(this);
  m_view->setItemDelegate(m_delegate);

  // to update the model on edits
  connect (m_model,SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SLOT(onDataChanged(QModelIndex,QModelIndex)));

  // for mouseclicks
  connect (m_delegate,SIGNAL(clicked(const QJsonTreeItem*,QString)),this,SIGNAL(clicked(const QJsonTreeItem*,QString)));
  connect (m_delegate,SIGNAL(clicked(int,int,QString,QVariant,const QJsonTreeItem*)),this,SIGNAL(clicked(int,int,QString,QVariant,const QJsonTreeItem*)));

  // various options
  this->setContextMenuPolicy(Qt::CustomContextMenu);
}

QJsonTreeWidget::~QJsonTreeWidget()
{
  delete m_model;
  delete m_delegate;
}

bool QJsonTreeWidget::loadJson(const QString &path)
{
  if (!m_model->loadJson(path))
    return false;
  m_view->setModel(m_model);
  return true;
}

bool QJsonTreeWidget::loadJson(const QByteArray &buf)
{
  if (!m_model->loadJson(buf))
    return false;
  m_view->setModel(m_model);

  return true;
}

bool QJsonTreeWidget::loadJson(QIODevice &dev)
{
  if (!m_model->loadJson(dev))
    return false;
  m_view->setModel(m_model);
  return true;
}

bool QJsonTreeWidget::loadJson(const QVariantMap &map)
{
    if (!m_model->loadJson(map))
      return false;
    m_view->setModel(m_model);
    return true;
}

void QJsonTreeWidget::resizeColumnsToContents()
{
  for (int i=0; i < this->invisibleRootItem()->columnCount(); i++)
  {
    resizeColumnToContent(i);
  }
}

void QJsonTreeWidget::clear()
{
  m_model->clear();
}

void QJsonTreeWidget::onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
  // since we emit onDataChanged with (index,index), topLeft is enough
  m_view->update(topLeft);
}

