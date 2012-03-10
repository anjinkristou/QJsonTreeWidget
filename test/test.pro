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

INCLUDEPATH += $$PWD/../libs/qjsontreewidget \
    $$PWD/../libs/qjson

LIBS += -L$$OUT_PWD/../libs/qjsontreewidget -lqjsontreewidget
LIBS += -L$$OUT_PWD/../libs/qjson/ -lqjson

jsonfiles.files = *.json
jsonfiles.path = $$OUT_PWD

win32 {
binfiles.files = $$OUT_PWD/../libs/qjson/*.dll
binfiles.path = $$OUT_PWD
INSTALLS += binfiles
}

INSTALLS += jsonfiles

POST_TARGETDEPS += install

OTHER_FILES += \
    rkcfg.json \
    test.json

