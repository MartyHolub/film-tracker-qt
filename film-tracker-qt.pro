QT += core gui widgets

CONFIG += c++17

TARGET = film-tracker-qt
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    filmdialog.cpp \
    statsdialog.cpp

HEADERS += \
    mainwindow.h \
    filmdialog.h \
    statsdialog.h
