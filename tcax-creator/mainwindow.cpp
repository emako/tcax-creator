#include "mainwindow.h"
#include "designer/script_designer.h"
#include "designer/tcc_designer.h"
#include "designer/tcas_designer.h"
#include "ui_mainwindow.h"
#include "ui_script_designer.h"
#include "ui_tcc_designer.h"
#include "ui_tcas_designer.h"

extern QMap<QUuid, StdWatcher*> g_pStdWatch;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_pFileSystemMenu(nullptr),
    m_pTimerMsg(new QTimer(this)),
    m_model(new QFileSystemModel(this)),
    m_fsWatcher(new QFileSystemWatcher),
    m_pActionGroupRecent(new QActionGroup(this))
{
    if(g_pConfig->getConfig(Config::eCONFIG_COMMON_STYLE_FACTORY_ENABLE).toBool())
    {
        QApplication::setStyle(g_pConfig->getConfig(Config::eCONFIG_COMMON_STYLE_FACTORY).toString());
    }

    ui->setupUi(this);
    this->loadFonts();
    this->setSyntax();
    this->setActions();
    this->setAcceptDrops(true);
    this->setWindowTitle(tr("TCAX Creator"));
    this->setFocusPolicy(Qt::StrongFocus);

    ui->folderTreeView->setModel(m_model);
    ui->folderTreeView->setRootIndex(m_model->index(QDir::currentPath() + "/.."));
    ui->folderTreeView->setColumnHidden(1, true);
    ui->folderTreeView->setColumnHidden(2, true);
    ui->folderTreeView->setColumnHidden(3, true);
    this->getConfigRecent();

    ui->tabWidget->setFont(DEFAULT_FONT);
    ui->tabWidget->setCurrentWidget(ui->tabCode);
    ui->scriptDesigner->setMainUi(this);
    ui->scriptEditor->setLineWrapMode(QPlainTextEdit::NoWrap);
    ui->scriptEditor->setAcceptDrops(false);
    ui->scriptEditor->setTabStopDistance(QFontMetrics(ui->scriptEditor->font()).width(BLANKCHR) * 4);
    ui->statusBar->setPalette(QPalette(Qt::red, Qt::red, Qt::red, Qt::red, Qt::red, Qt::red, Qt::red));

    m_pTimerMsg->setSingleShot(true);
    m_pTimerMsg->setInterval(1500);
    m_pTimerMsg->connect(m_pTimerMsg, SIGNAL(timeout()), this, SLOT(clearMsg()));

    m_fsWatcher->connect(m_fsWatcher, &QFileSystemWatcher::fileChanged, this, &MainWindow::fileChanged);
    m_fsWatcher->connect(m_fsWatcher, &QFileSystemWatcher::directoryChanged, this, &MainWindow::fileChanged);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_pTimerMsg;
    delete m_model;
    delete m_fsWatcher;
    delete m_pSyntaxHighlighter;
    delete m_pActionGroupRecent;
    delete g_pConfig;
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    if(event->text() == "\u0013") // Ctrl+S
    {
        save();
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    StdManager::releaseStdWatchAll();
    this->setConfigRecent();
    g_pConfig->saveConfigAll();
    event->accept();
}

void MainWindow::setActions(void)
{
    /* File */
    USING SCOPE
    {
        const QList<QAction *> c_actions_new_file = {
            ui->actionTcc_File,
            ui->actionPython_File,
            ui->actionVSScript,
            ui->actionAVSScript,
            ui->actionASS_File,
        };

        const QHash<QString, QAction *> c_actions_file_tools = {
            { EXEC_NONE, ui->actionPreferences, },
            { EXEC_ASSOC, ui->actionAssoc, },
        };

        for(int i = 0; i != c_actions_new_file.length(); i++)
        {
            c_actions_new_file.at(i)->connect(c_actions_new_file.at(i), SIGNAL(triggered()), this, SLOT(newFile()));
        }

        for(QHash<QString, QAction *>::const_iterator i = c_actions_file_tools.begin(); i != c_actions_file_tools.end(); i++)
        {
            i.value()->setProperty(TOOLS_PROP, i.key());
            i.value()->connect(i.value(), SIGNAL(triggered()), this, SLOT(openTools()));
        }

        ui->actionOpenFolder->connect(ui->actionOpenFolder, SIGNAL(triggered()), this, SLOT(openFolder()));
        ui->actionSave->connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(save()));
        ui->actionExit->connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
    }END_OF_USING;

    /* Build */
    ui->actionCompile->connect(ui->actionCompile, SIGNAL(triggered()), this, SLOT(projectCompile()));
    ui->actionClean->connect(ui->actionClean, SIGNAL(triggered()), this, SLOT(projectClean()));
    ui->actionExplore->connect(ui->actionExplore, SIGNAL(triggered()), this, SLOT(projectExplore()));

    /* Tools */
    USING SCOPE
    {
        const QHash<QString, QAction *> c_actions_tools = {
            { EXEC_NONE, ui->actionKass, },
            { EXEC_NONE, ui->actionTass, },
            { EXEC_CAPTURE_COLOR, ui->actionCapColor, },
            { EXEC_NONE, ui->actionAssConvertor, },
        };

        for(QHash<QString, QAction *>::const_iterator i = c_actions_tools.begin(); i != c_actions_tools.end(); i++)
        {
            i.value()->setProperty(TOOLS_PROP, i.key());
            i.value()->connect(i.value(), SIGNAL(triggered()), this, SLOT(openTools()));
        }
    }END_OF_USING;

    /* Help */
    ui->actionAbout->connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(slotAbout()));

    USING SCOPE
    {
        const QHash<QString, QAction *> c_actions_web_page = {
            { "https://github.com/emako/tcax-creator",                 ui->actionGithub,      },
            { "http://www.tcax.org",                                   ui->actionForum,       },
            { "http://www.tcax.org/forum.php?mod=forumdisplay&fid=41", ui->actionTCAX_Manual, },
        };

        for(QHash<QString, QAction *>::const_iterator i = c_actions_web_page.begin(); i != c_actions_web_page.end(); i++)
        {
            i.value()->setProperty(WEB_PAGE_PROP, i.key());
            i.value()->connect(i.value(), SIGNAL(triggered()), this, SLOT(openWebs()));
        }

    }END_OF_USING;

    /* File System Menu */
    USING SCOPE
    {
        QJsonObject json;
        QWidget *action_parent = ui->folderTreeView;
        QList<QPair<QString, QAction *>> action_list = {
            ACTION_NEW(ACTION_FILE_SYSTEM_REMOVE, ":/buttons/cross_grey.png", action_parent),
            ACTION_NONE,
            ACTION_NEW(ACTION_FILE_SYSTEM_BUILD, RESOURCE_COMPILE_NONE, action_parent),
            ACTION_NEW(ACTION_FILE_SYSTEM_EXPLORE, ":/buttons/folder_magnify.png", action_parent),
        };

        m_pFileSystemMenu = new QMenu(action_parent);

        for(int i = 0; i < action_list.length(); i++)
        {
            QString key = action_list.at(i).first;
            QAction *value = action_list.at(i).second;

            if( (key == NULLSTR) || (value == nullptr) )
            {
                m_pFileSystemMenu->addSeparator();
            }
            else
            {
                m_pFileSystemMenu->addAction(value);
                value->setProperty(ACTION_PROP_ID, key);
                value->connect(value, SIGNAL(triggered()), this, SLOT(slotFileSystem()));
            }
        }
    }
}

