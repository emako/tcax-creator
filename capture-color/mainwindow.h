#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "com/version.h"
#include "com/common.h"
#include "dialog/yuv_dialog.h"
#include "dialog/color_whell_dialog.h"

#include <QMainWindow>
#include <QApplication>
#include <QCursor>
#include <QPixmap>
#include <QScreen>
#include <QColor>
#include <QDebug>
#include <QMouseEvent>
#include <QColorDialog>
#include <QPropertyAnimation>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QFontDatabase>

enum class YuvFormat{
    BT709,
    BT601,
};

enum class AssColor{
    Blue = 1,
    Green,
    Red,
};

enum class ReflashItem{
    r_rgb,          /*index:1*/
    g_rgb,          /*index:2*/
    b_rgb,          /*index:3*/
    ass_rgb,        /*index:4*/
    html_rgb,       /*index:5*/
    rgb_rgb,        /*index:6*/
    h_hsv,          /*index:7*/
    s_hsv,          /*index:8*/
    v_hsv,          /*index:9*/
    hsv_hsv,        /*index:10*/
    h_hsl,          /*index:11*/
    s_hsl,          /*index:12*/
    l_hsl,          /*index:13*/
    hsl_hsl,        /*index:14*/
    y_yuv,          /*index:15*/
    u_yuv,          /*index:16*/
    v_yuv,          /*index:17*/
    yuv_yuv,        /*index:18*/
    max,            /*index:19*/
};

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
    void setupUi(void);
    void loadFonts(void);
    inline void updateColor(QColor color);
    inline QString toRgbColor(QColor color);
    inline QString toAssColor(QColor color);
    inline QString toHtmlColor(QColor color);
    inline QString toYuvColor(QColor color, YuvFormat format = YuvFormat::BT709);
    inline QColor yuvToRgbColor(YuvFormat format = YuvFormat::BT709);
    inline QString toHsvColor(QColor color);
    inline QString toHslColor(QColor color);
    inline QString toHexColor(QColor color);
    inline QString addRei2Two(QString text);
    void mouseMoveEvent(QMouseEvent * event) override;
    void mouseReleaseEvent(QMouseEvent * event) override;
    void mouseDoubleClickEvent(QMouseEvent * event) override;

    void disconnectValueChanged(void);
    void connectValueChanged(void);
    void initReflashChecker(void);

private slots:
    void slotUpdateColor(QColor color);
    void on_eyedropperBt_pressed();
    void on_eyedropperBt_clicked();
    void on_selectBt_clicked();
    void on_aboutBt_clicked();

    void on_redBox_valueChanged(int arg1);
    void on_greenBox_valueChanged(int arg1);
    void on_blueBox_valueChanged(int arg1);
    void on_assEdit_textEdited(const QString &arg1);
    void on_htmlEdit_textEdited(const QString &arg1);
    void on_rgbEdit_textEdited(const QString &arg1);
    void on_h_hsvBox_valueChanged(int arg1);
    void on_s_hsvBox_valueChanged(int arg1);
    void on_v_hsvBox_valueChanged(int arg1);
    void on_hsvEdit_textEdited(const QString &arg1);
    void on_h_hslBox_valueChanged(int arg1);
    void on_s_hslBox_valueChanged(int arg1);
    void on_l_hslBox_valueChanged(int arg1);
    void on_hslEdit_textEdited(const QString &arg1);
    void on_y_yuvBox_valueChanged(int arg1);
    void on_u_yuvBox_valueChanged(int arg1);
    void on_v_yuvBox_valueChanged(int arg1);
    void on_yuvEdit_textEdited(const QString &arg1);

private:
    Ui::MainWindow *ui;
    YuvDialog *mYuvDialog;
    ColorWhellDialog *mColorWhellDialog;
    ColorWheel *mColorWhell;
    bool is_eyedrop_enable;
    bool is_update_preview;
    YuvFormat yuv_format;
    QPixmap pixmap;
    int y;
    int u;
    int v;
    bool is_reflash[static_cast<int>(ReflashItem::max)];
};

#endif // MAINWINDOW_H
