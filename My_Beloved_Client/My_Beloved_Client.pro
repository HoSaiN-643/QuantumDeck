QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    change_profile.cpp \
    choose_mode.cpp \
    client.cpp \
    history.cpp \
    log.cpp \
    login.cpp \
    main.cpp \
    connect.cpp \
    mainmenu.cpp \
    player.cpp \
    recoverpass.cpp \
    signup.cpp \
    start.cpp \
    InputValidator.cpp

HEADERS += \
    change_profile.h \
    choose_mode.h \
    client.h \
    connect.h \
    history.h \
    log.h \
    login.h \
    mainmenu.h \
    player.h \
    recoverpass.h \
    signup.h \
    start.h \
    InputValidator.h

FORMS += \
    change_profile.ui \
    choose_mode.ui \
    connect.ui \
    history.ui \
    log.ui \
    login.ui \
    mainmenu.ui \
    recoverpass.ui \
    signup.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