void MainWindow::clearMsg(void)
{
    ui->statusBar->clearMessage();
}

void MainWindow::loadFonts(void)
{
    QFontDatabase::addApplicationFontFromData(Common::getResource(":/fonts/consola.ttf"));
}

void MainWindow::on_folderTreeView_activated(const QModelIndex &a_index)
{
    QFileInfo fileinfo = m_model->fileInfo(a_index);
    static const QStringList c_editable_file_suffix_filter = {
        EXT_TCC,
        EXT_ASS,
        EXT_SSA,
        EXT_PY,
        EXT_VPY,
        EXT_AVS,
        EXT_AVSI,
        EXT_TXT,
        EXT_LRC,
    };
    static const QStringList c_designable_file_suffix_filter = {
        EXT_TCC,
        EXT_TCAS,
    };

    if(QFileInfo(fileinfo.absoluteFilePath()).isFile())
    {
        QString filename = fileinfo.absoluteFilePath();
        QString ext = fileinfo.suffix().toLower();
        bool editable = false;
        bool designable = false;

        for(int i = 0; i < c_editable_file_suffix_filter.length(); i++)
        {
            if(ext == c_editable_file_suffix_filter.at(i))
            {
                editable = true;
                break;
            }
        }
        for(int i = 0; i < c_designable_file_suffix_filter.length(); i++)
        {
            if(ext == c_designable_file_suffix_filter.at(i))
            {
                designable = true;
                break;
            }
        }

        if(!editable && !designable)
        {
            showMessage(tr("Not supported format for \"%1\".").arg(fileinfo.fileName()));
            return;
        }
        else
        {
            setLoadedFile(filename);
        }

        if(g_pConfig->getConfig(Config::eCONFIG_COMMON_EDITOR_FILE_LIMIT_ENABLE).toBool())
        {
            int limitSize = g_pConfig->getConfig(Config::eCONFIG_COMMON_EDITOR_FILE_LIMIT_SIZE).toInt();

            if( (QFileInfo(filename).size() / MB) >= limitSize )
            {
                if(QMessageBox::Ok != QMessageBox::question(this, MSG_QUESTION, tr("File size is large then limit %1 MB.\nIt will take a while. Load sure?").arg(limitSize), QMessageBox::Ok|QMessageBox::No))
                {
                    return;
                }
            }
        }

        if(editable)
        {
            ui->scriptEditor->show();
            ui->scriptEditor->setPlainText(Common::getFileText(filename));

            if(ext == EXT_TCC)
            {
                ui->scriptDesigner->show();
            }
            else
            {
                ui->scriptDesigner->hide();
                ui->tabWidget->setCurrentWidget(ui->tabCode);
            }
        }
        if(designable)
        {
            ui->scriptDesigner->show();

            if(ext == EXT_TCC)
            {
                ui->scriptDesigner->reload(ScriptDesigner::TccDesign, filename);
            }
            else if(ext == EXT_TCAS)
            {
                ui->scriptEditor->clear();
                ui->scriptEditor->hide();
                ui->scriptDesigner->reload(ScriptDesigner::TcaxDesign, filename);
                ui->tabWidget->setCurrentWidget(ui->tabDesign);
            }
        }
    }
}

