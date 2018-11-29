#-------------------------------------------------
#
# Project created by QtCreator 2018-10-29T20:22:56
#
#-------------------------------------------------

QT      += core gui widgets

TARGET = tcax-creator
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

PROJECT_DIRECTORY = $$PWD
COMMON_DIRECTORY = $$PWD

RC_ICONS = $${COMMON_DIRECTORY}/res/icons/tcax.ico

LUA_LIB = $${COMMON_DIRECTORY}/lua/lib
LUA_HEADER = $${COMMON_DIRECTORY}/lua/include

INCLUDEPATH += $${LUA_HEADER}
LIBS += -L$${LUA_LIB} -llua5.1

CONFIG(debug, debug|release) {

    contains(QMAKE_COMPILER, gcc) {
        if($$ARCHITECTURE_64_BIT) {
            DESTDIR = $${PROJECT_DIRECTORY}/build/debug-64bit-gcc
            TARGET = ($$TARGET)-debug-64bit-gcc
            OBJECTS_DIR = $${PROJECT_DIRECTORY}/generated/obj-debug-64bit-gcc
        } else {
            DESTDIR = $${PROJECT_DIRECTORY}/build/debug-32bit-gcc
            TARGET = $${TARGET}-debug-32bit-gcc
            OBJECTS_DIR = $${PROJECT_DIRECTORY}/generated/obj-debug-32bit-gcc
        }
    }

    contains(QMAKE_COMPILER, msvc) {
        if($$ARCHITECTURE_64_BIT) {
            DESTDIR = $${PROJECT_DIRECTORY}/build/debug-64bit-msvc
            TARGET = $${TARGET}-debug-64bit-msvc
            OBJECTS_DIR = $${PROJECT_DIRECTORY}/generated/obj-debug-64bit-msvc
        } else {
            DESTDIR = $${PROJECT_DIRECTORY}/build/debug-32bit-msvc
            TARGET = $${TARGET}-debug-32bit-msvc
            OBJECTS_DIR = $${PROJECT_DIRECTORY}/generated/obj-debug-32bit-msvc
        }
    }

} else {

    contains(QMAKE_COMPILER, gcc) {
        if($$ARCHITECTURE_64_BIT) {
            DESTDIR = $${PROJECT_DIRECTORY}/build/release-64bit-gcc
            TARGET = $${TARGET}
            OBJECTS_DIR = $${PROJECT_DIRECTORY}/generated/obj-release-64bit-gcc
        } else {
            DESTDIR = $${PROJECT_DIRECTORY}/build/release-32bit-gcc
            TARGET = $${TARGET}-32bit
            OBJECTS_DIR = $${PROJECT_DIRECTORY}/generated/obj-release-32bit-gcc
        }
    }

    contains(QMAKE_COMPILER, msvc) {
        if($$ARCHITECTURE_64_BIT) {
            DESTDIR = $${PROJECT_DIRECTORY}/build/release-64bit-msvc
            TARGET = $${TARGET}
            OBJECTS_DIR = $${PROJECT_DIRECTORY}/generated/obj-release-64bit-msvc
        } else {
            DESTDIR = $${PROJECT_DIRECTORY}/build/release-32bit-msvc
            TARGET = $${TARGET}-32bit
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
    com/tcax_helper.cpp \
    com/common.cpp \
    com/style_sheet.cpp \
    com/config.cpp \
    editor/number_matcher.cpp \
    editor/script_editor.cpp \
    designer/tcc_designer.cpp \
    designer/script_designer.cpp \
    designer/tcas_designer.cpp \
    tcax/helper/hla_util.c \
    tcax/helper/rb.c \
    tcax/helper/tcas.c \
    tcax/helper/vector.c \
    tcax/libtcc/map.c \
    tcax/libtcc/string.c \
    tcax/libtcc/tcc.c \
    tcax/libmap/strmap.c \
    tcax/libmap/tccmap.c \
    std/std_manager.cpp \
    std/std_watcher.cpp \
    com/lua_helper.cpp \
    tools/ass_editor.cpp \
    tools/ass_syntax_highlighter.cpp \
    tools/ass_ktag_typer.cpp \
    editor/py_syntax_highlighter.cpp \
    tools/ass_conv.cpp \
    com/ass_helper.cpp \
    tools/ass_ttag_looper.cpp \
    com/preferences_dialog.cpp

HEADERS += \
        mainwindow.h \
    com/tcax_helper.h \
    com/common.h \
    com/style_sheet.h \
    com/config.h \
    editor/number_matcher.h \
    editor/script_editor.h \
    designer/tcc_designer.h \
    designer/script_designer.h \
    designer/tcas_designer.h \
    tcax/helper/hla_util.h \
    tcax/helper/rb.h \
    tcax/helper/std.h \
    tcax/helper/tcas.h \
    tcax/helper/vector.h \
    tcax/libtcc/map.h \
    tcax/libtcc/string.h \
    tcax/libtcc/tcc.h \
    tcax/libmap/strmap.h \
    tcax/libmap/tccmap.h \
    std/std_manager.h \
    std/std_watcher.h \
    com/lua_helper.h \
    tools/ass_editor.h \
    tools/ass_syntax_highlighter.h \
    tools/ass_ktag_typer.h \
    editor/py_syntax_highlighter.h \
    tools/ass_tag.h \
    tools/ass_conv.h \
    com/ass_helper.h \
    tools/ass_ttag_looper.h \
    com/preferences_dialog.h

FORMS += \
        mainwindow.ui \
    designer/tcc_designer.ui \
    designer/script_designer.ui \
    designer/tcas_designer.ui \
    std/std_watcher.ui \
    tools/ass_ktag_typer.ui \
    tools/ass_conv.ui \
    tools/ass_ttag_looper.ui \
    com/preferences_dialog.ui

RESOURCES += \
    res/res.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
