#-------------------------------------------------
#
# Project created by QtCreator 2012-02-10T10:52:16
#
#-------------------------------------------------

TARGET = qjsontreewidget
TEMPLATE = lib
DEFINES += QJSONTREEWIDGET_LIBRARY
CONFIG += create_prl

DESTDIR  =lib

SOURCES += qjsontreewidget.cpp \
    qjsontreemodel.cpp \
    qjsontreeitem.cpp \
    qjsontreeitemdelegate.cpp \
    qjsonsortfilterproxymodel.cpp

HEADERS += qjsontreewidget.h \
    qjsontreemodel.h \
    qjsontreeitem.h \
    qjsontreeitemdelegate.h \
    qjsonsortfilterproxymodel.h

INCLUDEPATH += ../qjson/include