void MainWindow::fileChanged(void)
{
    /* Check folder exist */
    if(!QFileInfo(folder()).isDir())
    {
        ui->folderTreeView->setRootIndex(m_model->index(QDir::currentPath()));
    }
    updateCompilerIcon();
}

void MainWindow::dragEnterEvent(QDragEnterEvent* e)
{
    if(e->mimeData()->hasText())
    {
        e->acceptProposedAction();
    }
}

void MainWindow::setFolder(const QString &a_folder)
{
    m_folder = a_folder;
}

QString MainWindow::folder(void)
{
    return m_folder;
}

void MainWindow::setLoadedFile(const QString &a_loadedFile)
{
    m_loadedFile = a_loadedFile;
}

QString MainWindow::loadedFile(void)
{
    return m_loadedFile;
}

void MainWindow::openProject(void)
{
    if(sender() != nullptr)
    {
        openProject(static_cast<QAction *>(sender())->text());
    }
}

void MainWindow::openProject(QString a_folder)
{
    bool isAddRecent = true;

    if(a_folder == folder())
    {
        return;
    }

    this->setFolder(a_folder);
    m_model->setRootPath(QDir::currentPath());

    if(!folder().isEmpty())
    {
        m_fsWatcher->removePath(folder());
    }
    m_fsWatcher->addPath(a_folder);
    ui->folderTreeView->setRootIndex(m_model->index(a_folder));
    this->updateCompilerIcon();

    for(QAction *actionRecent : m_pActionGroupRecent->actions())
    {
        if(sender() == actionRecent)
        {
            isAddRecent = false;
            break;
        }
        if(folder() == actionRecent->text())
        {
            actionRecent->setChecked(true);
            isAddRecent = false;
            break;
        }
    }
    if(isAddRecent)
    {
        this->addRecent(folder());
    }

}

