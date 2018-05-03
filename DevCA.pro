# -------------------------------------------------
# Project created by QtCreator 2009-12-09T12:27:48
# -------------------------------------------------
QT += network
TARGET = DevCA
TEMPLATE = app
CONFIG += communi
SOURCES += main.cpp \
    mainwindow.cpp \
    qgridview.cpp \
    qgridtab.cpp \
    aboutdialog.cpp \
    qgridscene.cpp \
    qsidebutton.cpp \
    griddialog.cpp \
    settingsdialog.cpp \
    connectdialog.cpp \
    netdialog.cpp \
    sounddialog.cpp \
    caline.cpp \
    cagraphicsitemcreator.cpp \
    cablasttemplate.cpp \
    caconetemplate.cpp \
    camask.cpp \
    offsetdialog.cpp \
    network.cpp \
    ircnetwork.cpp
HEADERS += mainwindow.h \
    qgridview.h \
    qgridtab.h \
    aboutdialog.h \
    qgridscene.h \
    qsidebutton.h \
    griddialog.h \
    settingsdialog.h \
    connectdialog.h \
    version.h \
    netdialog.h \
    sounddialog.h \
    caline.h \
    cagraphicsitem.h \
    cagraphicsitemcreator.h \
    cablasttemplate.h \
    caconetemplate.h \
    camask.h \
    offsetdialog.h \
    network.h \
    ircnetwork.h
FORMS += mainwindow.ui \
    aboutdialog.ui \
    griddialog.ui \
    settingsdialog.ui \
    connectdialog.ui \
    netdialog.ui \
    sounddialog.ui \
    offsetdialog.ui
#win32:QTPLUGIN += qjpeg qgif
RESOURCES += DevCA.qrc
win32:RC_FILE = DevCAIcon.rc
win32:LIBS += -lwinmm -static-libgcc
MAJOR = 2
MINOR = 2
VERSION_HEADER = version.h
versiontarget.target = $$VERSION_HEADER
win32:versiontarget.commands = version.exe $$MAJOR $$MINOR $$VERSION_HEADER
versiontarget.depends = FORCE
PRE_TARGETDEPS += $$VERSION_HEADER
QMAKE_EXTRA_TARGETS += versiontarget
