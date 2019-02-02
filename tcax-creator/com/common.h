#ifndef COMMON_H
#define COMMON_H

#include <QObject>
#include <QDebug>
#include <QResource>
#include <QUrl>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QFile>
#include <QFileInfo>
#include <QFileInfoList>
#include <QFileDialog>
#include <QDir>
#include <QDirModel>
#include <QFileSystemModel>
#include <QFileSystemWatcher>
#include <QCoreApplication>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QTimer>
#include <QMultiMap>
#include <QClipboard>
#include <QTime>
#include <QDateTime>
#include <QUuid>
#include <QPlainTextEdit>
#include <QSettings>
#include <QAction>
#include <QStyleFactory>
#include <QMessageBox>
#include <stdio.h>
#include <Windows.h>

class Config;

#define TCAX_CREATOR_VERSION "r6.1"

#define global

#define NULLSTR QString()
#define NULLSTRS QStringList()
#define NULLDIR "\"\""
#define BLANKSTR " "
#define BLANKCHR ' '

#define CONCAT(__x, __y) __x ## __y

#define UTF8_BOM (QChar(239) + QChar(187) + QChar(191) + NULLSTR) // UTF8-BOM: 0xEF 0xBB 0xBF

#define MSG_INFORMATION QObject::tr("Information")
#define MSG_QUESTION QObject::tr("Question")
#define MSG_WARNING QObject::tr("Warning")
#define MSG_CRITICAL QObject::tr("Critical")
#define MSG_ABOUT QObject::tr("About")

#define MSG_TCAX_INFORMATION tr("TCAX - information")
#define MSG_TCAX_QUESTION tr("TCAX - question")
#define MSG_TCAX_WARNING tr("TCAX - warning")
#define MSG_TCAX_CRITICAL tr("TCAX - critical")

#define DEFAULT_FONT QFont(tr("Consolas"))
#define DEFAULT_STYLE_FACTORY QStyleFactory::keys().first()

#define EXT_TCC   "tcc"
#define EXT_TCAS  "tcas"
#define EXT_ASS   "ass"
#define EXT_SSA   "ssa"
#define EXT_TXT   "txt"
#define EXT_LRC   "lrc"
#define EXT_KRC   "krc"
#define EXT_QRC   "qrc"
#define EXT_PY    "py"
#define EXT_VPY   "vpy"
#define EXT_AVS   "avs"
#define EXT_AVSI  "avsi"
#define EXT_TMP   "tmp"
#define EXT_TOKEN "token"
#define EXT_PYC   "pyc"
#define EXT_PNG   "png"
#define EXT_LUA   "lua"

#define DIR_PYCACHE "__pycache__"

#define EXEC_NONE QUuid::createUuid().toString()
#define EXEC_TCAX "tcax"
#define EXEC_PYTHON "python"
#define EXEC_PYTHONW "pythonw"
#define EXEC_ASSOC "assoc"
#define EXEC_CAPTURE_COLOR "capture-color"

#define DEFAULT_COLOR "000000"
#define DEFAULT_COLOR_INIT "#"

#define USING
#define SCOPE
#define END_OF_USING

#define RESOURCE_COMPILE      ":/buttons/compile.png"
#define RESOURCE_COMPILE_NONE ":/buttons/compile_error.png"
#define RESOURCE_COMPILE_WARN ":/buttons/compile_warning.png"

#define ACTION_FILE_SYSTEM_OPEN_WITH_SYS tr("Open with default")
#define ACTION_FILE_SYSTEM_COPY_FILENAME tr("Copy filename")
#define ACTION_FILE_SYSTEM_REMOVE tr("Remove")
#define ACTION_FILE_SYSTEM_BUILD tr("Build")
#define ACTION_FILE_SYSTEM_EXPLORE tr("Explore")

#define ACTION_KASS_MENU_MAKE tr("Make")
#define ACTION_KASS_MENU_OPEN tr("Open")
#define ACTION_KASS_MENU_COPY tr("Copy to clipboard")
#define ACTION_KASS_MENU_SAVE_HERE tr("Save here")
#define ACTION_KASS_MENU_SAVE_AS tr("Save As ...")
#define ACTION_KASS_MENU_CLEAR tr("Clear")
#define ACTION_KASS_MENU_ENABLE_WRAP_LINE tr("Enable Line Wrap")
#define ACTION_KASS_MENU_ENABLE_SCROLL_SYNC tr("Enable Scroll Sync")

