#-------------------------------------------------
#
# Project created by QtCreator 2018-01-12T23:16:12
#
#-------------------------------------------------

QT       += core gui network xml

#LIBS += -L/usr/local/libxls/lib -lxlsreader
#QT += xlsx

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = gov_zakupki
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    filedownloader.cpp \
    htmltextdelegate.cpp \
    multiplepushbutton.cpp \
    filedownloadspool.cpp \
    zakupki.cpp \
    reqdocumentmanager.cpp \
    advancedtablewidget.cpp \
    internetconnectionchecker.cpp \
    common.cpp \
    requestpool.cpp \
    requestgroup.cpp \

HEADERS += \
        mainwindow.h \
    filedownloader.h \
    htmltextdelegate.h \
    multiplepushbutton.h \
    versions.h \
    filedownloadspool.h \
    zakupki.h \
    reqdocumentmanager.h \
    advancedtablewidget.h \
    internetconnectionchecker.h \
    common.h \
    requestpool.h \
    requestgroup.h \

FORMS += \
        mainwindow.ui

INCLUDEPATH += /usr/local/include/libxml2

include(3rd_party/QtXlsxWriter/src/xlsx/qtxlsx.pri)
