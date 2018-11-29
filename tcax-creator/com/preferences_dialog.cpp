#include "preferences_dialog.h"
#include "ui_preferences_dialog.h"

Preferences::Preferences(QDialog *parent) :
    QDialog(parent),
    ui(new Ui::Preferences)
{
    ui->setupUi(this);
    this->setup();
}

Preferences::~Preferences()
{
    delete ui;
}

void Preferences::setup(void)
{
    ui->comboBoxStyleFactory->addItems(QStyleFactory::keys());
    this->setWindowTitle(tr("Preferences"));
    this->setFont(DEFAULT_FONT);
    this->loadCommonConfig();

    connect(ui->checkBoxEditorSizeLimit, SIGNAL(stateChanged(int)), this, SLOT(slotConfigChanged(int)));
    connect(ui->checkBoxSetStyleFactory, SIGNAL(stateChanged(int)), this, SLOT(slotConfigChanged(int)));
    connect(ui->comboBoxStyleFactory, SIGNAL(currentIndexChanged(int)), this, SLOT(slotConfigChanged(int)));
}

void Preferences::loadCommonConfig(void)
{
    ui->checkBoxShowSplashScreen->setChecked(g_pConfig->getConfig(Config::eCONFIG_COMMON_SHOW_SPLASH_SCREEN).toBool());
    ui->checkBoxEditorSizeLimit->setChecked(g_pConfig->getConfig(Config::eCONFIG_COMMON_EDITOR_FILE_LIMIT_ENABLE).toBool());
    ui->spinBoxFileSizeLimit->setValue(g_pConfig->getConfig(Config::eCONFIG_COMMON_EDITOR_FILE_LIMIT_SIZE).toInt());
    ui->spinBoxFileSizeLimit->setEnabled(ui->checkBoxEditorSizeLimit->isChecked());
    ui->checkBoxSetStyleFactory->setChecked(g_pConfig->getConfig(Config::eCONFIG_COMMON_STYLE_FACTORY_ENABLE).toBool());
    ui->comboBoxStyleFactory->setCurrentText(g_pConfig->getConfig(Config::eCONFIG_COMMON_STYLE_FACTORY).toString());
    ui->comboBoxStyleFactory->setEnabled(ui->checkBoxSetStyleFactory->isChecked());
}

void Preferences::on_buttonReset_clicked()
{
    g_pConfig->reset();
    loadCommonConfig();
}

void Preferences::on_buttonClose_clicked()
{
    g_pConfig->setConfig(Config::eCONFIG_COMMON_SHOW_SPLASH_SCREEN, ui->checkBoxShowSplashScreen->isChecked());
    g_pConfig->setConfig(Config::eCONFIG_COMMON_EDITOR_FILE_LIMIT_ENABLE, ui->checkBoxEditorSizeLimit->isChecked());
    g_pConfig->setConfig(Config::eCONFIG_COMMON_EDITOR_FILE_LIMIT_SIZE, ui->spinBoxFileSizeLimit->value());
    g_pConfig->setConfig(Config::eCONFIG_COMMON_STYLE_FACTORY_ENABLE, ui->checkBoxSetStyleFactory->isChecked());
    g_pConfig->setConfig(Config::eCONFIG_COMMON_STYLE_FACTORY, ui->comboBoxStyleFactory->currentText());
    this->accept();
}

void Preferences::slotConfigChanged(int a_arg1)
{
    if(sender() == ui->checkBoxEditorSizeLimit)
    {
        ui->spinBoxFileSizeLimit->setEnabled( (a_arg1 == Qt::Unchecked) ? false : true );
    }
    else if(sender() == ui->checkBoxSetStyleFactory)
    {
        ui->comboBoxStyleFactory->setEnabled( (a_arg1 == Qt::Unchecked) ? false : true );
        ui->comboBoxStyleFactory->setCurrentText(g_pConfig->getConfigDefault(Config::eCONFIG_COMMON_STYLE_FACTORY).toString());
        QApplication::setStyle(g_pConfig->getConfigDefault(Config::eCONFIG_COMMON_STYLE_FACTORY).toString());

    }
    else if(sender() == ui->comboBoxStyleFactory)
    {
        QApplication::setStyle(ui->comboBoxStyleFactory->currentText());
    }
}

