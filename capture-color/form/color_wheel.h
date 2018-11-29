#ifndef COLORWHEEL_H
#define COLORWHEEL_H

#include <QWidget>
#include <QtCore/qmath.h>
#include <QPainter>
#include <QResizeEvent>
#include <QStyleOption>

#ifdef DEBUG_MODE
#include <QDebug>
#endif

class ColorWheel : public QWidget
{
    Q_OBJECT
public:
    explicit ColorWheel(QWidget *parent = nullptr);
    ~ColorWheel();

    virtual QSize sizeHint () const;
    virtual QSize minimumSizeHint () const;
    QColor color();

signals:
    void colorChange(const QColor &color);

public slots:
    void setColor(const QColor &color);

protected:
    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *);

private:
    void composeWheel();
    QColor posColor(const QPoint &point);
    void drawWheelImage(const QSize &newSize);
    void drawSquareImage(const int &hue);
    void drawIndicator(const int &hue);
    void drawPicker(const QColor &color);

    QSize   initSize;
    QImage  wheelImage;
    QImage  squareImage;
    QPixmap wheel;
    bool    mouseDown;
    QPoint  lastPos;
    int     margin;
    int     wheelWidth;
    QRegion wheelRegion;
    QRegion squareRegion;
    QColor  current;
    bool    isInWheel;
    bool    isInSquare;

private slots:
    void hueChanged(const int &hue);
    void svChanged(const QColor &newcolor);
};

#endif // COLORWHEEL_H
