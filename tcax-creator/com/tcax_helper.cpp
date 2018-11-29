#include "../tcax/helper/std.h"
#include "tcax_helper.h"

#pragma comment(lib, "Advapi32.lib")

TccDefMap TcaxHelper::tccDefaultMap(void)
{
    TccDefMap tccMap;

    /*< tcc file version */
    tccMap.insert(TCC_TCC_FILE_VERSION, TCC_TCC_VER_STR);
    /*< mode configuration */
    tccMap.insert(TCC_INHERIT_ASS_HEADER, false);
    tccMap.insert(TCC_K_MODE, TCC_SYLLABLE);
    tccMap.insert(TCC_TEXT_LAYOUT, TCC_HORIZONTAL);
    tccMap.insert(TCC_TCAXPY_INIT, false);
    tccMap.insert(TCC_TCAXPY_USER, false);
    tccMap.insert(TCC_TCAXPY_FIN, false);
    /*< main settings */
    tccMap.insert(TCC_K_TIMED_ASS_FILE, NULLSTR);
    tccMap.insert(TCC_FONT_FILE, NULLSTR);
    tccMap.insert(TCC_FONT_FACE_ID, 1);
    tccMap.insert(TCC_FONT_SIZE, 36);
    tccMap.insert(TCC_FX_WIDTH, 1280);
    tccMap.insert(TCC_FX_HEIGHT, 720);
    tccMap.insert(TCC_FX_FPS, 23.976);
    tccMap.insert(TCC_ALIGNMENT, 7);
    tccMap.insert(TCC_X_OFFSET, 30);
    tccMap.insert(TCC_Y_OFFSET, 15);
    tccMap.insert(TCC_SPACING, 0);
    tccMap.insert(TCC_SPACE_SCALE, 1.0f);
    tccMap.insert(TCC_FONT_FACE_NAME, NULLSTR);
    tccMap.insert(TCC_BORD, 1);
    tccMap.insert(TCC_SHAD, 0);
    tccMap.insert(TCC_PRIMARY_COLOR, TCC_COLOR_FFFFFF);
    tccMap.insert(TCC_SECONDARY_COLOR, TCC_COLOR_000000);
    tccMap.insert(TCC_OUTLINE_COLOR, TCC_COLOR_000000);
    tccMap.insert(TCC_BACK_COLOR, TCC_COLOR_000000);
    tccMap.insert(TCC_PRIMARY_ALPHA, 0);
    tccMap.insert(TCC_SECONDARY_ALPHA, 255);
    tccMap.insert(TCC_OUTLINE_ALPHA, 0);
    tccMap.insert(TCC_BACK_ALPHA, 255);
    /*< additional settings */
    tccMap.insert(TCC_BLUR, 0.0f);
    return tccMap;
}

QString TcaxHelper::tccStrip(QString tccString)
{
    tccString = tccString.trimmed();
    tccString = tccString.remove(QT_QUOTE);
    return tccString;
}

QString TcaxHelper::tccToAttrLine(QString a_attr, QString a_attrValue)
{
    return QString("< %1 = %2 >").arg(a_attr).arg(a_attrValue == NULLSTR ? NULLDIR : a_attrValue);
}