void MainWindow::dropEvent(QDropEvent* e)
{
    const QMimeData* mime = e->mimeData();

    if(mime->hasUrls())
    {
        foreach(QUrl url, e->mimeData()->urls())
        {
            QFileInfo file(url.toLocalFile());
            QString filename = file.canonicalFilePath();
            QPoint posTccSrc;
            QRect retTccSrc;
            QPoint posTccPy;
            QRect retTccPy;

            posTccSrc = QPoint( ui->tabWidget->pos()
                        + ui->centralWidget->pos()
                        + ui->scriptDesigner->pos()
                        + ui->scriptDesigner->ui->tccDesigner->pos()
                        + ui->scriptDesigner->ui->tccDesigner->ui->groupBoxSource->pos()
                        + QPoint(ui->menuBar->pos().x(), ui->menuBar->height())
                        + ui->scriptDesigner->ui->tccDesigner->ui->editSource->pos() );
            retTccSrc = QRect(posTccSrc, ui->scriptDesigner->ui->tccDesigner->ui->editSource->size());

            posTccPy = QPoint( ui->tabWidget->pos()
                        + ui->centralWidget->pos()
                        + ui->scriptDesigner->pos()
                        + ui->scriptDesigner->ui->tccDesigner->pos()
                        + ui->scriptDesigner->ui->tccDesigner->ui->groupBoxSource->pos()
                        + QPoint(ui->menuBar->pos().x(), ui->menuBar->height())
                        + ui->scriptDesigner->ui->tccDesigner->ui->editPython->pos() );
            retTccPy = QRect(posTccPy, ui->scriptDesigner->ui->tccDesigner->ui->editPython->size());

            if( ( ui->tabWidget->currentWidget() == ui->tabDesign )
             && ( retTccSrc.contains(e->pos()) || retTccPy.contains(e->pos()) )
             && ( !ui->scriptDesigner->ui->tccDesigner->isHidden() )
             && (!filename.isEmpty()) )
            {
                QString ext = QFileInfo(filename).suffix().toLower();

                if(retTccSrc.contains(e->pos()) && (ext == EXT_ASS || ext == EXT_SSA))
                {
                    ui->scriptDesigner->ui->tccDesigner->ui->editSource->setText(ui->scriptDesigner->ui->tccDesigner->getSrcFilename(filename));
                }
                else if(retTccPy.contains(e->pos()) && ext == EXT_PY)
                {
                    ui->scriptDesigner->ui->tccDesigner->ui->editPython->setText(ui->scriptDesigner->ui->tccDesigner->getSrcFilename(filename));
                }
                else
                {
                    showMessage(tr("Not supported format for \"%1\".").arg(QFileInfo(filename).fileName()));
                }
            }
            else if(QFileInfo(filename).isFile())
            {
                openProject(QDir::toNativeSeparators(QFileInfo(filename).absolutePath()));
            }
            else if(QFileInfo(filename).isDir())
            {
                openProject(QDir::toNativeSeparators(filename));
            }
            else
            {
                continue;
            }

            break;
        }
    }
    e->acceptProposedAction();
}

void MainWindow::setSyntax(void)
{
    m_pSyntaxHighlighter = new PySyntaxHighlighter(ui->scriptEditor->document());
}

void MainWindow::clearSyntax(void)
{
    m_pSyntaxHighlighter->clear();
}

void MainWindow::showMessage(const QString &a_message, const QColor &a_color)
{
    ui->statusBar->setPalette(QPalette(a_color, a_color, a_color, a_color, a_color, a_color, a_color));
    ui->statusBar->showMessage(a_message);

    if(m_pTimerMsg->isActive())
    {
        m_pTimerMsg->stop();
    }
    m_pTimerMsg->start(); // clear message on timeout
}

void MainWindow::newFile(void)
{
    static const QMap<QObject *, QPair<QString, QString>> extensionToFilterMap = {
        { ui->actionTcc_File,    { EXT_TCC, tr("Tcc File (*.tcc)")           } },
        { ui->actionPython_File, { EXT_PY,  tr("Python Script (*.py)")       } },
        { ui->actionVSScript,    { EXT_VPY, tr("Vapoursynth Script (*.vpy)") } },
        { ui->actionAVSScript,   { EXT_AVS, tr("Avisynth Script (*.avs)")    } },
        { ui->actionASS_File,    { EXT_ASS, tr("ASS File (*.ass)")           } },
    };

    static const QMap<QString, QString> extensionToTemplateMap = {
        { EXT_TCC, ":/strings/template/template.tcc" },
        { EXT_PY,  ":/strings/template/template.py"  },
        { EXT_VPY, ":/strings/template/template.vpy" },
        { EXT_AVS, ":/strings/template/template.avs" },
        { EXT_ASS, ":/strings/template/template.ass" },
    };

    QString fileExtension = extensionToFilterMap[sender()].first;
    QString selectedFilter = extensionToFilterMap[sender()].second;
    QString dir = folder() + QString("/%1.%2").arg(tr("new")).arg(fileExtension);
    QStringList saveFormatsList;
    QString filename;

    if(!QFileInfo(folder()).isDir())
    {
        QMessageBox::warning(this, MSG_WARNING, tr("Project Folder does not exist!\nPlease open project folder firstly."));
        return;
    }

    for(QMap<QObject *, QPair<QString, QString>>::const_iterator i = extensionToFilterMap.constBegin(); i != extensionToFilterMap.constEnd(); i++)
    {
        saveFormatsList << i.value().second;
    }

    filename = QFileDialog::getSaveFileName(this, tr("New File"), dir, saveFormatsList.join(";;"), &selectedFilter);

    if(!filename.isEmpty())
    {
        if(QFileInfo(filename).isFile())
        {
            QMessageBox::critical(this, MSG_CRITICAL, tr("\"%1\" already exists!\nPlease select the other path.").arg(filename));
            return;
        }
        if(!Common::setFileText(filename, Common::getFileText(extensionToTemplateMap[fileExtension])))
        {
            QMessageBox::critical(this, MSG_CRITICAL, tr("Save file failed!"));
            return;
        }
    }
}

