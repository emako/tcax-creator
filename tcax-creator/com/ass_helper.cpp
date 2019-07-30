#include "ass_helper.h"

bool AssHelper::lyricToAss(const QString &a_srcPath, const QString &a_tarPath)
{
    QStringList assEventsLines;
    QList<QPair<qint64, QString>> lyricParsedList  = lyricParse(Common::getFileText(a_srcPath));

    assEventsLines << getAssHeader();
    for(int i = eINDEX_0; i < lyricParsedList.length(); i++)
    {
        qint64 startTimeMsec = lyricParsedList.at(i).first;
        qint64 endTimeMsec = eINDEX_0;
        QString text = lyricParsedList.at(i).second;

        if(Common::inRange(i + eINDEX_1, lyricParsedList.length()))
        {
            endTimeMsec = lyricParsedList.at(i + eINDEX_1).first;
        }
        else
        {
            endTimeMsec = startTimeMsec + eINDEX_10 * SECOND_TO_MILLISECOND_UNIT; // added 10s for end time of last line.
        }
        assEventsLines << toAssEventsLine(startTimeMsec, endTimeMsec, text, false);
    }

    return Common::setFileText(a_tarPath, assEventsLines.join(QT_OTR_EOL), true);
}

bool AssHelper::textToAss(const QString &a_srcPath, const QString &a_tarPath)
{
    QStringList assEventsLines;
    QList<QPair<qint64, QString>> textParsedList  = textParse(Common::getFileText(a_srcPath));

    assEventsLines << getAssHeader();
    for(int i = eINDEX_0; i < textParsedList.length(); i++)
    {
        qint64 startTimeMsec = textParsedList.at(i).first;
        qint64 endTimeMsec = eINDEX_0;
        QString text = textParsedList.at(i).second;

        if(Common::inRange(i + eINDEX_1, textParsedList.length()))
        {
            endTimeMsec = textParsedList.at(i + eINDEX_1).first;
        }
        else
        {
            endTimeMsec = startTimeMsec + eINDEX_5 * SECOND_TO_MILLISECOND_UNIT; // added 10s for end time of last line.
        }
        assEventsLines << toAssEventsLine(startTimeMsec, endTimeMsec, text, false);
    }

    return Common::setFileText(a_tarPath, assEventsLines.join(QT_OTR_EOL), true);
}

bool AssHelper::assToText(const QString &a_srcPath, const QString &a_tarPath)
{
    QStringList textList;
    QList<QPair<qint64, QString>> assParsedList = assParse(Common::getFileText(a_srcPath));

    for(auto assParsedLine : assParsedList)
    {
        textList << assParsedLine.second;
        qDebug() << textList.last();
    }

    return Common::setFileText(a_tarPath, textList.join(QT_OTR_EOL), true);
}

bool AssHelper::assToLyric(const QString &a_srcPath, const QString &a_tarPath)
{
    QStringList textList;
    QList<QPair<qint64, QString>> assParsedList = assParse(Common::getFileText(a_srcPath));

    textList << getLrcHeader();

    for(auto assParsedLine : assParsedList)
    {
        qint64 startTimeMsec = assParsedLine.first;
        QString text = assParsedLine.second;

        textList << QString("[%1]%2").arg(toTimecode(startTimeMsec, false)).arg(text);
        qDebug() << textList.last();
    }

    return Common::setFileText(a_tarPath, textList.join(QT_OTR_EOL), true);
}

QList<QPair<qint64, QString>> AssHelper::lyricParse(const QString &a_str)
{
    QList<QPair<qint64, QString>> lyricParsedList;
    QString lyricStr = a_str;
    QStringList lines = lyricStr.replace(QT_MAC_EOL, NULLSTR).split(QT_NOR_EOL);
    const QRegularExpression rex("\\[(ar)?(ti)?(al)?(by)?(offset)?(\\d+)?:(\\d+)?(\\.\\d+)?(\\S+)?\\]");

    for(QString line : lines)
    {
       QRegularExpressionMatch match = rex.match(line);

       if(match.captured(6).isEmpty())
       {
           continue;
       }
       else
       {
#ifdef QT_DEBUG
            for(int i = 0; i < match.capturedLength(); i++)
            {
                qDebug() << match.captured(i);
            }
#endif
            qint64 time = (match.captured(6).toInt() * MINUTE * SECOND_TO_MILLISECOND_UNIT) + (match.captured(7).toInt() * SECOND_TO_MILLISECOND_UNIT) + static_cast<int>((match.captured(8).toDouble() * SECOND_TO_MILLISECOND_UNIT));
            QString text = QString(line).right(QString(line).length() - match.capturedLength());

            lyricParsedList.append(QPair<qint64, QString>(time, text));
       }
    }
    return lyricParsedList;
}