bool TcaxHelper::tccSave(const QString &a_filename, TccMap a_tccMap)
{
    TccPyFileList tccPyFileList = tccMapParseToTccPyFileList(a_tccMap);
    QStringList tccStringList;

    tccStringList << TCC_COMMENT_1; // "# tcc file version"
    tccStringList << NULLSTR;
    tccStringList << tccToAttrLine(TCC_TCC_FILE_VERSION, TCC_TCC_VER_STR);
    tccStringList << NULLSTR;
    tccStringList << NULLSTR;
    tccStringList << TCC_COMMENT_2; // "# mode configuration"
    tccStringList << NULLSTR;
    tccStringList << tccToAttrLine(TCC_INHERIT_ASS_HEADER, a_tccMap.value(TCC_INHERIT_ASS_HEADER).toString());
    tccStringList << tccToAttrLine(TCC_K_MODE, a_tccMap.value(TCC_K_MODE).toString());
    tccStringList << tccToAttrLine(TCC_TEXT_LAYOUT, a_tccMap.value(TCC_TEXT_LAYOUT).toString());
    tccStringList << tccToAttrLine(TCC_TCAXPY_INIT, a_tccMap.value(TCC_TCAXPY_INIT).toString());
    tccStringList << tccToAttrLine(TCC_TCAXPY_USER, a_tccMap.value(TCC_TCAXPY_USER).toString());
    tccStringList << tccToAttrLine(TCC_TCAXPY_FIN, a_tccMap.value(TCC_TCAXPY_FIN).toString());
    tccStringList << NULLSTR;
    tccStringList << NULLSTR;
    tccStringList << TCC_COMMENT_3; // "# py settings"
    tccStringList << NULLSTR;
    for(TccPyFileList::iterator i = tccPyFileList.begin(); i != tccPyFileList.end(); i++)
    {
        tccStringList << tccToAttrLine(TCC_PY_FILE, i.key())
                       + TCC_LINE_SPACE
                       + tccToAttrLine(TCC_BEG_LINE, i.value().first.toString())
                       + TCC_LINE_SPACE
                       + tccToAttrLine(TCC_END_LINE, i.value().second.toString());
    }
    tccStringList << NULLSTR;
    tccStringList << NULLSTR;
    tccStringList << TCC_COMMENT_4; // "# main settings"
    tccStringList << NULLSTR;
    tccStringList << tccToAttrLine(TCC_K_TIMED_ASS_FILE, a_tccMap.value(TCC_K_TIMED_ASS_FILE).toString());
    tccStringList << tccToAttrLine(TCC_FONT_FILE, a_tccMap.value(TCC_FONT_FILE).toString())
                   + TCC_LINE_SPACE
                   + tccToAttrLine(TCC_FONT_FACE_ID, a_tccMap.value(TCC_FONT_FACE_ID).toString())
                   + TCC_LINE_SPACE
                   + tccToAttrLine(TCC_FONT_SIZE, a_tccMap.value(TCC_FONT_SIZE).toString());
    tccStringList << tccToAttrLine(TCC_FX_WIDTH, a_tccMap.value(TCC_FX_WIDTH).toString())
                   + TCC_LINE_SPACE
                   + tccToAttrLine(TCC_FX_HEIGHT, a_tccMap.value(TCC_FX_HEIGHT).toString())
                   + TCC_LINE_SPACE
                   + tccToAttrLine(TCC_FX_FPS, a_tccMap.value(TCC_FX_FPS).toString());
    tccStringList << tccToAttrLine(TCC_ALIGNMENT, a_tccMap.value(TCC_ALIGNMENT).toString());
    tccStringList << tccToAttrLine(TCC_X_OFFSET, a_tccMap.value(TCC_X_OFFSET).toString())
                   + TCC_LINE_SPACE
                   + tccToAttrLine(TCC_Y_OFFSET, a_tccMap.value(TCC_Y_OFFSET).toString());
    tccStringList << tccToAttrLine(TCC_SPACING, a_tccMap.value(TCC_SPACING).toString())
                   + TCC_LINE_SPACE
                   + tccToAttrLine(TCC_SPACE_SCALE, a_tccMap.value(TCC_SPACE_SCALE).toString());
    tccStringList << NULLSTR;
    tccStringList << NULLSTR;
    tccStringList << TCC_COMMENT_5; // "# style settings"
    tccStringList << NULLSTR;
    tccStringList << tccToAttrLine(TCC_FONT_FACE_NAME, a_tccMap.value(TCC_FONT_FACE_NAME).toString());
    tccStringList << tccToAttrLine(TCC_BORD, a_tccMap.value(TCC_BORD).toString())
                   + TCC_LINE_SPACE
                   + tccToAttrLine(TCC_SHAD, a_tccMap.value(TCC_SHAD).toString());
    tccStringList << tccToAttrLine(TCC_FONT_FACE_NAME, a_tccMap.value(TCC_FONT_FACE_NAME).toString());
    tccStringList << tccToAttrLine(TCC_PRIMARY_COLOR, a_tccMap.value(TCC_PRIMARY_COLOR).toString())
                   + TCC_LINE_SPACE
                   + tccToAttrLine(TCC_SECONDARY_COLOR, a_tccMap.value(TCC_SECONDARY_COLOR).toString())
                   + TCC_LINE_SPACE
                   + tccToAttrLine(TCC_OUTLINE_COLOR, a_tccMap.value(TCC_OUTLINE_COLOR).toString())
                   + TCC_LINE_SPACE
                   + tccToAttrLine(TCC_BACK_COLOR, a_tccMap.value(TCC_BACK_COLOR).toString());
    tccStringList << tccToAttrLine(TCC_PRIMARY_ALPHA, a_tccMap.value(TCC_PRIMARY_ALPHA).toString())
                   + TCC_LINE_SPACE
                   + tccToAttrLine(TCC_SECONDARY_ALPHA, a_tccMap.value(TCC_SECONDARY_ALPHA).toString())
                   + TCC_LINE_SPACE
                   + tccToAttrLine(TCC_OUTLINE_ALPHA, a_tccMap.value(TCC_OUTLINE_ALPHA).toString())
                   + TCC_LINE_SPACE
                   + tccToAttrLine(TCC_BACK_ALPHA, a_tccMap.value(TCC_BACK_ALPHA).toString());
    tccStringList << NULLSTR;
    tccStringList << NULLSTR;
    tccStringList << TCC_COMMENT_6; // "# additional settings"
    tccStringList << NULLSTR;
    tccStringList << tccToAttrLine(TCC_BLUR, a_tccMap.value(TCC_BLUR).toString());
    tccStringList << NULLSTR;

    return Common::setFileText(a_filename, tccStringList.join(QT_OTR_EOL), true);
}

