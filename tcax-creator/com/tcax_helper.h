#ifndef TCAX_HELPER_H
#define TCAX_HELPER_H

#include "common.h"
#include "../tcax/libtcc/tcc.h"

#define L2Q(__x) QString::fromStdWString(__x)

#define TCC_COMMENT_1           "# tcc file version"
#define TCC_COMMENT_2           "# mode configuration"
#define TCC_COMMENT_3           "# py settings"
#define TCC_COMMENT_4           "# main settings"
#define TCC_COMMENT_5           "# style settings"
#define TCC_COMMENT_6           "# additional settings"
#define TCC_LINE_SPACE          "    "
#define TCC_TRUE                "true"
#define TCC_FALSE               "false"
#define TCC_HORI                "hori"
#define TCC_HORIZONTAL          "horizontal"
#define TCC_VERT                "vert"
#define TCC_VERTICAL            "vertical"
#define TCC_WORD                "word"
#define TCC_NORMAL              "normal"
#define TCC_SYL                 "syl"
#define TCC_SYLLABLE            "syllable"
#define TCC_ADV                 "adv"
#define TCC_ADVANCED            "advanced"

#define TCC_COLOR_FFFFFF        "FFFFFF"
#define TCC_COLOR_000000        "000000"

#define TCC_TCC_VER_STR         L2Q(TCC_VER_STR)
/*< tcc file version */
#define TCC_TCC_FILE_VERSION    L2Q(TCC_FILE_VERSION)
/*< mode configuration */
#define TCC_INHERIT_ASS_HEADER  L2Q(INHERIT_ASS_HEADER)
#define TCC_K_MODE              L2Q(K_MODE)
#define TCC_TEXT_LAYOUT         L2Q(TEXT_LAYOUT)
#define TCC_TCAXPY_INIT         L2Q(TCAXPY_INIT)
#define TCC_TCAXPY_USER         L2Q(TCAXPY_USER)
#define TCC_TCAXPY_FIN          L2Q(TCAXPY_FIN)
/*< py settings */
#define TCC_PY_FILE             L2Q(PY_FILE)
#define TCC_BEG_LINE            L2Q(BEG_LINE)
#define TCC_END_LINE            L2Q(END_LINE)
/*< main settings */
#define TCC_K_TIMED_ASS_FILE    L2Q(K_TIMED_ASS_FILE)
#define TCC_FONT_FILE           L2Q(FONT_FILE)
#define TCC_FONT_FACE_ID        L2Q(FONT_FACE_ID)
#define TCC_FONT_SIZE           L2Q(FONT_SIZE)
#define TCC_FX_WIDTH            L2Q(FX_WIDTH)
#define TCC_FX_HEIGHT           L2Q(FX_HEIGHT)
#define TCC_FX_FPS              L2Q(FX_FPS)
#define TCC_ALIGNMENT           L2Q(ALIGNMENT)
#define TCC_X_OFFSET            L2Q(X_OFFSET)
#define TCC_Y_OFFSET            L2Q(Y_OFFSET)
#define TCC_SPACING             L2Q(SPACING)
#define TCC_SPACE_SCALE         L2Q(SPACE_SCALE)
/*< style settings */
#define TCC_FONT_FACE_NAME      L2Q(FONT_FACE_NAME)
#define TCC_BORD                L2Q(BORD)
#define TCC_SHAD                L2Q(SHAD)
#define TCC_PRIMARY_COLOR       L2Q(PRIMARY_COLOR)
#define TCC_SECONDARY_COLOR     L2Q(SECONDARY_COLOR)
#define TCC_OUTLINE_COLOR       L2Q(OUTLINE_COLOR)
#define TCC_BACK_COLOR          L2Q(BACK_COLOR)
#define TCC_PRIMARY_ALPHA       L2Q(PRIMARY_ALPHA)
#define TCC_SECONDARY_ALPHA     L2Q(SECONDARY_ALPHA)
#define TCC_OUTLINE_ALPHA       L2Q(OUTLINE_ALPHA)
#define TCC_BACK_ALPHA          L2Q(BACK_ALPHA)
/*< additional settings */
#define TCC_BLUR                L2Q(BLUR)

#define winFontRegRoot          L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts\\"

#define fileTypeRaw             "Raw";
#define fileTypeCompressed      "Compressed";
#define fileTypeCompressedZ     "Compressed Z";
#define fileTypeParsed          "Parsed";
#define fileTypeParsedZ         "Parsed Z";
#define fileTypeUnknown         "Unknown";

typedef QMultiHash<QString, QVariant> TccMap; /// MultiHash<AttrKey, AttrValue>
typedef QMap<QString, QVariant> TccDefMap; /// QMap<AttrKey, AttrValue>
typedef QList<QVariant> TccMapItems; /// QList<AttrValues>
typedef QMultiHash<QString, QPair<QVariant, QVariant>> TccPyFileList; /// QMultiHash<PyFile, QPair<BegLine, EndLine>>

namespace TcaxHelper
{
    bool tccCheck(const QString &a_tccString);
    QString tccStrip(QString a_tccString);
    TccDefMap tccDefaultMap(void);
    QString tccToAttrLine(QString a_attr, QString a_attrValue);
    TccPyFileList tccMapParseToTccPyFileList(TccMap a_tccMap);

    TccMap tccParseToMap(const QString &a_filename);
    bool tccSave(const QString &a_filename, TccMap a_tccMap);

    QT_DEPRECATED bool tccCreate(const QString &a_filename);
    QT_DEPRECATED TCC_Attributes tccParse(const QString &a_filename);
    QT_DEPRECATED bool tccSave(const QString &a_filename, TCC_Attributes a_attr);
    QT_DEPRECATED void tccFree(TCC_pAttributes a_pAttr);

    bool tcasShiftTime(const QString &a_filename, int a_shiftTime, bool a_saveCopy);
    QString tcasInfo(const QString &a_filename);

    QStringList fontList(void);
    QString fontFile(const QString &a_fontName);
    QStringList fontFace(const QString &a_fontName);

    void tcaxW2A(const wchar_t *a_uni, char **a_ansi);
};
#endif // TCAX_HELPER_H
