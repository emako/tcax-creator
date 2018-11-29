#ifndef KASS_H
#define KASS_H

#include "ass_tag.h"
#include "../com/common.h"
#include "ass_syntax_highlighter.h"

#include <QWidget>
#include <QAbstractButton>
#include <QMenu>
#include <QDebug>
#include <QTime>
#include <QRegExp>
#include <QFontDatabase>
#include <QClipboard>
#include <QMessageBox>
#include <QFileDialog>
#include <QIODevice>
#include <QFile>
#include <QByteArray>
#include <QTextStream>
#include <QResource>
#include <QSize>
#include <QDesktopServices>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QCloseEvent>
#include <QMimeData>
#include <QUrl>
#include <QScrollBar>
#include <QLocale>
#include <QTranslator>

namespace Ui {
    class AssKtagTyper;
}

class AssKtagTyper : public QWidget
{
    Q_OBJECT

public:
    explicit AssKtagTyper(QWidget *parent = nullptr);
    ~AssKtagTyper() override;

    void reload(const QString &path);
    void setEditorHighlightSynth(AssSyntaxHighlighter *highlighter);
    QString getSavePath(QString path);
    QString callAssKtagTyper(QString rv_text);
    float calcTextLength(QString text);
    QString calcAssKtagTyperValue(QString start, QString end, float length);
    QString stripTag(QString text);
    QString getAssText(QStringList events);

private slots:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

    void saveDiskFile(void);
    void saveAsDiskFile(void);
    void save(const QString &a_path);
    void pasteText(void);
    void slotTextEditSliderVertical(int value);
    void slotTextEditSliderHorizontal(int value);
    void slotTextEditMenuRequested(void);
    void slotHighlight(void);
    void slotActions(bool a_checked);

    void convert();
    void copy();
    void on_karaokeComboBox_activated(int index);
    void on_otherTagCheckBox_stateChanged(int arg1);

private:
    void setup(void);
    void initParm(void);
    void setActions(void);
    void setHighlight(void);

private:
    Ui::AssKtagTyper *ui;
    QClipboard *m_clipboard;
    AssSyntaxHighlighter *m_input_highlighter;
    AssSyntaxHighlighter *m_output_highlighter;
    QMenu *m_pTextEditMenu;
    QString input_path;
    bool isTimeLimted;
    int kCount;
};


#endif // KASS_H
