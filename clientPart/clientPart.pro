QT += core gui network widgets

TEMPLATE = app
TARGET = client

MOC_DIR     += generated/mocs
UI_DIR      += generated/uis
RCC_DIR     += generated/rccs
OBJECTS_DIR += generated/objs

SOURCES += main.cpp\
    form.cpp \
        mainwindow.cpp \
    clientStuff.cpp

HEADERS  += mainwindow.h \
    clientStuff.h \
    form.h

FORMS    += mainwindow.ui \
    form.ui
