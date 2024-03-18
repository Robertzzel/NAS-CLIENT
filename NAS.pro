QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets core core-private

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    commands.cpp \
    downloadworker.cpp \
    filewidget.cpp \
    main.cpp \
    mainwindow.cpp \
    messagehandler.cpp \
    uploadworker.cpp

HEADERS += \
    commands.h \
    file.h \
    filewidget.h \
    mainwindow.h \
    messagehandler.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

include(/home/robert/Qt/android_openssl-master/openssl.pri)
