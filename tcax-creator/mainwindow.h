#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "com/common.h"
#include "com/config.h"
#include "com/style_sheet.h"
#include "com/tcax_helper.h"
#include "com/lua_helper.h"
#include "com/preferences_dialog.h"
#include "std/std_manager.h"
#include "tools/ass_ktag_typer.h"
#include "tools/ass_ttag_looper.h"
#include "tools/ass_conv.h"
#include "editor/py_syntax_highlighter.h"

#include <QMainWindow>
#include <QColorDialog>
#include <QDesktopServices>
#include <QUrl>
#include <QFontMetrics>
#include <QMessageBox>
#include <QActionGroup>

#define WEB_PAGE_PROP "web_page_prop"
#define TOOLS_PROP "tools_prop"

using namespace StyleSheet;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setFolder(const QString &a_folder);
    QString folder(void);

    void setLoadedFile(const QString &a_loadedFile);
    QString loadedFile(void);

private slots:
    void openProject(void);
    void openProject(QString a_folder);
    void reloadFile(const QFileInfo &a_fileInfo);

private:
    void execArgument(void);
    void loadFonts(void);
    void setSyntax(void);
    void setActions(void);
    void clearSyntax(void);
    void updateCompilerIcon(void);
    QFileInfoList currentFolderTcc(void);
    void showMessage(const QString &a_message, const QColor &a_color = Qt::red);
    void getConfigRecent(void);
    void setConfigRecent(void);

private slots:
    void keyPressEvent(QKeyEvent *event);
    void closeEvent(QCloseEvent *event);
    void dragEnterEvent(QDragEnterEvent* event);
    void dropEvent(QDropEvent* event);
    void clearMsg(void);
    void fileChanged(void);
    void newFile(void);
    void openFolder(void);
    void save(void);
    void openTools(void);
    void projectCompile(void);
    void projectClean(void);
    void projectExplore(void);
    void tcaxExplore(void);
    QPair<QString, QFileInfo> selectedCompiler(void);
    void slotAbout(void);
    void openWebs(void);
    void slotFileSystem(void);
    void addRecent(const QString &a_recent);
    bool isSaved(void); /// unused
    void setSaved(bool a_isSaved); /// unused

private slots:
    void on_folderTreeView_activated(const QModelIndex &a_index);
    void on_folderTreeView_customContextMenuRequested(const QPoint &a_pos);

private:
    Ui::MainWindow *ui;
    QMenu *m_pFileSystemMenu;
    QString m_folder;
    QString m_loadedFile;
    QTimer *m_pTimerMsg;
    QFileSystemModel *m_model;
    QFileSystemWatcher *m_fsWatcher;
    PySyntaxHighlighter *m_pSyntaxHighlighter;
    QActionGroup *m_pActionGroupRecent;
};

#endif // MAINWINDOW_H