bool TcaxHelper::tccCheck(const QString &a_tccString)
{
    int flag = 0;
    int quote = 0;

    for (int i = 0; i < a_tccString.length(); i ++)
    {
        if ('\"' == a_tccString[i])
        {
            if (1 == flag)
            {
                return false;
            }
            else if (0 != flag)
            {
                quote = (0 == quote) ? 1 : 0;
            }
        }
        else if ('<' == a_tccString[i])
        {
            if (1 == quote)
            {
                return false;
            }
            else if (0 == flag)
            {
                flag = 1;
            }
            else
            {
                return false;
            }
        }
        else if ('=' == a_tccString[i])
        {
            if (0 == flag || 1 == quote)
            {
                continue;
            }
            else if (1 == flag)
            {
                flag = 2;
            }
            else
            {
                return false;
            }
        }
        else if ('>' == a_tccString[i])
        {
            if (1 == quote)
            {
                return false;
            }
            else if (2 == flag)
            {
                flag = 0;
            }
            else
            {
                return false;
            }
        }
        else if ('\n' == a_tccString[i] || '\r' == a_tccString[i])
        {
            if (0 != flag && 1 != quote)
            {
                return false;
            }
        }
    }
    if (0 != flag || 0 != quote)
    {
        return false;
    }
    return true;
}

TccPyFileList TcaxHelper::tccMapParseToTccPyFileList(TccMap a_tccMap)
{
    TccPyFileList tccPyFileList;
    TccMapItems valuesPyFile = a_tccMap.values(TCC_PY_FILE);
    TccMapItems valuesBegLine = a_tccMap.values(TCC_BEG_LINE);
    TccMapItems valuesEndLine = a_tccMap.values(TCC_END_LINE);

    if( ( valuesPyFile.length() == valuesBegLine.length() )
     && ( valuesBegLine.length() == valuesEndLine.length() ) )
    {
        for(int i = 0; i < valuesPyFile.length(); i++)
        {
#ifdef QT_DEBUG
            qDebug() << valuesPyFile.at(i) << valuesBegLine.at(i) << valuesEndLine.at(i);
#endif
            tccPyFileList.insert(valuesPyFile.at(i).toString(), { valuesBegLine.at(i), valuesEndLine.at(i) });
        }
    }
    return tccPyFileList;
}

