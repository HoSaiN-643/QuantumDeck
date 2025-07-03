QT += core gui network multimedia multimediawidgets
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    change_profile.cpp \
    choose_mode.cpp \
    client.cpp \
    close.cpp \
    gamewindow.cpp \
    history.cpp \
    log.cpp \
    login.cpp \
    main.cpp \
    connect.cpp \
    mainmenu.cpp \
    player.cpp \
    recoverpass.cpp \
    search_window.cpp \
    signup.cpp \
    InputValidator.cpp

HEADERS += \
    change_profile.h \
    choose_mode.h \
    client.h \
    close.h \
    connect.h \
    gamewindow.h \
    history.h \
    log.h \
    login.h \
    mainmenu.h \
    player.h \
    recoverpass.h \
    search_window.h \
    signup.h \
    InputValidator.h

FORMS += \
    change_profile.ui \
    choose_mode.ui \
    close.ui \
    connect.ui \
    gamewindow.ui \
    history.ui \
    log.ui \
    login.ui \
    mainmenu.ui \
    recoverpass.ui \
    search_window.ui \
    signup.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

# Optional: Ensure debug and release output are separate
CONFIG(debug, debug|release) {
    DESTDIR = build/debug
} else {
    DESTDIR = build/release
}
