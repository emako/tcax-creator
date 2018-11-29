#include "ass_conv.h"
#include "ui_ass_conv.h"

AssConv::AssConv(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AssConv),
    m_pContextMenu(nullptr)
{
    ui->setupUi(this);
    this->setup();
}

AssConv::~AssConv()
{
    delete m_pContextMenu;
    delete ui;
}

void AssConv::setup(void)
{
    this->setFont(DEFAULT_FONT);
    this->setAttribute(Qt::WA_DeleteOnClose, true);
    this->setWindowTitle(tr("ASS Convertor"));
    this->setAcceptDrops(true);
    ui->listWidget->setAcceptDrops(false);
    ui->listWidget->connect(ui->listWidget, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(slotContextMenuRequested()));

    /* Content Menu */
    USING SCOPE
    {
        QJsonObject json;
        QWidget *action_parent = ui->listWidget;
        QList<QPair<QString, QAction *>> action_list = {
            ACTION_NEW(ACTION_CASS_MENU_OPEN, ":/buttons/folder_page_white.png", action_parent),
        };

        m_pContextMenu = new QMenu(action_parent);

        for(int i = 0; i < action_list.length(); i++)
        {
            QString key = action_list.at(i).first;
            QAction *value = action_list.at(i).second;

            if( (key == NULLSTR) || (value == nullptr) )
            {
                m_pContextMenu->addSeparator();
            }
            else
            {
                m_pContextMenu->addAction(value);
                value->setProperty(ACTION_PROP_ID, key);
                value->connect(value, SIGNAL(triggered()), this, SLOT(slotContextMenu()));
            }
        }
    }

    ui->buttonConvert->connect(ui->buttonConvert, SIGNAL(clicked()), this, SLOT(convert()));
}

void AssConv::slotContextMenu(void)
{
    QString id = sender()->property(ACTION_PROP_ID).toString();

    if(id == ACTION_CASS_MENU_OPEN)
    {
        getOpens();
    }
}

void AssConv::getOpens(void)
{
    QStringList filelist = QFileDialog::getOpenFileNames(this, tr("Open Files"), "/", tr("Supported Files (*.ass *.ssa *.txt *.lrc *.krc)"));

    if(filelist.isEmpty())
    {
        return;
    }
    reload(filelist);
}

void AssConv::reload(const QStringList &a_filelist)
{
    if(!a_filelist.isEmpty())
    {
        ui->listWidget->clear();
        ui->listWidget->addItems(a_filelist);
    }
}

void AssConv::dragEnterEvent(QDragEnterEvent* event)
{
    if(event->mimeData()->hasText())
    {
        event->acceptProposedAction();
    }
}

void AssConv::dropEvent(QDropEvent* e)
{
    const QMimeData* mime = e->mimeData();

    if(mime->hasUrls())
    {
        QStringList filelist;

        foreach(QUrl url, e->mimeData()->urls())
        {
            QFileInfo file(url.toLocalFile());
            QString filename = QDir::toNativeSeparators(file.canonicalFilePath());

            if(!QFileInfo(filename).isFile())
            {
                continue;
            }
            if(isEnabledInput(filename))
            {
                filelist << filename;
            }
        }
        reload(filelist);
    }
    e->acceptProposedAction();
}

bool AssConv::isEnabledInput(const QString &a_filename, bool a_isIgnore)
{
    bool isEnabledInput = false;
    QString ext = QFileInfo(a_filename).suffix().toLower();
    static const QMap<AssConvType, QStringList> enableExtMap = {
        { Ass2Lrc, { EXT_ASS, EXT_SSA, } },
        { Ass2Txt, { EXT_ASS, EXT_SSA, } },
        { Lrc2Ass, { EXT_LRC, EXT_KRC, } },
        { Txt2Ass, { EXT_TXT, } },
    };

    if(a_isIgnore)
    {
        for(QMap<AssConvType, QStringList>::const_iterator i = enableExtMap.constBegin(); i != enableExtMap.constEnd(); i++)
        {
            for(const QString &enableExt : i.value())
            {
                if(ext == enableExt)
                {
                    isEnabledInput = true;
                    break;
                }
            }
            if(isEnabledInput)
            {
                break;
            }
        }
    }
    else
    {
        for(const QString &enableExt : enableExtMap[static_cast<AssConvType>(ui->comboBoxFormat->currentIndex())])
        {
            if(ext == enableExt)
            {
                isEnabledInput = true;
                break;
            }
        }
    }
    return isEnabledInput;
}