inline QString AssHelper::toTimecode(qint64 a_msecTime, bool a_fullFormat)
{
    int secTime = static_cast<int>(a_msecTime / SECOND_TO_MILLISECOND_UNIT);
    int hourPart = static_cast<int>(std::floor(secTime / (MINUTE * MINUTE)));
    int minPart = static_cast<int>(std::floor(secTime % (MINUTE * MINUTE)) / MINUTE);
    int secPart = static_cast<int>(std::floor(secTime % MINUTE));
    int msecPart = QString::number(a_msecTime).right(3).toInt();
    QString timecode;

    if(a_fullFormat)
    {
        timecode.sprintf("%01d:%02d:%02d.%02d", hourPart, minPart, secPart, msecPart / eINDEX_10);
    }
    else
    {
        if(hourPart >= eINDEX_1)
        {
            minPart = MINUTE;
            secPart = eINDEX_0;
            msecPart = eINDEX_0;
        }
        timecode.sprintf("%02d:%02d.%02d", minPart, secPart, msecPart / eINDEX_10);
    }

    return timecode;
}

inline QString AssHelper::getAssHeader(void)
{
    const char *assHeader = "[Script Info]\n"
                            "; This script is generated by TCAX Creator\n"
                            "; Welcome to TCAX forum http://www.tcax.org\n"
                            "ScriptType: v4.00+\n"
                            "\n"
                            "[V4+ Styles]\n"
                            "Format: Name, Fontname, Fontsize, PrimaryColour, SecondaryColour, OutlineColour, BackColour, Bold, Italic, Underline, StrikeOut, ScaleX, ScaleY, Spacing, Angle, BorderStyle, Outline, Shadow, Alignment, MarginL, MarginR, MarginV, Encoding\n"
                            "Style: Default,Arial,20,&H00FFFFFF,&H000000FF,&H00000000,&H00000000,0,0,0,0,100,100,0,0,1,2,2,2,10,10,10,1\n"
                            "\n"
                            "[Events]\n"
                            "Format: Layer, Start, End, Style, Name, MarginL, MarginR, MarginV, Effect, Text";
    return
#ifdef Q_OS_WIN32
    QString(assHeader).replace(QT_NOR_EOL, QT_OTR_EOL);
#else
    QString(assHeader);
#endif
}

inline QString AssHelper::getLrcHeader(void)
{
    const char *lrcHeader = "[ti:None]\n"
                            "[ar:None]\n"
                            "[al:None]\n"
                            "[by:TCAX Creator]";
    return
#ifdef Q_OS_WIN32
    QString(lrcHeader).replace(QT_NOR_EOL, QT_OTR_EOL);
#else
    QString(lrcHeader);
#endif
}

inline QString AssHelper::toAssEventsLine(qint64 a_startTimeMsec, qint64 a_endTimeMsec, QString text, bool a_isComment)
{
    return QString("%1: 0,%2,%3,Default,,0,0,0,,%4").arg(a_isComment ? ASS_TAG_EVENT_FORMAT_COMMENT : ASS_TAG_EVENT_FORMAT_DIALOGUE)
            .arg(toTimecode(a_startTimeMsec)).arg(toTimecode(a_endTimeMsec)).arg(text);
}

inline QString AssHelper::stripAssTag(QString a_text)
{
    return a_text.replace(QRegExp(ASS_TAG_BRACE_REGEXP), QT_EMPTY)
            .replace(ASS_TAG_HARD_EOL, QT_NOR_EOL)
            .replace(ASS_TAG_SOFT_EOL, QT_NOR_EOL)
            .replace(ASS_TAG_BLANK, QT_BLANK);
}

