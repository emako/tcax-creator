#ifndef YUVDIALOG_H
#define YUVDIALOG_H

#include "../com/common.h"

#include <QDialog>
#include <QIcon>

namespace Ui {
    class YuvDialog;
}

class YuvDialog : public QDialog
{
    Q_OBJECT

public:
    explicit YuvDialog(QDialog *parent = nullptr);
    ~YuvDialog();
    void saveYuv(void);
    void setYuv(int y, int u, int v);
    void setGroupTitle(QString title);
    int y;
    int u;
    int v;

private:
    Ui::YuvDialog *ui;

};
#endif // YUVDIALOG_H
