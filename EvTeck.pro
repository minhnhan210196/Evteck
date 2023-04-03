QT       += core gui
QT += charts network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    butterworthfilter.cpp \
    chart.cpp \
    chartview.cpp \
    dialog.cpp \
    hl_simplekalmanfilter.cpp \
    main.cpp \
    mainwindow.cpp \
    notchfilter.cpp \
    setting.cpp

HEADERS += \
    butterworthfilter.h \
    chart.h \
    chartview.h \
    dialog.h \
    hl_simplekalmanfilter.h \
    mainwindow.h \
    notchfilter.h \
    setting.h

FORMS += \
    dialog.ui \
    mainwindow.ui \
    setting.ui


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    ../../Desktop/EVTeck/MONTPELIER_BiaMedical-master/1.Software/Picture/MPL_U_Logo_30pixel.png \
    data_test/data_test_4.txt

RESOURCES += \
    image.qrc
