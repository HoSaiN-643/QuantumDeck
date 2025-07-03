QT       += core gui sql widgets network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    PreGame.cpp \
    card.cpp \
    cardmanager.cpp \
    close.cpp \
    game.cpp \
    main.cpp \
    manager.cpp \
    memberdatabasemanager.cpp \
    server.cpp \


HEADERS += \
    PreGame.h \
    card.h \
    cardmanager.h \
    close.h \
    game.h \
    manager.h \
    memberdatabasemanager.h \
    server.h \


FORMS += \
    close.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