TccMap TcaxHelper::tccParseToMap(const QString &a_filename)
{
    QString tccString = Common::getFileText(a_filename);
    TccDefMap tccDefMap = tccDefaultMap();
    TccMap tccMap;
    int indexBegin1 = 0;
    int indexEnd1 = 0;
    int indexBegin2 = 0;
    int indexEnd2 = 0;
    int flag = 0;
    int quote = 0;

    if(!tccCheck(tccString))
    {
        qDebug() << "Not tcc format from" << a_filename;
    }

    for (int i = 0; i < tccString.length(); i ++)
    {
        if ('\"' == tccString[i])
        {
            if (0 != flag) quote = (0 == quote) ? 1 : 0;
        }
        else if ('<' == tccString[i])
        {
            flag = 1;
            indexBegin1 = i + 1;
        }
        else if (1 == flag && 0 == quote && L'=' == tccString[i])
        {
            flag = 2;
            indexEnd1 = i;
            indexBegin2 = i + 1;
        }
        else if ('>' == tccString[i])
        {
            flag = 0;
            indexEnd2 = i;
            tccMap.insert(tccStrip(tccString.mid(indexBegin1, indexEnd1 - indexBegin1)),
                          tccStrip(tccString.mid(indexBegin2, indexEnd2 - indexBegin2)));
        }
    }

    for(TccDefMap::iterator i = tccDefMap.begin(); i != tccDefMap.end(); i++)
    {
        if(!tccMap.contains(i.key()))
        {
            tccMap.insert(i.key(), i.value());
        }
    }

    return tccMap;
}

void TcaxHelper::tcaxW2A(const wchar_t *a_uni, char **a_ansi)
{
    int size;
	char *sz;

    size = WideCharToMultiByte(0, 0, a_uni, -1, nullptr, 0, nullptr, nullptr);
    sz = static_cast<char *>(malloc(static_cast<unsigned int>(size) * sizeof(char)));
    WideCharToMultiByte(0, 0, a_uni, -1, sz, size, nullptr, nullptr);
    *a_ansi = sz;
}

bool TcaxHelper::tcasShiftTime(const QString &a_filename, int a_shiftTime, bool a_saveCopy)
{
    QString message;
    int ok = false;
    char *filename_ansi;

    tcaxW2A(Common::toWCharT(a_filename), &filename_ansi);

    if(a_saveCopy)
    {
        QFileInfo fileinfo(a_filename);
        QString filename_copy = fileinfo.absolutePath() + QDir::separator() + fileinfo.completeBaseName() + "_old.tcas";
        if(!QFile::copy(a_filename, filename_copy))
        {
            qDebug() << "Save copy failed!";
        }
    }

    switch(libtcas_file_shift_time(filename_ansi, a_shiftTime))
    {
    case tcas_error_success:
        ok = true;
		break;
    case tcas_error_file_type_not_support:
        ok = false;
        qDebug() << "Unknown file type!";
		break;
    default:
        ok = false;
        qDebug() << "Can not open the TCAS file.";
	}
    free(filename_ansi);

    return ok;
}

QString TcaxHelper::tcasInfo(const QString &a_filename)
{
    QString info;
    QString fileType;
	TCAS_File file;
    TCAS_Header header;
    char *filename_ansi;

    tcaxW2A(Common::toWCharT(a_filename), &filename_ansi);

    if (libtcas_open_file(&file, filename_ansi, tcas_file_open_existing) != tcas_error_success)
    {
        free(filename_ansi);
        qDebug() << QString("Cannot open the TCAS file - \"%1\"").arg(a_filename);
	}
    free(filename_ansi);

	libtcas_read_header(&file, &header, tcas_false);

    switch (GETHI16B(header.flag))
    {
	case TCAS_FILE_TYPE_RAW:
        fileType = fileTypeRaw;
		break;
	case TCAS_FILE_TYPE_COMPRESSED:
        fileType = fileTypeCompressed;
		break;
	case TCAS_FILE_TYPE_COMPRESSED_Z:
        fileType = fileTypeCompressedZ;
		break;
	case TCAS_FILE_TYPE_PARSED:
        fileType = fileTypeParsed;
		break;
	case TCAS_FILE_TYPE_PARSED_Z:
        fileType = fileTypeParsedZ;
		break;
	default:
        fileType = fileTypeUnknown;
		break;
    }
    info.sprintf("Filename: %s\nType:     %s\nSize:     %ix%i\nFPS:      %.3f (%i/%i)\nTime:     %d~%d\nChunks:   %d",
                 a_filename.toUtf8().data(), fileType.toUtf8().data(),
                 GETPOSX(header.resolution), GETPOSY(header.resolution),
                 (static_cast<double>(header.fpsNumerator) / static_cast<double>(header.fpsDenominator)), header.fpsNumerator, header.fpsDenominator,
                 header.minTime, header.maxTime, header.chunks);
    libtcas_close_file(&file);

    return info;
}