void MainWindow::openFolder(void)
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Folder"));

    if(!dir.isEmpty())
    {
        openProject(dir);
    }
}

void MainWindow::openTools(void)
{
    if(sender() == ui->actionKass)
    {
        AssKtagTyper *kassUi = new AssKtagTyper();
        kassUi->show(); // deleted on closed
    }
    else if(sender() == ui->actionTass)
    {
        AssTtagLooper *tassUi = new AssTtagLooper();
        tassUi->show(); // deleted on closed
    }
    else if(sender() == ui->actionAssConvertor)
    {
        AssConv *assConvUi = new AssConv();
        assConvUi->show(); // deleted on closed
    }
    else if(sender() == ui->actionPreferences)
    {
        Preferences preferencesUi;
        preferencesUi.exec();
    }
    else if(sender() == ui->actionCapColor || sender() == ui->actionAssoc)
    {
        QString exec = Common::findFirstFilePath(sender()->property(TOOLS_PROP).toString());

        if(QFileInfo(exec.replace("\"", NULLSTR)).exists())
        {
            QProcess::startDetached(exec);
        }
        else
        {
            QMessageBox::warning(this, MSG_WARNING, tr("File %1 does not exist.\nPlease reinstall program.").arg(exec));
        }
    }
}

QPair<QString, QFileInfo> MainWindow::selectedCompiler(void)
{
    QPair<QString, QFileInfo> compiler;

    compiler.second = m_model->fileInfo(ui->folderTreeView->currentIndex());

    if(compiler.second.suffix().toLower() == EXT_TCC)
    {
        compiler.first = EXEC_TCAX;
    }
    else if(compiler.second.suffix().toLower() == EXT_PY)
    {
        compiler.first = EXEC_PYTHON;
    }
    return compiler;
}

void MainWindow::projectCompile(void)
{
    StdWatcherCmd cmd;
    QString execFile;
    QString targetFile;
    QFileInfoList tccList = currentFolderTcc();

    if(sender() == ui->actionCompile)
    {
        if(tccList.length() == eINDEX_0)
        {
            QMessageBox::warning(this, MSG_TCAX_WARNING, tr("None TCC file in project folder!"));
            return;
        }
        else if(tccList.length() == eINDEX_1)
        {
            targetFile = tccList.first().filePath();
            execFile = EXEC_TCAX;
        }
        else
        {
            QPair<QString, QFileInfo> compiler = selectedCompiler();

            if(!compiler.first.isEmpty())
            {
                execFile = compiler.first;
                targetFile = compiler.second.filePath();
            }
            else
            {
                QMessageBox::warning(this, MSG_TCAX_WARNING, tr("Multi TCC files existed in project folder!\nPlease select the target TCC files."));
                return;
            }
        }
    }
    else if(sender()->parent() == ui->folderTreeView)
    {
        QPair<QString, QFileInfo> compiler = selectedCompiler();

        if(!compiler.first.isEmpty())
        {
            execFile = compiler.first;
            targetFile = compiler.second.filePath();
        }
        else
        {
            return;
        }
    }

    cmd.cmd = QString("%1 \"%2\"").arg(Common::findFirstFilePath(execFile)).arg(targetFile);

    QUuid uid = StdManager::createStdWatch();

    g_pStdWatch[uid]->setFont(DEFAULT_FONT);
    g_pStdWatch[uid]->show();
    g_pStdWatch[uid]->initJob(uid);
    g_pStdWatch[uid]->startJob(cmd);
}

