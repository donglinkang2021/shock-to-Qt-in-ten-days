QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    DB/groupinfo.cpp \
    DB/user.cpp \
    Network/chat.cpp \
    Network/socket.cpp \
    UI/addfriendwidget.cpp \
    UI/addgroupwidget.cpp \
    UI/alterimagewidget.cpp \
    UI/alternamewidget.cpp \
    UI/capturescreen.cpp \
    UI/connectionwidget.cpp \
    UI/creategroupwidget.cpp \
    UI/itemchatwidget.cpp \
    UI/mainwindow.cpp \
    UI/warn.cpp \
    labelwithclick.cpp \
    UI/login.cpp \
    UI/main.cpp \
    UI/register.cpp

HEADERS += \
    DB/groupinfo.h \
    DB/user.h \
    Network/chat.h \
    Network/socket.h \
    UI/addfriendwidget.h \
    UI/addgroupwidget.h \
    UI/alterimagewidget.h \
    UI/alternamewidget.h \
    UI/capturescreen.h \
    UI/connectionwidget.h \
    UI/creategroupwidget.h \
    UI/itemchatwidget.h \
    UI/mainwindow.h \
    UI/warn.h \
    labelwithclick.h \
    UI/login.h \
    UI/register.h

FORMS += \
    Network/chat.ui \
    UI/addfriendwidget.ui \
    UI/addgroupwidget.ui \
    UI/alterimagewidget.ui \
    UI/alternamewidget.ui \
    UI/connectionwidget.ui \
    UI/creategroupwidget.ui \
    UI/itemchatwidget.ui \
    UI/login.ui \
    UI/mainwindow.ui \
    UI/register.ui \
    UI/warn.ui


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    UI/resourse.qrc
