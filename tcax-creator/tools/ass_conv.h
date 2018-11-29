#ifndef ASS_CONV_H
#define ASS_CONV_H

#include "ass_tag.h"
#include "../com/lua_helper.h"
#include "../com/common.h"
#include "../com/ass_helper.h"

#include <QWidget>
#include <QMessageBox>
#include <QMenu>

#define MESSAGE_BOX_CONVERT_FAILD(parent, src, target) QMessageBox::warning(parent, tr("Failed"), tr("Convert Failed!\nSource file: \"%1\"\nTarget file:  \"%2\"").arg(src).arg(target))

namespace Ui {
class AssConv;
}

class AssConv : public QWidget
{
    Q_OBJECT

public:
    explicit AssConv(QWidget *parent = nullptr);
    ~AssConv();
    void reload(const QStringList &a_filelist);

    enum AssConvType {
        Ass2Lrc,
        Ass2Txt,
        Lrc2Ass,
        Txt2Ass,
    };

private slots:
    void slotContextMenuRequested(void);
    void dragEnterEvent(QDragEnterEvent* event);
    void dropEvent(QDropEvent* e);
    void slotContextMenu(void);
    void convert(void);
    void getOpens(void);

private:
    void setup(void);
    bool isEnabledInput(const QString &a_filename, bool a_isIgnore = true);
    QString getOutputExt(void);
    void convertAssToLrc(const QString &a_srcFile, const QString &a_tarFile);
    void convertAssToTxt(const QString &a_srcFile, const QString &a_tarFile);
    void convertLrc2Ass(const QString &a_srcFile, const QString &a_tarFile);
    void convertTxt2Ass(const QString &a_srcFile, const QString &a_tarFile);

private:
    Ui::AssConv *ui;
    QMenu *m_pContextMenu;
};

#endif // ASS_CONV_H
