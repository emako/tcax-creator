#-------------------------------------------------
#
# Project created by QtCreator 2018-05-01T13:40:02
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = capture-color
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

PROJECT_DIRECTORY = $$PWD
COMMON_DIRECTORY = $$PWD

ARCHITECTURE_64_BIT = false

RC_ICONS = $${COMMON_DIRECTORY}/res/icons/art.ico

CONFIG(debug, debug|release) {

    contains(QMAKE_COMPILER, gcc) {
        if($$ARCHITECTURE_64_BIT) {
            DESTDIR = $${COMMON_DIRECTORY}/build/debug-64bit-gcc
            TARGET = ($$TARGET)-debug-64bit-gcc
            OBJECTS_DIR = $${PROJECT_DIRECTORY}/generated/obj-debug-64bit-gcc
        } else {
            DESTDIR = $${COMMON_DIRECTORY}/build/debug-32bit-gcc
            TARGET = $${TARGET}-debug-32bit-gcc
            OBJECTS_DIR = $${PROJECT_DIRECTORY}/generated/obj-debug-32bit-gcc
        }
    }

    contains(QMAKE_COMPILER, msvc) {
        if($$ARCHITECTURE_64_BIT) {
            DESTDIR = $${COMMON_DIRECTORY}/build/debug-64bit-msvc
            TARGET = $${TARGET}-debug-64bit-msvc
            OBJECTS_DIR = $${PROJECT_DIRECTORY}/generated/obj-debug-64bit-msvc
        } else {
            DESTDIR = $${COMMON_DIRECTORY}/build/debug-32bit-msvc
            TARGET = $${TARGET}-debug-32bit-msvc
            OBJECTS_DIR = $${PROJECT_DIRECTORY}/generated/obj-debug-32bit-msvc
        }
    }

} else {

    contains(QMAKE_COMPILER, gcc) {
        if($$ARCHITECTURE_64_BIT) {
            DESTDIR = $${COMMON_DIRECTORY}/build/release-64bit-gcc
            TARGET = $${TARGET}
            OBJECTS_DIR = $${PROJECT_DIRECTORY}/generated/obj-release-64bit-gcc
        } else {
            DESTDIR = $${COMMON_DIRECTORY}/build/release-32bit-gcc
            TARGET = $${TARGET}
            OBJECTS_DIR = $${PROJECT_DIRECTORY}/generated/obj-release-32bit-gcc
        }
    }

    contains(QMAKE_COMPILER, msvc) {
        if($$ARCHITECTURE_64_BIT) {
            DESTDIR = $${COMMON_DIRECTORY}/build/release-64bit-msvc
            TARGET = $${TARGET}
            OBJECTS_DIR = $${PROJECT_DIRECTORY}/generated/obj-release-64bit-msvc
        } else {
            DESTDIR = $${COMMON_DIRECTORY}/build/release-32bit-msvc
            TARGET = $${TARGET}
            OBJECTS_DIR = $${PROJECT_DIRECTORY}/generated/obj-release-32bit-msvc
        }
    }
    DEFINES += NDEBUG
}

S = $${DIR_SEPARATOR}

D = $$DESTDIR
D = $$replace(D, $$escape_expand(\\), $$S)
D = $$replace(D, /, $$S)

E = $$escape_expand(\n\t)

win32 {
    QT += winextras

    DEPLOY_COMMAND = windeployqt
    DEPLOY_TARGET = $$shell_quote($$shell_path($${D}/$${TARGET}.exe))
    QMAKE_POST_LINK += $${DEPLOY_COMMAND} --no-translations $${DEPLOY_TARGET} $${E}

    if($$ARCHITECTURE_64_BIT) {
        message("x86_64 build")
    } else {
        message("x86 build")
        contains(QMAKE_COMPILER, gcc) {
                QMAKE_LFLAGS += -Wl,--large-address-aware
        }
        contains(QMAKE_COMPILER, msvc) {
                QMAKE_LFLAGS += /LARGEADDRESSAWARE
        }
    }
}

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    dialog/yuv_dialog.cpp \
    form/color_wheel.cpp \
    dialog/color_whell_dialog.cpp \
    com/common.cpp

HEADERS += \
        mainwindow.h \
    dialog/yuv_dialog.h \
    form/color_wheel.h \
    dialog/color_whell_dialog.h \
    com/version.h \
    com/common.h

FORMS += \
        mainwindow.ui \
    dialog/yuv_dialog.ui \
    dialog/color_whell_dialog.ui

RESOURCES += \
    res/res.qrc
