#include "common.h"
#include "config.h"
#include "../tools/ass_tag.h"

wchar_t *Common::toWCharT(QString a_string)
{
    return const_cast<wchar_t *>(reinterpret_cast<const wchar_t *>(a_string.utf16()));
}

QString Common::fromWCharT(wchar_t *a_wchart)
{
    return QString::fromWCharArray(a_wchart);
}

QString Common::getFileText(const QString &a_filename, const QString &a_codec)
{
    QFile file(a_filename);
    QTextStream in(&file);
    QString text;

    if(!a_codec.isEmpty())
    {
        in.setCodec(a_codec.toStdString().data());
    }
    if (!file.open(QIODevice::ReadOnly))
    {
        return text;
    }
    text = in.readAll();
    file.close();
    return text;
}

QStringList Common::getFileTextLines(const QString &a_filename, const QString &a_codec)
{
    QFile file(a_filename);
    QTextStream in(&file);
    QStringList textLines;

    if(!a_codec.isEmpty())
    {
        in.setCodec(a_codec.toStdString().data());
    }
    if (!file.open(QIODevice::ReadOnly))
    {
        return textLines;
    }
    while(!file.atEnd())
    {
        textLines << in.readLine();
    }

    file.close();
    return textLines;
}

bool Common::setFileText(const QString &a_filename, const QString &a_text, bool a_isBom)
{
    QFile file(a_filename);
    QTextStream in(&file);

    in.setCodec("UTF-8");
    in.setGenerateByteOrderMark(a_isBom);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        return false;
    }
    in << a_text;
    file.close();
    return true;
}

QByteArray Common::getResource(const QString &a_filename)
{
    QResource resource(a_filename);

    return QByteArray(reinterpret_cast<const char *>(resource.data()), static_cast<int>(resource.size()));
}

QString Common::fromResource(const QString &a_filename)
{
    return QString::fromUtf8(getResource(a_filename));
}

QJsonObject Common::toJson(const QString a_string)
{
    QJsonDocument json_document = QJsonDocument::fromJson(a_string.toLocal8Bit().data());

    if(json_document.isNull())
    {
        qDebug()<< "Illegal json string: "<< a_string.toLocal8Bit().data();
    }

    QJsonObject json = json_document.object();
    return json;
}

QJsonArray Common::toJsonArray(const QString a_string)
{
    QJsonDocument json_document = QJsonDocument::fromJson(a_string.toLocal8Bit().data());

    if(json_document.isNull())
    {
        qDebug()<< "Illegal json string: "<< a_string.toLocal8Bit().data();
    }

    QJsonArray jsonArray = json_document.array();
    return jsonArray;
}

QString Common::fromJson(const QJsonObject& a_json)
{
    return QString(QJsonDocument(a_json).toJson());
}

QString Common::fromJsonArray(const QJsonArray& a_jsonArray)
{
    return QString(QJsonDocument(a_jsonArray).toJson());
}

bool Common::recycleFile(const QString &a_filename, QWidget *a_pHwnd)
{
    bool ret = true;
    SHFILEOPSTRUCT opRecycle;

    opRecycle.hwnd              = (HWND)a_pHwnd->winId();
    opRecycle.wFunc             = FO_DELETE;
    opRecycle.pFrom             = toWCharT(a_filename);
    opRecycle.pTo               = L"\0";
    opRecycle.fFlags            = FOF_ALLOWUNDO;
    opRecycle.hNameMappings     = nullptr;

    if(SHFileOperation(&opRecycle) != 0)
    {
        ret = false;
    }
    if(opRecycle.fAnyOperationsAborted)
    {
        ret = true;
    }
    return ret;
}

QFileInfoList Common::getFolderList(QString a_path)
{
    QDir dir(a_path);
    QFileInfoList folder_list = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Dirs);

    return folder_list;
}

QFileInfoList Common::getFileList(QString a_path)
{
    QDir dir(a_path);
    QFileInfoList file_list = dir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    QFileInfoList folder_list = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

    for(int i = 0; i != folder_list.size(); i++)
    {
        QString name = folder_list.at(i).absoluteFilePath();
        QFileInfoList child_file_list = getFileList(name);

        file_list.append(child_file_list);
    }
    return file_list;
}

