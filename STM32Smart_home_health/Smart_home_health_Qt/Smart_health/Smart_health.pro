QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#加入开源库组件
LIBS += $$PWD/libs/libcomponents.a
INCLUDEPATH += \
    $$PWD/libs/Include



SOURCES += \
    log_in.cpp \
    main.cpp \
    mysql.cpp \
    sign_in.cpp \
    widget.cpp

HEADERS += \
    log_in.h \
    mysql.h \
    sign_in.h \
    widget.h

FORMS += \
    log_in.ui \
    sign_in.ui \
    widget.ui



# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    image.qrc

DISTFILES +=
