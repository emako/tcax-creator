#ifndef PREFERENCES_DIALOG_H
#define PREFERENCES_DIALOG_H

#include "common.h"
#include "config.h"

#include <QDialog>
#include <QStyleFactory>

namespace Ui {
class Preferences;
}

class Preferences : public QDialog
{
    Q_OBJECT

public:
    explicit Preferences(QDialog *parent = nullptr);
    ~Preferences();

private slots:
    void slotConfigChanged(int a_arg1);
    void on_buttonReset_clicked();
    void on_buttonClose_clicked();

private:
    void setup(void);
    void loadCommonConfig(void);

private:
    Ui::Preferences *ui;
};

#endif // PREFERENCES_DIALOG_H
