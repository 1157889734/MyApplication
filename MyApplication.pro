#-------------------------------------------------
#
# Project created by QtCreator 2021-07-21T01:26:24
#
#-------------------------------------------------

QT       += core gui widgets virtualkeyboard multimedia multimediawidgets sql

TARGET = MyApplication
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

CONFIG += c++11

SOURCES += \
        main.cpp \
    MyApplication.cpp \
    choicelogindevwidget.cpp \
    loginwidget.cpp \
    pvmsmenuwidget.cpp \
    pvmsmonitorwidget.cpp \
    recordplaywidget.cpp \
    inteanalywidget.cpp \
    devmanagewidget.cpp \
    devupdatewidget.cpp \
#    pmsgcli.c
    alarmwidget.cpp \
    fileConfig.cpp \
    state.cpp \
#    timeset.cpp \
    gb2312_utf8.c \
    log.c \
    pmsgcli.c \
    presetpasswdconfirm.cpp \
    myslider.cpp \
    qplayer.cpp \
#    useredit.cpp \
#    useradd.cpp \
#    usermanage.cpp

HEADERS += \
    MyApplication.h \
    choicelogindevwidget.h \
    loginwidget.h \
    pvmsmenuwidget.h \
    pvmsmonitorwidget.h \
    recordplaywidget.h \
    inteanalywidget.h \
    devmanagewidget.h \
    devupdatewidget.h \
#    pmsgcli.h
    alarmwidget.h \
#    debug.h \
    fileConfig.h \
    gb2312_utf8.h \
    log.h \
    pmsgcli.h \
    state.h \
#    timeset.h \
    types.h \
    presetpasswdconfirm.h \
    myslider.h \
    qplayer.h \
#    useredit.h \
#    useradd.h \
#    usermanage.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc

FORMS += \
    choicelogindevwidget.ui \
    loginwidget.ui \
    pvmsmenuwidget.ui \
    pvmsmonitorwidget.ui \
    recordplaywidget.ui \
    inteanalywidget.ui \
    devmanagewidget.ui \
    devupdatewidget.ui \
    alarmwidget.ui \
    presetpasswdconfirm.ui \
    useredit.ui \
    useradd.ui \
    usermanage.ui

