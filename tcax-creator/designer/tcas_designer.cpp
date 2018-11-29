#include "tcas_designer.h"
#include "ui_tcas_designer.h"

TcasDesigner::TcasDesigner(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TcasDesigner)
{
    ui->setupUi(this);
}

TcasDesigner::~TcasDesigner()
{
    delete ui;
}

void TcasDesigner::reload(const QString &a_filename)
{
    m_filename = a_filename;
    ui->labelInfo->setText(TcaxHelper::tcasInfo(a_filename));
}

void TcasDesigner::on_buttonStartShifting_clicked()
{
    QTime time = ui->editTimeShift->time();
    int shiftTime = time. hour() * 60 * 60 * 1000 + time.minute() * 60 * 1000 + time.second() * 1000 + time.msec();
    bool saveCopy = ui->checkBoxMakeCopy->isChecked();

    if(ui->radioButtonBackward->isChecked())
    {
        shiftTime = -shiftTime;
    }
    if(m_filename.isEmpty())
    {
        QMessageBox::critical(this, MSG_TCAX_CRITICAL, tr("File none!"));
        return;
    }
    else if(!QFileInfo(m_filename).isFile())
    {
        QMessageBox::critical(this, MSG_TCAX_CRITICAL, tr("\"%1\" is not a file!").arg(m_filename));
        return;
    }
    if(TcaxHelper::tcasShiftTime(m_filename, shiftTime, saveCopy))
    {
        QMessageBox::information(this, MSG_TCAX_INFORMATION, tr("Tcas Shift Time Completed!"));
    }
    else
    {
        QMessageBox::critical(this, MSG_TCAX_CRITICAL, tr("Tcas Shift Time Failed!"));
    }
}
