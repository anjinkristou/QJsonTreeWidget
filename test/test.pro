#-------------------------------------------------
#
# Project created by QtCreator 2012-02-10T15:13:08
#
#-------------------------------------------------

QT       += core gui

TARGET = test
TEMPLATE = app

SOURCES += main.cpp\
    dialog.cpp

HEADERS  += \
    dialog.h

FORMS    += \
    dialog.ui

INCLUDEPATH += ../src \
    ../../qjson/include

LIBS += -L$$OUT_PWD/../qjsontreewidget/lib -lqjsontreewidget
LIBS += -L$$PWD/../../qjson/lib -lqjson

jsonfiles.files = *.json
jsonfiles.path = $$OUT_PWD/$(DESTDIR)
INSTALLS += jsonfiles

POST_TARGETDEPS += install

OTHER_FILES += \
    rkcfg.json \
    test.json