inline qint64 AssHelper::assTimeToLrcTime(const QString &timecode)
{
    //timecode = a_eventsList.at(static_cast<int>(AssEvents::Start))
    qint64 startTimeMsec = qint64(eINDEX_NONE);
    const QRegularExpression rex("(\\d+)?:(\\d+)?:(\\d+)?(\\.\\d+)?(\\S+)?");
    QRegularExpressionMatch match = rex.match(timecode);

#ifdef QT_DEBUG
    qDebug() << "-->";
    for(int i = 0; i < match.capturedLength(); i++)
    {
        qDebug() << match.captured(i);
    }
    qDebug() << "<--";
#endif
    if(match.captured(0).isEmpty())
    {
        return startTimeMsec;
    }
    else
    {
        int hour = match.captured(1).toInt();
        int min = match.captured(2).toInt();
        int sec = match.captured(3).toInt();
        double msec = match.captured(4).toDouble();

        startTimeMsec = static_cast<int>((hour * MINUTE * MINUTE + min * MINUTE + sec + msec) * SECOND_TO_MILLISECOND_UNIT);
    }
    return startTimeMsec;
}

inline QStringList AssHelper::splitAssEvents(const QString &a_assLine)
{
    QStringList eventsFirstSplited = stripAssTag(a_assLine).split(ASS_TAG_COMMA);
    QStringList events;

    if(!a_assLine.startsWith(ASS_TAG_EVENT_FORMAT_DIALOGUE) && !a_assLine.startsWith(ASS_TAG_EVENT_FORMAT_COMMENT))
    {
        return events;
    }

    if(eventsFirstSplited.length() < static_cast<int>(AssEvents::MaxEvent))
    {
        return events;
    }

    else
    {
        QString text = QT_EMPTY;

        if(eventsFirstSplited.length() == static_cast<int>(AssEvents::MaxEvent))
        {
            text = eventsFirstSplited.at(static_cast<int>(AssEvents::Text));
        }
        else if(eventsFirstSplited.length() > static_cast<int>(AssEvents::MaxEvent))
        {
            QStringList texts;

            for(int i = static_cast<int>(AssEvents::Text); i < eventsFirstSplited.length(); i++)
            {
                texts.append(eventsFirstSplited.at(i));
            }
            text = texts.join(ASS_TAG_COMMA);
        }

        for(int i = static_cast<int>(AssEvents::Layer); i < static_cast<int>(AssEvents::Text); i++)
        {
            events.append(eventsFirstSplited.at(i));
        }
        events.append(text);
    }

    return events;
}

QList<QPair<qint64, QString>> AssHelper::assParse(const QString &a_str)
{
    QList<QPair<qint64, QString>> assParsedList;
    QString assStr = a_str;
    QStringList assLines = assStr.remove(QT_MAC_EOL).split(QT_NOR_EOL);

    if(assLines.isEmpty())
    {
        return assParsedList;
    }

    for(int i = eINDEX_0; i < assLines.length(); i++)
    {
        QString assLine = assLines.at(i);
        QStringList events = splitAssEvents(assLine);

        if(events.length() >= static_cast<int>(AssEvents::MaxEvent))
        {
            qint64 startTimeMsec = assTimeToLrcTime(events.at(static_cast<int>(AssEvents::Start)));
            qint64 endTimeMsec = assTimeToLrcTime(events.at(static_cast<int>(AssEvents::End)));
            QString text = events.at(static_cast<int>(AssEvents::Text));

            if(startTimeMsec < qint64(eINDEX_0))
            {
                continue;
            }

            assParsedList.append( { startTimeMsec, text });

            /* No making times continued. */
            if(i < assLines.length() && endTimeMsec >= qint64(eINDEX_0))
            {
                QString assLineNext = assLines.at(i + eINDEX_1);
                QStringList events = splitAssEvents(assLineNext);

                if(events.length() >= static_cast<int>(AssEvents::MaxEvent))
                {
                    qint64 startTimeMsecNext = assTimeToLrcTime(events.at(static_cast<int>(AssEvents::Start)));

                    if(endTimeMsec != startTimeMsecNext && startTimeMsec >= qint64(eINDEX_0))
                    {
                        assParsedList.append( { endTimeMsec, NULLSTR });
                    }
                }
            }
        }
    }
    return assParsedList;
}

QList<QPair<qint64, QString>> AssHelper::textParse(const QString &a_str)
{
    QList<QPair<qint64, QString>> textParsedList;
    QString textStr = a_str;
    QStringList textLines = textStr.remove(QT_MAC_EOL).split(QT_NOR_EOL);
    qint64 startTimeMsecTemp = eINDEX_0;

    for(QString textLine : textLines)
    {
        textParsedList.append( { startTimeMsecTemp, textLine });
        startTimeMsecTemp += eINDEX_5 * SECOND_TO_MILLISECOND_UNIT;
    }
    return textParsedList;
}
