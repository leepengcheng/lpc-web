#-------------------------------------------------
#                   _ooOoo_
#                  o8888888o
#                  88" . "88
#                  (| -_- |)
#                  O\  =  /O
#               ____/`---'\____
#             .'  \\|     |//  `.
#            /  \\|||  :  |||//  \
#           /  _||||| -:- |||||-  \
#           |   | \\\  -  /// |   |
#           | \_|  ''\---/''  |   |
#           \  .-\__  `-`  ___/-. /
#         ___`. .'  /--.--\  `. . __
#      ."" '<  `.___\_<|>_/___.'  >'"".
#     | | :  `- \`.;`\ _ /`;.`/ - ` : | |
#     \  \ `-.   \_ __\ /__ _/   .-` /  /
#======`-.____`-.___\_____/___.-`____.-'======
#                   `=---='
#^^^^^^^^^^^^^^^^Create By 李鹏程^^^^^^^^^^^^^^^^^^
#^^^^^^^^^^^^^^^^2017年3月10日^^^^^^^^^^^^^^^^^^^^^
#-------------------------------------------------

QT       += core gui printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AnsysPlugin
TEMPLATE = app


SOURCES += main.cpp\
    qcustomplot.cpp \
    pugixml.cpp \
    matconfigure.cpp \
    ansysdataplot.cpp \
    matdataparse.cpp \
    dataimport.cpp \
    fluentparser.cpp

HEADERS  += \
    qcustomplot.h \
    pugiconfig.hpp \
    pugixml.hpp \
    matconfigure.h \
    ansysdataplot.h \
    matdataparse.h \
    dataimport.h \
    fluentparser.h

TRANSLATIONS+=cn.ts

FORMS    += \
    matconfigure.ui \
    dataplot.ui \
    dataimport.ui \
    fluentparser.ui

RESOURCES += \
    icon.qrc

DISTFILES +=