void MainWindow::projectClean(void)
{
    QString path = folder();
    QFileInfoList fileInfoList;

    if(path.isEmpty() || !QFileInfo(path).isDir())
    {
        return;
    }

    /* Remove token and pycache file. */
    for(QFileInfo fileInfo : Common::getFileList(path))
    {
        QString ext = fileInfo.suffix().toLower();

        if(ext == EXT_TOKEN)
        {
            if(Common::getFileText(fileInfo.filePath()).isEmpty())
            {
                fileInfoList << fileInfo;
                qDebug() << fileInfo.filePath();
            }
        }
        else if(ext == EXT_PYC)
        {
            if(fileInfo.absolutePath().split(QT_NOR_DIR_SPL).last() == DIR_PYCACHE)
            {
                fileInfoList << fileInfo;
                qDebug() << fileInfo.filePath();
            }
        }
    }

    for(QFileInfo file : fileInfoList)
    {
        QFile::remove(file.filePath());
    }

    /* Remove empty pycache folder. */
    for(QFileInfo fileInfo : Common::getFolderList(path))
    {
        if(fileInfo.filePath().split(QT_NOR_DIR_SPL).last() == DIR_PYCACHE)
        {
            if(QDir(fileInfo.filePath()).isEmpty())
            {
                fileInfo.dir().rmdir(fileInfo.filePath());
            }
        }
    }
}

void MainWindow::projectExplore(void)
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(QString("file:///%1").arg(folder())));
}

void MainWindow::slotAbout(void)
{
    QMessageBox::about(this, MSG_ABOUT, Common::fromResource(":/strings/about").arg(TCAX_CREATOR_VERSION));
}

void MainWindow::openWebs(void)
{
    QString url = sender()->property(WEB_PAGE_PROP).toString();

    if(!url.isEmpty())
    {
        QDesktopServices::openUrl(QUrl(url));
    }
}

void MainWindow::updateCompilerIcon(void)
{
    if(sender() == ui->folderTreeView)
    {
        QAction *action = nullptr;
        QString ext = QFileInfo(m_model->fileInfo(ui->folderTreeView->currentIndex())).suffix().toLower();

        for(QAction *tempAction : m_pFileSystemMenu->actions())
        {
            if(tempAction->property(ACTION_PROP_ID).toString() == ACTION_FILE_SYSTEM_BUILD)
            {
                action = tempAction;
                break;
            }
        }

        if(action == nullptr)
        {
            return;
        }

        if( (ext == EXT_TCC) || (ext == EXT_PY) )
        {
            action->setIcon(QIcon(RESOURCE_COMPILE));
        }
        else
        {
            action->setIcon(QIcon(RESOURCE_COMPILE_NONE));
        }
    }
    else
    {
        QFileInfoList tccList = currentFolderTcc();

        switch(tccList.length())
        {
        case eINDEX_0:
            ui->actionCompile->setIcon(QIcon(RESOURCE_COMPILE_NONE));
            break;
        case eINDEX_1:
            ui->actionCompile->setIcon(QIcon(RESOURCE_COMPILE));
            break;
        default:
            ui->actionCompile->setIcon(QIcon(RESOURCE_COMPILE_WARN));
            break;
        }
    }
}

QFileInfoList MainWindow::currentFolderTcc(void)
{
    QFileInfoList tccList;
    QDir dir(folder());
    QFileInfoList entryList = dir.entryInfoList();

    for (int i = 0; i < entryList.size(); i++)
    {
        QFileInfo fileInfo = entryList.at(i);

        if(fileInfo.suffix().toLower() == EXT_TCC)
        {
            tccList.append(fileInfo);
        }
    }
    return tccList;
}

void MainWindow::slotFileSystem(void)
{
    QFileInfo fileinfo = m_model->fileInfo(ui->folderTreeView->currentIndex());
    QString id = sender()->property(ACTION_PROP_ID).toString();

    if(id == ACTION_FILE_SYSTEM_REMOVE)
    {
        if(!Common::recycleFile(fileinfo.filePath(), this))
        {
            QMessageBox::warning(this, MSG_WARNING, tr("Remove file failed!"));
        }
    }
    else if(id == ACTION_FILE_SYSTEM_BUILD)
    {
        projectCompile();
    }
    else if(id == ACTION_FILE_SYSTEM_EXPLORE)
    {
        projectExplore();
    }
}

