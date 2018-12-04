#include "ass_ttag_looper.h"
#include "ui_ass_ttag_looper.h"

AssTtagLooper::AssTtagLooper(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AssTtagLooper)
{
    ui->setupUi(this);
    this->setup();
}

AssTtagLooper::~AssTtagLooper()
{
    delete ui;
}

void AssTtagLooper::setup(void)
{
    ui->textEdit->setWordWrapMode(QTextOption::WordWrap);
    this->setAttribute(Qt::WA_DeleteOnClose, true);
    this->setWindowTitle(tr("ASS T-tag Looper (TASS)"));
    this->setAcceptDrops(true);
    this->setFont(DEFAULT_FONT);
}

void AssTtagLooper::setEndTime(QString text)
{
    QStringList events = text.split(ASS_TAG_COMMA);

    prev_calc_text = text;
    if(events.length() < static_cast<int>(AssEvents::MaxEvent))
    {
        return;
    }

    /* Length OK. */
    QString start = events.at(static_cast<int>(AssEvents::Start));
    QString end = events.at(static_cast<int>(AssEvents::End));
    QTime startTime = QTime::fromString(start, ASS_TAG_TIMECODE_FORMAT);
    QTime endTime = QTime::fromString(end, ASS_TAG_TIMECODE_FORMAT);
    int elapsed_msecs = startTime.msecsTo(endTime);

    ui->endTimeSpinBox->setValue(elapsed_msecs);
}

QString AssTtagLooper::getPrevEffectText(void)
{
    return prev_calc_text;
}

void AssTtagLooper::on_buttonClose_clicked()
{
    this->close();
}

QString AssTtagLooper::tt(int start_time, int end_time, int fx_time, QString tag1, QString tag2)
{
    QString tags = tag1;
    bool change_tag = false;
    QList<int> array = range(start_time, end_time, fx_time);

    for(int i = 0; i < array.length(); i++)
    {
        int parm1 = array[i];
        int parm2 = parm1 + fx_time;

        if(change_tag)
        {
            QString tagAt = tag1;
            tag1 = tag2;
            tag2 = tagAt;
            change_tag = false;
        }
        tags += QString(ASS_TAG_STR_TT).arg(ASS_TAG_TT).arg(QString::number(parm1)).arg(QString::number(parm2)).arg(tag2);
        change_tag = true;
    }
    return tags;
}

QList<int> AssTtagLooper::range(int start, int end, int step)
{
    QList<int> array;

    for(int i = start; i <= end; i += step)
    {
        array << i;
    }
    return array;
}

void AssTtagLooper::on_buttonMake_clicked()
{
    int startTime = ui->startTimeSpinBox->value();
    int endTime = ui->endTimeSpinBox->value();
    int effectTime = ui->effectTimeSpinBox->value();

    QString tag1 = ui->initTagLineEdit->text();
    QString tag2 = ui->effectTagTineEdit->text();

    QString tag0 = ui->constTagLineEdit->text();
    QString text = ui->constTextLineEdit->text();

    QString tags = tt(startTime, endTime, effectTime, tag1, tag2);

    ui->textEdit->setText(QString(QT_STR_TAG_TEXT).arg(tag0).arg(tags).arg(text));
}
