#include "color_whell_dialog.h"
#include "ui_color_whell_dialog.h"

ColorWhellDialog::ColorWhellDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ColorWhellDialog)
{
    ui->setupUi(this);
    mColorWhell = ui->colorwhell;
    this->setWindowTitle("Color Whell");
    this->setWindowIcon(QIcon(":/icons/art.ico"));
    this->setFont(DEFAULT_FONT);
}

ColorWhellDialog::~ColorWhellDialog()
{
    delete ui;
}
