QT       += core gui network sql multimedia

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
    Keyboard/KeyButton.cpp \
    Keyboard/Keyboard.cpp \
    Keyboard/NumberKeyboard.cpp \
    health_home.cpp \
    health_record.cpp \
    health_report.cpp \
    health_user.cpp \
    log_in.cpp \
    main.cpp \
    mysql.cpp \
    sign_in.cpp \
    widget.cpp \


HEADERS += \
    Keyboard/AbstractKeyboard.h \
    Keyboard/KeyButton.h \
    Keyboard/Keyboard.h \
    Keyboard/NumberKeyboard.h \
    health_home.h \
    health_record.h \
    health_report.h \
    health_user.h \
    log_in.h \
    mysql.h \
    sign_in.h \
    widget.h

FORMS += \
    health_home.ui \
    health_record.ui \
    health_report.ui \
    health_user.ui \
    log_in.ui \
    sign_in.ui \
    widget.ui



# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Image.qrc \
    icon.qrc

DISTFILES +=


# 启用谷歌汉字库(推荐使用)
ENABLED_GOOGLE_CHINESE_LIB


DEFINES += ENABLED_GOOGLE_CHINESE_LIB


contains(DEFINES, ENABLED_GOOGLE_CHINESE_LIB) {
    RESOURCES += Resources/GoogleChineseLib.qrc
}



