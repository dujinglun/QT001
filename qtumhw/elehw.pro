#-------------------------------------------------
#
# Project created by QtCreator 2018-10-29T20:29:24
#
#-------------------------------------------------

QT       += core gui
QT       += sql


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = elehw
TEMPLATE = app


SOURCES += main.cpp\
        mainwidget.cpp \
    qextserial/qextserialbase.cpp \
    qextserial/posix_qextserialport.cpp \
    form2.cpp \
    form3.cpp \
    form4.cpp \
    hex.cpp \
    dialog.cpp \
    form5.cpp \
    scp_multi_sig.cpp \
    mydatabase.cpp

HEADERS  += mainwidget.h \
    hex.h \
    scp_multi_sig.h\
    qextserial/qextserialbase.h \
    qextserial/posix_qextserialport.h \
    form2.h \
    form3.h \
    form4.h \
    dialog.h \
    form5.h \
    mydatabase.h

FORMS    += mainwidget.ui \
    form2.ui \
    form3.ui \
    form4.ui \
    dialog.ui \
    form5.ui



include(./QRCode/QRCode.pri)
include(./qt-json/qt-json.pri)


#要部署的目录
target.path=/opt/QtE-Demo/


#安装目标文件
INSTALLS+=target

RESOURCES += \
    p.qrc



