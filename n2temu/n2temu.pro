#-------------------------------------------------
#
# Project created by QtCreator 2017-01-15T21:02:42
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = n2temu
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    videowidget.cpp \
    cpu.cpp \
    memory.cpp \
    emu.cpp \
    rom16loader.cpp \
    codetablemodel.cpp \
    memtablemodel.cpp \
    disassembler.cpp

HEADERS  += mainwindow.h \
    videowidget.h \
    cpu.h \
    memory.h \
    emu.h \
    rom16loader.h \
    codetablemodel.h \
    memtablemodel.h \
    disassembler.h

FORMS    += mainwindow.ui

DISTFILES += \
    icons/folder_documents.png \
    icons/1rightarrow.png \
    icons/2leftarrow.png \
    icons/2rightarrow.png \
    icons/stop.png

RESOURCES += \
    resources.qrc
