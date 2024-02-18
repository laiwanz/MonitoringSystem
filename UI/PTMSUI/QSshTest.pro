#-------------------------------------------------
#
# Project created by QtCreator 2019-12-18T09:09:18
#
#-------------------------------------------------

QT       += core
QT       += gui
QT       += network
QT       += concurrent
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += c++11


TARGET = QSshTest
TEMPLATE = app


include($$PWD/../QSsh/QSsh.pri)
# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
    RedisParse.cpp \
    RedisPubSuber.cpp \
        Widget.cpp \
    CConnectionForSshClient.cpp \
    cmd.cpp \
    mainwindow.cpp \
    master.cpp \
    task.cpp


HEADERS  += Widget.h \
    CConnectionForSshClient.h \
    RedisParse.h \
    RedisPubSuber.h \
    RedisRecDefine.h \
    cmd.h \
    mainwindow.h \
    master.h \
    task.h





win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../data/middleware/middleware/common/lib/vc15/x86/ -lcbl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../data/middleware/middleware/common/lib/vc15/x86/ -lcbld
else:unix: LIBS += -L$$PWD/../../../../../data/middleware/middleware/common/lib/vc15/x86/ -lcbl


LIBS += -lws2_32


INCLUDEPATH += $$PWD/../../../../../data/middleware/middleware/common/include
DEPENDPATH += $$PWD/../../../../../data/middleware/middleware/common/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../data/middleware/middleware/common/lib/vc15/x86/libcbl.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../data/middleware/middleware/common/lib/vc15/x86/libcbld.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../data/middleware/middleware/common/lib/vc15/x86/cbl.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../data/middleware/middleware/common/lib/vc15/x86/cbld.lib
else:unix: PRE_TARGETDEPS += $$PWD/../../../../../data/middleware/middleware/common/lib/vc15/x86/libcbl.a

FORMS += \
    cmd.ui \
    mainwindow.ui \
    task.ui


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../QSsh/lib/ -lQSsh
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../QSsh/lib/ -lQSsh
else:unix: LIBS += -L$$PWD/../QSsh/lib/ -lQSsh

INCLUDEPATH += $$PWD/../QSsh/include
DEPENDPATH += $$PWD/../QSsh/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../QSsh/lib/ -lBotan
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../QSsh/lib/ -lBotan
else:unix: LIBS += -L$$PWD/../QSsh/lib/ -lBotan

INCLUDEPATH += $$PWD/../QSsh/include
DEPENDPATH += $$PWD/../QSsh/include