#define ACTION_CASS_MENU_OPEN tr("Open ...")

#define ACTION_JSON_RECENT "Recent"
#define ACTION_JSON_RECENT_LIST "List"

#define ACTION_PROP_ID "action_prop_id"
#define ACTION_NEW(a, b, c) {a, new QAction(QIcon(b), a, c)}
#define ACTION_NONE {NULLSTR, nullptr}

#define COLOR_LOGGING_DEFAULT QColor(0, 0, 0)
#define COLOR_LOGGING_INFO QColor(0, 0, 128)
#define COLOR_LOGGING_WARN QColor(255, 0, 0)
#define COLOR_LOGGING_STATUS QColor(0, 128, 128)
#define COLOR_LOGGING_STD_ERROR QColor(135, 45, 90)
#define COLOR_LOGGING_STD_OUTPUT QColor(128, 0, 128)

#define SAVED_FLAG "saved_flag"

#define PASS do{}while(false)

#define KB (1024)
#define MB (KB * KB)
#define GB (MB * KB)

#define MINUTE (60)
#define HOUR (MINUTE * MINUTE)
#define SECOND_TO_MILLISECOND_UNIT (1000)

extern Config *g_pConfig;

static const char QT_DQM[]              = "\"";
static const char QT_NOR_DIR_SPL[]      = "/";
static const char QT_NOR_EOL[]          = "\n";
static const char QT_OTR_EOL[]          = "\r\n";
static const char QT_MAC_EOL[]          = "\r";
static const char QT_QUOTE[]            = "\"";

enum EINDEX : int {
    eINDEX_NONE = -1,
    eINDEX_0 = 0,
    eINDEX_1 = 1,
    eINDEX_2 = 2,
    eINDEX_3 = 3,
    eINDEX_4 = 4,
    eINDEX_5 = 5,
    eINDEX_6 = 6,
    eINDEX_7 = 7,
    eINDEX_8 = 8,
    eINDEX_9 = 9,
    eINDEX_10 = 10,
    eINDEX_16 = 16,
    eINDEX_24 = 24,
    eINDEX_30 = 30,
    eINDEX_32 = 32,
    eINDEX_100 = 100,
    eINDEX_1000 = 1000,
    eINDEX_MAX,
};

namespace Common
{
    wchar_t *toWCharT(QString a_string);
    QString fromWCharT(wchar_t *a_wchart);

    QString getFileExt(const QString &a_filename);
    QString chgFileExt(const QString &a_filename, QString a_ext);
    QString delFileExt(const QString &a_filename);

    QString getFileText(const QString &a_filename, const QString &a_codec = NULLSTR);
    QStringList getFileTextLines(const QString &a_filename, const QString &a_codec = NULLSTR);
    bool setFileText(const QString &a_filename, const QString &a_text, bool a_isBom = false);

    QJsonObject toJson(const QString a_string);
    QString fromJson(const QJsonObject& a_json);

    QJsonArray toJsonArray(const QString a_string);
    QString fromJsonArray(const QJsonArray& a_jsonArray);

    bool removeFile(const QString &a_filename, QWidget *a_pHwnd);
    QT_DEPRECATED bool recycleFile(const QString &a_filename, QWidget *a_pHwnd = nullptr);

    QFileInfoList getFolderList(QString a_path);
    QFileInfoList getFileList(QString a_path);
    QString findFirstFilePath(QString a_filename, QString a_path = QDir::currentPath());

    QString fromHtml(const QString &a_str);
    QString toHtml(const QString &a_str, QColor a_color = QColor(Qt::black), bool logging = false);
    QString toHtmlText(const QString &a_str);

    QString toNormalEol(const QString &a_str);
    QString toDosEol(const QString &a_str);

    QByteArray getResource(const QString &a_filename);
    QString fromResource(const QString &a_filename);

    QString currentTime(void);
    QString currentDateTime(void);

    QString creatTempFilename(const QString &a_ext = EXT_TMP);

    bool inRange(int a_index, int a_len);
};

#endif // COMMON_H