void MainWindow::on_folderTreeView_customContextMenuRequested(const QPoint &)
{
    updateCompilerIcon();
    m_pFileSystemMenu->exec(QCursor::pos());
}

void MainWindow::addRecent(const QString &a_recent)
{
    QAction *recentAction = new QAction(m_pActionGroupRecent);

    if(m_pActionGroupRecent->actions().length() > eINDEX_10)
    {
        QAction *recentActionFirst = m_pActionGroupRecent->actions().first();

        m_pActionGroupRecent->removeAction(recentActionFirst);
        delete recentActionFirst;
        recentActionFirst = nullptr;
    }
    m_pActionGroupRecent->addAction(recentAction);
    recentAction->setCheckable(true);
    recentAction->setChecked(true);
    recentAction->setText(a_recent);
    recentAction->setIcon(QIcon(":/buttons/folder.png"));
    recentAction->connect(recentAction, SIGNAL(triggered()), this, SLOT(openProject()));
    ui->menuRecent->addAction(recentAction);
}

void MainWindow::getConfigRecent(void)
{
    QJsonObject json = Common::toJson(g_pConfig->getConfig(Config::eCONFIG_COMMON_RECENT).toString());
    QJsonArray recentList = json.value(ACTION_JSON_RECENT_LIST).toArray();

    for(QJsonValue recent : recentList)
    {
        this->addRecent(recent.toString());
        if(recent.toString() == json.value(ACTION_JSON_RECENT).toString())
        {
            openProject(recent.toString());
        }
    }
}

void MainWindow::setConfigRecent(void)
{
    QJsonArray recentList;
    QJsonObject json;

    for(QAction *actionRecent : m_pActionGroupRecent->actions())
    {
        recentList << actionRecent->text();
    }
    json.insert(ACTION_JSON_RECENT, m_pActionGroupRecent->checkedAction()->text());
    json.insert(ACTION_JSON_RECENT_LIST, recentList);

    g_pConfig->setConfig(Config::eCONFIG_COMMON_RECENT, Common::fromJson(json).simplified().trimmed());
}

void MainWindow::save(void)
{
    bool savedOk = false;
    void *mode = ui->tabWidget->currentWidget();
    QString filename = loadedFile();
    QString ext = QFileInfo(filename).suffix().toLower();

    if(filename.isEmpty())
    {
        showMessage(tr("None file loaded."), Qt::blue);
        return;
    }

    if(mode == ui->tabCode)
    {
        savedOk = Common::setFileText(filename, ui->scriptEditor->toPlainText());

        if(savedOk)
        {
            if(ext == EXT_TCC)
            {
                ui->scriptDesigner->reload(ScriptDesigner::TccDesign, filename);
            }
            showMessage(tr("File \"%1\" saved.").arg(filename), Qt::blue);
        }
        else
        {
            showMessage(tr("File \"%1\" saving failed.").arg(filename), Qt::red);
        }
    }
    else if(mode == ui->tabDesign)
    {
        if(ext == EXT_TCC)
        {
            savedOk = ui->scriptDesigner->ui->tccDesigner->save(filename);

            if(savedOk)
            {
                ui->scriptEditor->setPlainText(Common::getFileText(filename));
                showMessage(tr("File \"%1\" saved.").arg(filename), Qt::blue);
            }
            else
            {
                showMessage(tr("File \"%1\" saving failed.").arg(filename), Qt::red);
            }
        }
        else if(ext == EXT_TCAS)
        {
            showMessage(tr("None file was modified."), Qt::blue);
        }
    }

    if(!savedOk && ext != EXT_TCAS)
    {
        QMessageBox::warning(this, MSG_WARNING, tr("Save file \"%1\" failed!").arg(filename), QMessageBox::Ok);
    }
}

bool MainWindow::isSaved(void)
{
    bool ret = false;

    if(ui->scriptEditor->document()->isModified())
    {
        ret = true;
    }
    ui->scriptEditor->document()->setModified(false);
    qDebug() << ret;
    return ret;
}

void MainWindow::setSaved(bool a_isSaved)
{
    ui->scriptEditor->setProperty(SAVED_FLAG, a_isSaved);
    ui->scriptEditor->document()->setModified(false);
}
