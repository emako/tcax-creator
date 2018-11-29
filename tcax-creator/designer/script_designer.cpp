#include "script_designer.h"
#include "ui_script_designer.h"
#include "ui_tcas_designer.h"
#include "ui_tcc_designer.h"

ScriptDesigner::ScriptDesigner(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ScriptDesigner)
{
    ui->setupUi(this);
    ui->tcasDesigner->hide();
    ui->tccDesigner->hide();
}

ScriptDesigner::~ScriptDesigner()
{
    delete ui;
}

void ScriptDesigner::setMainUi(QWidget *a_pWidget)
{
    this->mainUi = a_pWidget;
    ui->tcasDesigner->mainUi = mainUi;
    ui->tccDesigner->mainUi = mainUi;
}

void ScriptDesigner::clear(void)
{
    ui->tcasDesigner->hide();
    ui->tccDesigner->hide();
}

void ScriptDesigner::reload(DesignScriptType a_type, const QString &a_filename)
{
    switch(a_type)
    {
    case TcaxDesign:
        reloadTcas(a_filename);
        break;
    case TccDesign:
        reloadTcc(a_filename);
        break;
    default:
        break;
    }
}

void ScriptDesigner::reloadTcas(const QString &a_filename)
{
    ui->tccDesigner->hide();
    ui->tcasDesigner->show();
    ui->tcasDesigner->reload(a_filename);
}

void ScriptDesigner::reloadTcc(const QString &a_filename)
{
    ui->tccDesigner->show();
    ui->tcasDesigner->hide();
    ui->tccDesigner->reload(a_filename);
}
