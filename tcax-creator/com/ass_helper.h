#ifndef ASS_HELPER_H
#define ASS_HELPER_H

#include "common.h"
#include "../tools/ass_tag.h"

#include <QObject>
#include <QRegularExpressionMatch>

namespace AssHelper
{
    bool lyricToAss(const QString &a_srcPath, const QString &a_tarPath);
    bool textToAss(const QString &a_srcPath, const QString &a_tarPath);
    bool assToText(const QString &a_srcPath, const QString &a_tarPath);
    bool assToLyric(const QString &a_srcPath, const QString &a_tarPath);

    QList<QPair<qint64, QString>> lyricParse(const QString &a_str);
    QList<QPair<qint64, QString>> assParse(const QString &a_str);
    QList<QPair<qint64, QString>> textParse(const QString &a_str);

    inline QString getAssHeader(void);
    inline QString getLrcHeader(void);

    inline QString toTimecode(qint64 a_msec, bool a_fullFormat = true);
    inline QString toAssEventsLine(qint64 a_startTimeMsec, qint64 a_endTimeMsec, QString text, bool a_isComment = false);
    inline QString stripAssTag(QString a_text);
    inline QStringList splitAssEvents(const QString &a_assLine);
    inline qint64 assTimeToLrcTime(const QString &timecode);
};

#endif // ASS_HELPER_H