QString AssConv::getOutputExt(void)
{
    QString ext;

    switch(static_cast<AssConvType>(ui->comboBoxFormat->currentIndex()))
    {
    case Ass2Lrc:
        ext = EXT_LRC;
        break;
    case Ass2Txt:
        ext = EXT_TXT;
        break;
    case Lrc2Ass:
        ext = EXT_ASS;
        break;
    case Txt2Ass:
        ext = EXT_ASS;
        break;
    }
    return ext;
}

void AssConv::convert(void)
{
    QStringList checkFileList;
    QList<QPair<QString, QString>> outputFileList;

    for(int i = eINDEX_0; i < ui->listWidget->count(); i++)
    {
        QString srcFile = ui->listWidget->item(i)->text();

        if(isEnabledInput(srcFile, false))
        {
            QString tarFile = QString("%1_out.%2").arg(Common::delFileExt(srcFile)).arg(getOutputExt());

            outputFileList.append( { srcFile, tarFile } );

            if(QFileInfo(tarFile).exists())
            {
                checkFileList << tarFile;
            }
        }
    }

    if(outputFileList.isEmpty())
    {
        QMessageBox::information(this, MSG_INFORMATION, tr("None support file exists."));
        return;
    }

    if(!checkFileList.isEmpty())
    {
        QString message = tr("Files exists. Overwrite all?");
        int ret = QMessageBox::question(this, MSG_QUESTION, tr("%1\nExisting File list:\n%2").arg(message).arg(checkFileList.join(QT_NOR_EOL)));

        if(ret == QMessageBox::No)
        {
            return;
        }
    }

    for(const QPair<QString, QString> &outputFile : outputFileList)
    {
        switch(static_cast<AssConvType>(ui->comboBoxFormat->currentIndex()))
        {
        case Ass2Lrc:
            convertAssToLrc(outputFile.first, outputFile.second);
            break;
        case Ass2Txt:
            convertAssToTxt(outputFile.first, outputFile.second);
            break;
        case Lrc2Ass:
            convertLrc2Ass(outputFile.first, outputFile.second);
            break;
        case Txt2Ass:
            convertTxt2Ass(outputFile.first, outputFile.second);
            break;
        }
    }
    QMessageBox::information(this, MSG_INFORMATION, tr("Conversion completed."));
}

void AssConv::convertAssToLrc(const QString &a_srcFile, const QString &a_tarFile)
{
    if(!AssHelper::assToLyric(a_srcFile, a_tarFile))
    {
        MESSAGE_BOX_CONVERT_FAILD(this, a_srcFile, a_tarFile);
    }
}

void AssConv::convertAssToTxt(const QString &a_srcFile, const QString &a_tarFile)
{
    if(!AssHelper::assToText(a_srcFile, a_tarFile))
    {
        MESSAGE_BOX_CONVERT_FAILD(this, a_srcFile, a_tarFile);
    }
}

void AssConv::convertLrc2Ass(const QString &a_srcFile, const QString &a_tarFile)
{
    bool saved = false;

    if(Common::getFileExt(a_srcFile) == EXT_KRC)
    {
        saved = LuaHelper::lyricToAss(a_srcFile, a_tarFile);
    }
    else
    {
        saved = AssHelper::lyricToAss(a_srcFile, a_tarFile);
    }

    if(!saved)
    {
        MESSAGE_BOX_CONVERT_FAILD(this, a_srcFile, a_tarFile);
    }
}

void AssConv::convertTxt2Ass(const QString &a_srcFile, const QString &a_tarFile)
{
    if(!AssHelper::textToAss(a_srcFile, a_tarFile))
    {
        MESSAGE_BOX_CONVERT_FAILD(this, a_srcFile, a_tarFile);
    }
}

void AssConv::slotContextMenuRequested(void)
{
    m_pContextMenu->exec(QCursor::pos());
}