TCC_Attributes TcaxHelper::tccParse(const QString &a_filename)
{
    const char *filename = a_filename.toUtf8().data();
    wchar_t *uni = nullptr;
    int count;
    TCC_Attributes attr;

    memset(&attr, NULL, sizeof(attr));

    if(libtcc_read_file_to_unicode(filename, &uni, &count) == tcc_error_success)
    {
        if(libtcc_check_format(uni, count) == tcc_error_success)
        {
            if(libtcc_parse_tcc_string(uni, count, &attr) != tcc_error_success)
            {
                qDebug() << "Parse tcc file failed.";
            }
        }
        else
        {
            qDebug() << "Check tcc format failed.";
        }
    }
    else
    {
        qDebug() << "Read tcc file failed.";
    }

    free(uni);
    return attr;
}

bool TcaxHelper::tccSave(const QString &a_filename, TCC_Attributes a_attr)
{
    bool ok = false;
    wchar_t *tccString = nullptr;
    int count = 0;

    libtcc_convert_attributes_to_tcc_string(&a_attr, &tccString, &count);

    if(libtcc_write_tcc_string_to_file(a_filename.toUtf8().data(), tccString, count) == tcc_error_success)
    {
        ok = true;
    }
    free(tccString);

    return ok;
}

bool TcaxHelper::tccCreate(const QString &a_filename)
{
    bool ok = false;

    if(libtcc_create_default_tcc_file(a_filename.toUtf8().data()) == tcc_error_success)
    {
        ok = true;
    }
    return ok;
}

void TcaxHelper::tccFree(TCC_pAttributes a_pAttr)
{
    libtcc_free_attributes(a_pAttr);
}

QStringList TcaxHelper::fontList(void)
{
    HKEY  hKey;
    LONG  lRes;
    DWORD cbValues;
    DWORD cbSize = 256;
    TCHAR szFnBuf[256];
    QStringList strFilter;

    lRes = RegOpenKeyEx(HKEY_LOCAL_MACHINE, winFontRegRoot, 0, KEY_READ, &hKey);
    if (lRes != ERROR_SUCCESS)
    {
        return NULLSTRS;
    }
    RegQueryInfoKey(hKey, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, &cbValues, nullptr, nullptr, nullptr, nullptr);
    for (UINT i = 0; i < cbValues; i ++)
    {
        cbSize = 256;
        RegEnumValue(hKey, i, szFnBuf, &cbSize, nullptr, nullptr, nullptr, nullptr);
        strFilter << Common::fromWCharT(szFnBuf);
    }
    return strFilter;
}

QString TcaxHelper::fontFile(const QString &a_fontName)
{
    LONG lResult;
    HKEY hKey;
    DWORD dwType;
    DWORD cbSize = 256;
    wchar_t szFileName[256];

    lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, winFontRegRoot, 0, KEY_READ, &hKey);
    if (lResult != ERROR_SUCCESS)
    {
        return NULLSTR;
    }
    RegQueryValueEx(hKey, Common::toWCharT(a_fontName), nullptr, &dwType, (BYTE *)szFileName, &cbSize);
    return Common::fromWCharT(szFileName);
}

QStringList TcaxHelper::fontFace(const QString &a_fontName)
{
    QString fontName = a_fontName;
    QStringList faces;
    QStringList id_faces;

    if(a_fontName.right(10) == "(TrueType)" || a_fontName.right(10) == "(OpenType)")
    {
        fontName = a_fontName.left(a_fontName.length() - 10).trimmed();
    }
    faces = fontName.split("&");
    for(int i = 0; i < faces.length(); i++)
    {
        id_faces << QString("%1: %2").arg(i + 1).arg(faces.at(i).trimmed());
    }
    return id_faces;
}
