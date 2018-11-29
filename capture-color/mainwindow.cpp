#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setupUi();
}

void MainWindow::setupUi(void)
{
    this->loadFonts();
    this->setFont(DEFAULT_FONT);
    this->setWindowTitle(QString("Capture Color"));
    this->setWindowIcon(QIcon(":/icons/art.ico"));
    ui->previewlabel->setStyleSheet("QLabel{border:1px solid rgb(0, 255, 0);border-top-color: rgb(160, 160, 160);border-left-color: rgb(160, 160, 160);border-right-color: rgb(255, 255, 255);border-bottom-color: rgb(255, 255, 255);}");
    ui->previewlabel->setText("");
    ui->pixlabel->setStyleSheet("QLabel{border:1px solid rgb(0, 255, 0);border-top-color: rgb(160, 160, 160);border-left-color: rgb(160, 160, 160);border-right-color: rgb(255, 255, 255);border-bottom-color: rgb(255, 255, 255);}");
    ui->pixlabel->setText("");
    is_eyedrop_enable = false;
    is_update_preview = false;
    yuv_format = YuvFormat::BT709;
    initReflashChecker();
    mYuvDialog = new YuvDialog();
    mColorWhellDialog = new ColorWhellDialog();
    mColorWhell = mColorWhellDialog->mColorWhell;
    connect(mColorWhell, SIGNAL(colorChange(QColor)), this, SLOT(slotUpdateColor(QColor)));
    updateColor(QColor());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotUpdateColor(QColor color)
{
    updateColor(color);
}

void MainWindow::initReflashChecker(void)
{
    for(int i = 0; i < static_cast<int>(ReflashItem::max); i++)
    {
        is_reflash[i] = true;
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent * event)
{
    if(is_eyedrop_enable)
    {
        unsetCursor();
        ui->eyedropperBt->setStyleSheet("background-image: url(:/buttons/eyedropper_tool_32.png);");
    }
    else
    {
        /*プレビュー画像をクリック*/
        QRect rect(ui->previewlabel->pos()+ui->colorgroupBox->pos()+ui->centralWidget->pos(), ui->previewlabel->size());
        if(rect.contains(event->pos()))
        {
            QScreen *screen = QGuiApplication::primaryScreen();
            pixmap = screen->grabWindow(0, event->globalX(), event->globalY(), 1, 1);
            if (!pixmap.isNull())
            {
                QImage image = pixmap.toImage();
                if (!image.isNull())
                {
                    if (image.valid(0, 0))
                    {
                       QColor color = image.pixel(0, 0);
                       is_update_preview = false;
                       updateColor(color);
                       return;
                    }
                }
            }
        }
        /*YUVグループをクリック*/
        rect = QRect(ui->yuvgroupBox->pos()+ui->centralWidget->pos(), QSize(ui->yuvgroupBox->size().width()/2, ui->yuvgroupBox->size().height()/5));
        if(rect.contains(event->pos()))
        {
            yuv_format = static_cast<YuvFormat>(!(static_cast<bool>(yuv_format)));
            if(static_cast<bool>(yuv_format))
            {
                ui->yuvgroupBox->setTitle("YUV(bt601)");
            }
            else
            {
                ui->yuvgroupBox->setTitle("YUV(bt709)");
            }
        }
    }
    is_eyedrop_enable = false;
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent * event)
{
    if(!is_eyedrop_enable)
    {
        /*YUVラベルをクリック*/
        QRect rect(ui->yuvlabel->pos()+ui->yuvgroupBox->pos()+ui->centralWidget->pos(), ui->yuvlabel->size());
        if(rect.contains(event->pos()))
        {
            if(mYuvDialog->isHidden())
            {
                mYuvDialog->setYuv(ui->y_yuvBox->value(), ui->u_yuvBox->value(), ui->v_yuvBox->value());
                mYuvDialog->setGroupTitle(ui->yuvgroupBox->title());
                if(mYuvDialog->exec() == QDialog::Accepted)
                {
                    mYuvDialog->saveYuv();
                    y = mYuvDialog->y;
                    u = mYuvDialog->u;
                    v = mYuvDialog->v;
                    QColor color = yuvToRgbColor(yuv_format);
                    is_reflash[static_cast<int>(ReflashItem::yuv_yuv)] = false;
                    updateColor(color);
                    is_reflash[static_cast<int>(ReflashItem::yuv_yuv)] = true;
                }
                else
                {
                    /*Do nothing*/;
                }
            }
            else
            {
                mYuvDialog->hide();
            }
        }
    }

}

void MainWindow::mouseMoveEvent(QMouseEvent * event)
{
    if(!is_eyedrop_enable)
    {
        return;
    }
    QScreen *screen = QGuiApplication::primaryScreen();
#ifdef EYEDROPPER_CUR
    QPixmap pixmap = screen->grabWindow(0, x-10, y, 10, 10);
#else
    pixmap = screen->grabWindow(0, event->globalX()-14, event->globalY()+3, 10, 10);
#endif
    if (!pixmap.isNull())
    {
        QImage image = pixmap.toImage();
        pixmap = pixmap.scaled(100, 100, Qt::IgnoreAspectRatio);
        if (!image.isNull())
        {
            if (image.valid(0, 0)) //坐标位置有效
            {
                QColor color = image.pixel(5, 5);
                ui->poslabel->setText(QString("Pos(%1,%2)").arg(event->globalX()).arg(event->globalY()));
                is_update_preview = true;
                updateColor(color);
            }
        }
    }
}

inline QString MainWindow::toAssColor(QColor color)
{
    return QString("&H%1%2%3&").arg(addRei2Two(QString::number(color.blue(), 16))).arg(addRei2Two(QString::number(color.green(), 16))).arg(addRei2Two(QString::number(color.red(), 16))).toUpper();
}

inline QString MainWindow::toHtmlColor(QColor color)
{
    return QString("#%1").arg(toHexColor(color));
}

inline QString MainWindow::toHexColor(QColor color)
{
    return QString("%1%2%3").arg(addRei2Two(QString::number(color.red(), 16))).arg(addRei2Two(QString::number(color.green(), 16))).arg(addRei2Two(QString::number(color.blue(), 16))).toUpper();
}

inline QString MainWindow::addRei2Two(QString text)
{
    if(text.length() == 1)
    {
        text = "0" + text;
    }
    return text;
}

void MainWindow::on_eyedropperBt_pressed()
{
    is_eyedrop_enable = true;
    setCursor(QPixmap(":/buttons/eyedropper_tool_24.png"));
#ifdef EYEDROPPER_CUR
    setCursor(QPixmap(":/curs/eyedropper.cur"));
#else
    ui->eyedropperBt->setStyleSheet("background-image: url(:/buttons/blank_alpha.png);");
#endif
}

void MainWindow::on_eyedropperBt_clicked()
{
    unsetCursor();
    ui->eyedropperBt->setStyleSheet("background-image: url(:/buttons/eyedropper_tool_32.png);");
}

inline QString MainWindow::toYuvColor(QColor color, YuvFormat format)
{
    float r = color.red();
    float g = color.green();
    float b = color.blue();

    switch(format)
    {
    case YuvFormat::BT601:
        /*RGB -> YCbCr (BT.601 SDTV)*/
        y = static_cast<int>( (0.257f * r) + (0.504f * g) + (0.098f * b) + 16 );
        u = static_cast<int>( (-0.148f * r) - (0.291f * g) + (0.439f * b) + 128 );
        v = static_cast<int>( (0.439f * r) - (0.368f * g) - (0.071f * b) + 128 );
        break;
    case YuvFormat::BT709:
        /*RGB -> YPbPr (BT.709 HDTV)*/
        y = static_cast<int>( (0.183f * r) + (0.614f * g) + (0.062f * b) + 16 );
        u = static_cast<int>( (-0.101f * r) - (0.339f * g) + (0.439f * b) + 128 );
        v = static_cast<int>( (0.439f * r) - (0.399f * g) - (0.040f * b) + 128 );
        break;
    }
    y = qBound(0, y, 255);
    u = qBound(0, u, 255);
    v = qBound(0, v, 255);
    return QString("%1 %2 %3").arg(QString::number(y)).arg(QString::number(u)).arg(QString::number(v));
}

inline QColor MainWindow::yuvToRgbColor(YuvFormat format)
{
    QColor color;
    int r;
    int g;
    int b;

    switch(format)
    {
    case YuvFormat::BT601:
        /*YCbCr -> RGB (BT.601 SDTV)*/
        r = static_cast<int>( 1.164 * (y - 16) + 1.596 * (v - 128) );
        g = static_cast<int>( 1.164 * (y - 16) - 0.391 * (u - 128) - 0.813 * (v - 128) );
        b = static_cast<int>( 1.164 * (y - 16) + 2.018 * (u - 128) );
        break;
    case YuvFormat::BT709:
        /*YPbPr -> RGB (BT.709 HDTV)*/
        r = static_cast<int>( 1.164 * (y - 16) + 1.793 * (v - 128) );
        g = static_cast<int>( 1.164 * (y - 16) - 0.213 * (u - 128) - 0.533 * (v - 128) );
        b = static_cast<int>( 1.164 * (y - 16) + 2.112 * (u - 128) );
        break;
    }
    r = qBound(0, r, 255);
    g = qBound(0, g, 255);
    b = qBound(0, b, 255);

    color.setRed(r);
    color.setGreen(g);
    color.setBlue(b);
    return color;
}

inline QString MainWindow::toRgbColor(QColor color)
{
    return QString("%1 %2 %3").arg(QString::number(color.red())).arg(QString::number(color.green())).arg(QString::number(color.blue()));
}

inline QString MainWindow::toHsvColor(QColor color)
{
    return QString("%1 %2 %3").arg(QString::number(color.hsvHue() > 0 ? color.hsvHue() : 0)).arg(QString::number(color.hsvSaturation())).arg(QString::number(color.value()));
}

inline QString MainWindow::toHslColor(QColor color)
{
    return QString("%1 %2 %3").arg(QString::number(color.hslHue() > 0 ? color.hslHue() : 0)).arg(QString::number(color.hslSaturation())).arg(QString::number(color.lightness()));
}

void MainWindow::on_selectBt_clicked()
{
#ifdef ColorDialog
    QColor color = QColorDialog::getColor(Qt::black);

    if (color.isValid())
    {
        updateColor(color);
    }
#endif

    if(mColorWhellDialog->isHidden())
    {
        if(mColorWhellDialog->exec() == QDialog::Accepted)
        {
            updateColor(mColorWhell->color());
        }
    }
    else
    {
        mYuvDialog->hide();
    }

}

void MainWindow::on_aboutBt_clicked()
{
    QMessageBox::about(this, tr("About"), Common::fromResource(":/strings/about").arg(SOFT_VERSION));
}

inline void MainWindow::updateColor(QColor color)
{
    disconnectValueChanged();
    /*RGB値を設置*/
    if(is_reflash[static_cast<int>(ReflashItem::rgb_rgb)])
    {
        ui->rgbEdit->setText(toRgbColor(color));
    }
    if(is_reflash[static_cast<int>(ReflashItem::ass_rgb)])
    {
        ui->assEdit->setText(toAssColor(color));
    }
    if(is_reflash[static_cast<int>(ReflashItem::html_rgb)])
    {
        ui->htmlEdit->setText(toHtmlColor(color));
    }

    if(is_reflash[static_cast<int>(ReflashItem::r_rgb)])
    {
        ui->redBox->setValue(color.red());
    }
    if(is_reflash[static_cast<int>(ReflashItem::g_rgb)])
    {
        ui->greenBox->setValue(color.green());
    }
    if(is_reflash[static_cast<int>(ReflashItem::b_rgb)])
    {
        ui->blueBox->setValue(color.blue());
    }

    /*YUV値を設置*/
    if(is_reflash[static_cast<int>(ReflashItem::yuv_yuv)])
    {
        ui->yuvEdit->setText(toYuvColor(color));
    }
    if(is_reflash[static_cast<int>(ReflashItem::y_yuv)])
    {
        ui->y_yuvBox->setValue(y);
    }
    if(is_reflash[static_cast<int>(ReflashItem::u_yuv)])
    {
        ui->u_yuvBox->setValue(u);
    }
    if(is_reflash[static_cast<int>(ReflashItem::v_yuv)])
    {
        ui->v_yuvBox->setValue(v);
    }

    /*HSV/HSB値を設置*/
    if(is_reflash[static_cast<int>(ReflashItem::hsv_hsv)])
    {
        ui->hsvEdit->setText(toHsvColor(color));
    }
    if(is_reflash[static_cast<int>(ReflashItem::h_hsv)])
    {
        ui->h_hsvBox->setValue(color.hsvHue());
    }
    if(is_reflash[static_cast<int>(ReflashItem::s_hsv)])
    {
        ui->s_hsvBox->setValue(color.hsvSaturation());
    }
    if(is_reflash[static_cast<int>(ReflashItem::v_hsv)])
    {
        ui->v_hsvBox->setValue(color.value());
    }

    /*HSL値を設置*/
    if(is_reflash[static_cast<int>(ReflashItem::hsl_hsl)])
    {
        ui->hslEdit->setText(toHslColor(color));
    }
    if(is_reflash[static_cast<int>(ReflashItem::h_hsl)])
    {
        ui->h_hslBox->setValue(color.hslHue());
    }
    if(is_reflash[static_cast<int>(ReflashItem::s_hsl)])
    {
        ui->s_hslBox->setValue(color.hslSaturation());
    }
    if(is_reflash[static_cast<int>(ReflashItem::l_hsl)])
    {
        ui->l_hslBox->setValue(color.lightness());
    }

    /*ピクセル画像を設置*/
    QPalette palette = ui->pixlabel->palette();

    palette.setColor(QPalette::Window, color);
    ui->pixlabel->setPalette(palette);
    ui->pixlabel->setAutoFillBackground(true);

    /*プレビュー画像を設置*/
    if(is_update_preview)
    {
        ui->previewlabel->setPixmap(pixmap);
        is_update_preview = false;
    }
    connectValueChanged();
}

/*UI Color Changed*/
void MainWindow::on_redBox_valueChanged(int arg1)
{
    QColor color;

    color.setRed(arg1);
    color.setGreen(ui->greenBox->value());
    color.setBlue(ui->blueBox->value());
    is_reflash[static_cast<int>(ReflashItem::r_rgb)] = false;
    updateColor(color);
    is_reflash[static_cast<int>(ReflashItem::r_rgb)] = true;
}

void MainWindow::on_greenBox_valueChanged(int arg1)
{
    QColor color;

    color.setRed(ui->redBox->value());
    color.setGreen(arg1);
    color.setBlue(ui->blueBox->value());
    is_reflash[static_cast<int>(ReflashItem::g_rgb)] = false;
    updateColor(color);
    is_reflash[static_cast<int>(ReflashItem::g_rgb)] = true;
}

void MainWindow::on_blueBox_valueChanged(int arg1)
{
    QColor color;
    color.setRed(ui->redBox->value());
    color.setGreen(ui->greenBox->value());
    color.setBlue(arg1);
    is_reflash[static_cast<int>(ReflashItem::b_rgb)] = false;
    updateColor(color);
    is_reflash[static_cast<int>(ReflashItem::b_rgb)] = true;
}

void MainWindow::on_assEdit_textEdited(const QString &arg1)
{
    QColor color;
    QString text = arg1.toUpper();

    text.remove("H");
    text.remove("&");
    if(text.length() < 6)
    {
        return;
    }
    for(int i = 3; i > 0; i--)
    {
        QString gbr;
        int ass_color;
        bool ok = false;

        switch(static_cast<AssColor>(i))
        {
        case AssColor::Red:
            gbr = QString(text.mid(text.length()-2, 2));
            ass_color = gbr.toInt(&ok, 16);
            color.setRed(ass_color);
            break;
        case AssColor::Green:
            gbr = QString(text.mid(text.length()-4, 2));
            ass_color = gbr.toInt(&ok, 16);
            color.setGreen(ass_color);
            break;
        case AssColor::Blue:
            gbr = QString(text.mid(text.length()-6, 2));
            ass_color = gbr.toInt(&ok, 16);
            color.setBlue(ass_color);
            break;
        }
    }
    is_reflash[static_cast<int>(ReflashItem::ass_rgb)] = false;
    updateColor(color);
    is_reflash[static_cast<int>(ReflashItem::ass_rgb)] = true;
}

void MainWindow::on_htmlEdit_textEdited(const QString &arg1)
{
    QColor color;
    QString text = arg1.toUpper();

    text.remove("#");
    if(text.length() < 6)
    {
        return;
    }


    for(int i = 1; i <= 3; i++)
    {
        QString gbr;
        int ass_color;
        bool ok = false;

        switch(static_cast<AssColor>(i))
        {
        case AssColor::Red:
            gbr = text.mid(0, 2);
            ass_color = gbr.toInt(&ok, 16);
            color.setRed(ass_color);
            break;
        case AssColor::Green:
            gbr = text.mid(2, 2);
            ass_color = gbr.toInt(&ok, 16);
            color.setGreen(ass_color);
            break;
        case AssColor::Blue:
            gbr = text.mid(4, 2);
            ass_color = gbr.toInt(&ok, 16);
            color.setBlue(ass_color);
            break;
        }
    }
    is_reflash[static_cast<int>(ReflashItem::html_rgb)] = false;
    updateColor(color);
    is_reflash[static_cast<int>(ReflashItem::html_rgb)] = true;
}

void MainWindow::on_rgbEdit_textEdited(const QString &arg1)
{
    QColor color;
    QStringList color_list = arg1.split(' ');
    int value;

    if(color_list.length() < 3)
    {
        return;
    }
    value = color_list.at(0).toInt();
    color.setRed(qBound(0, value, 255));
    value = color_list.at(1).toInt();
    color.setGreen(qBound(0, value, 255));
    value = color_list.at(2).toInt();
    color.setBlue(qBound(0, value, 255));
    is_reflash[static_cast<int>(ReflashItem::rgb_rgb)] = false;
    updateColor(color);
    is_reflash[static_cast<int>(ReflashItem::rgb_rgb)] = true;
}

void MainWindow::on_h_hsvBox_valueChanged(int arg1)
{
    QColor color;

    color = QColor::fromHsv(arg1, ui->s_hsvBox->value(), ui->v_hsvBox->value());
    is_reflash[static_cast<int>(ReflashItem::h_hsv)] = false;
    updateColor(color);
    is_reflash[static_cast<int>(ReflashItem::h_hsv)] = true;
}

void MainWindow::on_s_hsvBox_valueChanged(int arg1)
{
    QColor color;

    color = QColor::fromHsv(ui->h_hsvBox->value(), arg1, ui->v_hsvBox->value());
    is_reflash[static_cast<int>(ReflashItem::s_hsv)] = false;
    updateColor(color);
    is_reflash[static_cast<int>(ReflashItem::s_hsv)] = true;
}

void MainWindow::on_v_hsvBox_valueChanged(int arg1)
{
    QColor color;

    color = QColor::fromHsv(ui->h_hsvBox->value(), ui->s_hsvBox->value(), arg1);
    is_reflash[static_cast<int>(ReflashItem::v_hsv)] = false;
    updateColor(color);
    is_reflash[static_cast<int>(ReflashItem::v_hsv)] = true;
}

void MainWindow::on_hsvEdit_textEdited(const QString &arg1)
{
    QColor color;
    QStringList color_list = arg1.split(' ');

    if(color_list.length() < 3)
    {
        return;
    }
    int h_value = color_list.at(0).toInt();
    int s_value = color_list.at(1).toInt();
    int v_value = color_list.at(2).toInt();
    qBound(0, h_value, 255);
    qBound(0, s_value, 255);
    qBound(0, v_value, 255);
    color = QColor::fromHsv(h_value, s_value, v_value);
    is_reflash[static_cast<int>(ReflashItem::hsv_hsv)] = false;
    updateColor(color);
    is_reflash[static_cast<int>(ReflashItem::hsv_hsv)] = true;
}

void MainWindow::on_h_hslBox_valueChanged(int arg1)
{
    QColor color;
    color = QColor::fromHsl(arg1, ui->s_hslBox->value(), ui->l_hslBox->value());
    is_reflash[static_cast<int>(ReflashItem::h_hsl)] = false;
    updateColor(color);
    is_reflash[static_cast<int>(ReflashItem::h_hsl)] = true;
}

void MainWindow::on_s_hslBox_valueChanged(int arg1)
{
    QColor color;
    color = QColor::fromHsl(ui->h_hslBox->value(), arg1, ui->l_hslBox->value());
    is_reflash[static_cast<int>(ReflashItem::s_hsl)] = false;
    updateColor(color);
    is_reflash[static_cast<int>(ReflashItem::s_hsl)] = true;
}

void MainWindow::on_l_hslBox_valueChanged(int arg1)
{
    QColor color;
    color = QColor::fromHsl(ui->h_hslBox->value(), ui->s_hslBox->value(), arg1);
    is_reflash[static_cast<int>(ReflashItem::l_hsl)] = false;
    updateColor(color);
    is_reflash[static_cast<int>(ReflashItem::l_hsl)] = true;
}

void MainWindow::on_hslEdit_textEdited(const QString &arg1)
{
    QColor color;
    QStringList color_list = arg1.split(' ');
    if(color_list.length() < 3)
    {
        return;
    }
    int h_value = color_list.at(0).toInt();
    int s_value = color_list.at(1).toInt();
    int l_value = color_list.at(2).toInt();
    qBound(0, h_value, 255);
    qBound(0, s_value, 255);
    qBound(0, l_value, 255);
    color = QColor::fromHsl(h_value, s_value, l_value);
    is_reflash[static_cast<int>(ReflashItem::hsl_hsl)] = false;
    updateColor(color);
    is_reflash[static_cast<int>(ReflashItem::hsl_hsl)] = true;
}

void MainWindow::on_y_yuvBox_valueChanged(int arg1)
{
    y = arg1;
    u = ui->u_yuvBox->value();
    v = ui->v_yuvBox->value();
    QColor color = yuvToRgbColor(yuv_format);
    is_reflash[static_cast<int>(ReflashItem::y_yuv)] = false;
    updateColor(color);
    is_reflash[static_cast<int>(ReflashItem::y_yuv)] = true;
}

void MainWindow::on_u_yuvBox_valueChanged(int arg1)
{
    y = ui->y_yuvBox->value();
    u = arg1;
    v = ui->v_yuvBox->value();
    QColor color = yuvToRgbColor(yuv_format);
    is_reflash[static_cast<int>(ReflashItem::u_yuv)] = false;
    updateColor(color);
    is_reflash[static_cast<int>(ReflashItem::u_yuv)] = true;
}

void MainWindow::on_v_yuvBox_valueChanged(int arg1)
{
    y = ui->y_yuvBox->value();
    u = ui->u_yuvBox->value();
    v = arg1;
    QColor color = yuvToRgbColor(yuv_format);
    is_reflash[static_cast<int>(ReflashItem::v_yuv)] = false;
    updateColor(color);
    is_reflash[static_cast<int>(ReflashItem::v_yuv)] = true;
}

void MainWindow::on_yuvEdit_textEdited(const QString &arg1)
{
    QStringList color_list = arg1.split(' ');
    if(color_list.length() < 3)
    {
        return;
    }
    y = color_list.at(0).toInt();
    u = color_list.at(1).toInt();
    v = color_list.at(2).toInt();
    QColor color = yuvToRgbColor(yuv_format);
    is_reflash[static_cast<int>(ReflashItem::yuv_yuv)] = false;
    updateColor(color);
    is_reflash[static_cast<int>(ReflashItem::yuv_yuv)] = true;
}

void MainWindow::disconnectValueChanged(void)
{
    ui->redBox->disconnect();
    ui->greenBox->disconnect();
    ui->blueBox->disconnect();
    ui->h_hsvBox->disconnect();
    ui->s_hsvBox->disconnect();
    ui->v_hsvBox->disconnect();
    ui->h_hslBox->disconnect();
    ui->s_hslBox->disconnect();
    ui->l_hslBox->disconnect();
    ui->y_yuvBox->disconnect();
    ui->u_yuvBox->disconnect();
    ui->v_yuvBox->disconnect();
}

void MainWindow::connectValueChanged(void)
{
    connect(ui->redBox, SIGNAL(valueChanged(int)), this, SLOT(on_redBox_valueChanged(int)));
    connect(ui->greenBox, SIGNAL(valueChanged(int)), this, SLOT(on_greenBox_valueChanged(int)));
    connect(ui->blueBox, SIGNAL(valueChanged(int)), this, SLOT(on_blueBox_valueChanged(int)));
    connect(ui->h_hsvBox, SIGNAL(valueChanged(int)), this, SLOT(on_h_hsvBox_valueChanged(int)));
    connect(ui->s_hsvBox, SIGNAL(valueChanged(int)), this, SLOT(on_s_hsvBox_valueChanged(int)));
    connect(ui->v_hsvBox, SIGNAL(valueChanged(int)), this, SLOT(on_v_hsvBox_valueChanged(int)));
    connect(ui->h_hslBox, SIGNAL(valueChanged(int)), this, SLOT(on_h_hslBox_valueChanged(int)));
    connect(ui->s_hslBox, SIGNAL(valueChanged(int)), this, SLOT(on_s_hslBox_valueChanged(int)));
    connect(ui->l_hslBox, SIGNAL(valueChanged(int)), this, SLOT(on_l_hslBox_valueChanged(int)));
    connect(ui->y_yuvBox, SIGNAL(valueChanged(int)), this, SLOT(on_y_yuvBox_valueChanged(int)));
    connect(ui->u_yuvBox, SIGNAL(valueChanged(int)), this, SLOT(on_u_yuvBox_valueChanged(int)));
    connect(ui->v_yuvBox, SIGNAL(valueChanged(int)), this, SLOT(on_v_yuvBox_valueChanged(int)));
}

void MainWindow::loadFonts(void)
{
    QFontDatabase::addApplicationFontFromData(Common::getResource(":/fonts/consola.ttf"));
}