QString Common::findFirstFilePath(QString a_filename, QString a_path)
{
    QString path;
    QFileInfoList fileInfoList;

    if(a_path.isEmpty() || a_filename.isEmpty())
    {
        return path;
    }

    fileInfoList = getFileList(a_path);

    for(QFileInfo fileInfo : fileInfoList)
    {
        if( (fileInfo.fileName().toLower() == a_filename.toLower())
         || (fileInfo.baseName().toLower() == a_filename.toLower()) )
        {
            path = QString("\"%1\"").arg(QDir::toNativeSeparators(fileInfo.absoluteFilePath()));
            break;
        }
    }
    if(path.isEmpty())
    {
        path = a_filename;
    }
    return path;
}

QString Common::fromHtml(const QString &a_str)
{
    QPlainTextEdit editor;

    editor.appendHtml(a_str);
    return editor.toPlainText();
}

QString Common::toHtmlText(const QString &a_str)
{
    QString input = a_str;
    QString output;

    for(const QChar &ch : input)
    {
        switch (ch.unicode())
        {
        case '<':  output += QString::fromLatin1("&lt;");   break;
        case '>':  output += QString::fromLatin1("&gt;");   break;
        case '&':  output += QString::fromLatin1("&amp;");  break;
        case '"':  output += QString::fromLatin1("&quot;"); break;
        case '\'': output += QString::fromLatin1("&apos;"); break;
        case '\n': output += QString::fromLatin1("<br/>");  break;
        default:   output += ch;                            break;
        }
    }
    return output;
}

QString Common::toHtml(const QString &a_str, QColor a_color, bool logging)
{
    QString html;
    QString inputs = toNormalEol(a_str);
    QByteArray color = QByteArray().append(static_cast<char>(a_color.red()))
                                   .append(static_cast<char>(a_color.green()))
                                   .append(static_cast<char>(a_color.blue()));
    int count = eINDEX_0;

    for(QString input : inputs.split(QT_NOR_EOL))
    {
        if(logging)
        {
            html += QString("<span style=\"color:#000000;\">%1</span>").arg(toHtmlText(currentTime()));
        }
        input = toHtmlText(input);

        if(count != inputs.split(QT_NOR_EOL).length() - eINDEX_1)
        {
            input += toHtmlText(QT_NOR_EOL); /* No appending of EOL at last line. */
        }

        html += QString("<span style=\"color:#%1;\">%2</span>").arg(QString(color.toHex())).arg(input);
        count++;
    }
    return html;
}

QString Common::toNormalEol(const QString &a_str)
{
    QString str = a_str;

    str.replace(QT_OTR_EOL, QT_NOR_EOL);
    str.replace(QT_MAC_EOL, QT_NOR_EOL);
    return str;
}

QString Common::toDosEol(const QString &a_str)
{
    QString str = a_str;

    str.replace(QT_NOR_EOL, QT_OTR_EOL);
    return str;
}

QString Common::currentTime(void)
{
    return QString("[%1]").arg(QTime::currentTime().toString());
}

QString Common::currentDateTime(void)
{
    return QDateTime::currentDateTime().toString("yyyy-MM-dd, hh:mm:ss");
}


QString Common::creatTempFilename(const QString &a_ext)
{
    QString tempFilename;
    QString ext = a_ext;
    QFileInfo tempDir = QFileInfo(QFileInfo(g_pConfig->settingsFilePath()).path());

    if(ext.toLower() != EXT_TMP)
    {
        ext = QString("%1.%2").arg(EXT_TMP).arg(ext);
    }

    if(tempDir.isDir())
    {
        tempFilename = QString("%1/tcax-%2.%3").arg(tempDir.path()).arg(QUuid::createUuid().toString().remove("{").remove("}")).arg(ext);
    }
    return tempFilename;
}

QString Common::getFileExt(const QString &a_filename)
{
    QFileInfo file(a_filename);
    return file.suffix();
}

QString Common::chgFileExt(const QString &a_filename, QString a_ext)
{
    if(!a_ext.startsWith(QString(QT_EXT_SPLITE)))
    {
        a_ext = QString(QT_EXT_SPLITE) + a_ext;
    }
    return delFileExt(a_filename) + a_ext;
}

QString Common::delFileExt(const QString &a_filename)
{
    QFileInfo file(a_filename);
    QString filename = QDir::toNativeSeparators(file.absolutePath());

    filename += ((filename.right(eINDEX_1) == QDir::separator()) ? QT_EMPTY : QString(QDir::separator())) + file.completeBaseName();
    return filename;
}

bool Common::inRange(int a_index, int a_len)
{
    return (((a_index >= eINDEX_0) && (a_index < (a_len - eINDEX_1))) ? true : false);
}
