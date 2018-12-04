#ifndef ASSTTAG_LOOPER_H
#define ASSTTAG_LOOPER_H

#include "ass_tag.h"
#include "../com/common.h"

#include <QWidget>
#include <QTime>
#include <QDebug>

namespace Ui {
    class AssTtagLooper;
}

class AssTtagLooper : public QWidget
{
    Q_OBJECT

public:
    explicit AssTtagLooper(QWidget *parent = nullptr);
    ~AssTtagLooper();

    void setEndTime(QString text);
    QString getPrevEffectText(void);
    QString tt(int start_time, int end_time, int fx_time, QString tag1, QString tag2);
    QList<int> range(int start, int end, int step);

private slots:
    void on_buttonMake_clicked();
    void on_buttonClose_clicked();

private:
    void setup(void);

private:
    Ui::AssTtagLooper *ui;
    QString prev_calc_text;
};
#endif // ASSTTAG_LOOPER_H
