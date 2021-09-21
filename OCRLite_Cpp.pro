QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
CONFIG += console

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
INCLUDEPATH += /usr/local/include/tesseract \
            /usr/include/leptonica \
            /usr/local/include/leptonica

LIBS += -L/usr/local/lib/ -llept -ltesseract -larchive

SOURCES += \
    Engine/baidu.cpp \
    Engine/base.cpp \
    Engine/engine_factory.cpp \
    Engine/latexms.cpp \
    Engine/pixmap2byteArray.cpp \
    Engine/tesseract_api.cpp \
    area_picker.cpp \
    main.cpp \
    main_window.cpp \
    setting_dialog.cpp

HEADERS += \
    Engine/baidu.h \
    Engine/base.h \
    Engine/engine_factory.h \
    Engine/latexms.h \
    Engine/pixmap2byteArray.h \
    Engine/tesseract_api.h \
    area_picker.h \
    main_window.h \
    setting_dialog.h

FORMS += \
    main_window.ui \
    setting_dialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    icons.qrc
