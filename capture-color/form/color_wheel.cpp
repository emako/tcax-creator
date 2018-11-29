#include "color_wheel.h"

ColorWheel::ColorWheel(QWidget *parent) :
    QWidget(parent),
    initSize(200,200),
    mouseDown(false),
    margin(0),
    wheelWidth(30),
    current(Qt::red),
    isInWheel(false),
    isInSquare(false)
{
    resize(initSize);
    current = current.toHsv();
    setCursor(Qt::CrossCursor);
}

ColorWheel::~ColorWheel()
{
}

QSize ColorWheel::sizeHint () const
{
    return QSize(height(),height());
}

QSize ColorWheel::minimumSizeHint () const
{
    return initSize;
}

QColor ColorWheel::color()
{
    return current;
}

void ColorWheel::setColor(const QColor &color)
{
    if(color == current) return;
    if(color.hue() != current.hue())
        hueChanged(color.hue());
    if(color.saturation() != current.saturation()
            || color.value() != current.value() )
        svChanged(color);

    update();
}

QColor ColorWheel::posColor(const QPoint &point)
{
    if(!wheel.rect().contains(point))
        return QColor();
    if(isInWheel)
    {
        qreal hue = 0;
        int r = qMin(width(), height()) / 2;
        if(point.x() > r )
        {
            if(point.y() < r )      //1
                hue = 90 - (qAtan2( (point.x() - r) , (r - point.y()) )  / 3.14 / 2 * 360);
            else                    //4
                hue = 270 + (qAtan2( (point.x() - r) , (point.y() - r ) )  / 3.14 / 2 * 360);
        }
        else
        {
            if(point.y() < r )      //2
                hue =  90 + (qAtan2( (r - point.x()) , (r - point.y()) )  / 3.14 / 2 * 360);
            else                    //3
                hue =  270 - (qAtan2( (r - point.x()) , (point.y() - r ))  / 3.14 / 2 * 360);
        }
        hue = hue>359 ? 359 : hue;
        hue = hue<0   ? 0   : hue;
        return QColor::fromHsv(hue,
                               current.saturation(),
                               current.value());
    }
    if(isInSquare)
    {
        int w = qMin(width(), height());
        qreal r = w/2 - margin;
        qreal ir = r-wheelWidth;
        qreal m = w/2.0-ir/qSqrt(2);
        QPoint p = point - QPoint(m, m);
        qreal SquareWidth = 2*ir/qSqrt(2);
        return QColor::fromHsvF( current.hueF(),
                                 p.x()/SquareWidth,
                                 p.y()/SquareWidth);
    }
    return QColor();
}

void ColorWheel::resizeEvent(QResizeEvent *event)
{
    wheel = QPixmap(event->size());
    wheel.fill(palette().background().color());
    drawWheelImage(event->size());
    drawSquareImage(current.hue());
    update();
}

void ColorWheel::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QStyleOption opt;
    opt.initFrom(this);
    composeWheel();
    painter.drawPixmap(0,0,wheel);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
}

void ColorWheel::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
    if(wheelRegion.contains(lastPos))
    {
        isInWheel = true;
        isInSquare = false;
        QColor color = posColor(lastPos);
        hueChanged(color.hue());
    }
    else if(squareRegion.contains(lastPos))
    {
        isInWheel = false;
        isInSquare = true;
        QColor color = posColor(lastPos);
        svChanged(color);
    }
    mouseDown = true;
}

void ColorWheel::mouseMoveEvent(QMouseEvent *event)
{
    lastPos = event->pos();
    if(!mouseDown)
        return;
    if(wheelRegion.contains(lastPos) && isInWheel)
    {
        QColor color = posColor(lastPos);
        hueChanged(color.hue());
    }
    else if(squareRegion.contains(lastPos) && isInSquare)
    {
        QColor color = posColor(lastPos);
        svChanged(color);
    }
    else
    {
        ;
    }
}

void ColorWheel::mouseReleaseEvent(QMouseEvent *)
{
    mouseDown = false;
    isInWheel = false;
    isInSquare = false;
}

void ColorWheel::composeWheel()
{
    QPainter composePainter(&wheel);
    composePainter.drawImage(0, 0, wheelImage);
    composePainter.drawImage(squareRegion.boundingRect().topLeft(), squareImage);
    composePainter.end();
    drawIndicator(current.hue());
    drawPicker(current);
}

void ColorWheel::drawWheelImage(const QSize &newSize)
{
    int r = qMin(newSize.width(), newSize.height());

    QStyleOption option;
    option.initFrom(this);
    QBrush background = option.palette.window();

    wheelImage = QImage(newSize, QImage::Format_ARGB32_Premultiplied);
#if QT_VERSION > QT_VERSION_CHECK(4,8,0)
    wheelImage.fill(background.color());
#endif
    QPainter painter(&wheelImage);
    painter.setRenderHint(QPainter::Antialiasing);


    QConicalGradient conicalGradient(0, 0, 0);
    conicalGradient.setColorAt(0.0, Qt::red);
    conicalGradient.setColorAt(60.0/360.0, Qt::yellow);
    conicalGradient.setColorAt(120.0/360.0, Qt::green);
    conicalGradient.setColorAt(180.0/360.0, Qt::cyan);
    conicalGradient.setColorAt(240.0/360.0, Qt::blue);
    conicalGradient.setColorAt(300.0/360.0, Qt::magenta);
    conicalGradient.setColorAt(1.0, Qt::red);

    painter.translate(r/2, r/2);

    QBrush brush(conicalGradient);
    painter.setPen(Qt::NoPen);
    painter.setBrush(brush);
    painter.drawEllipse(QPoint(0,0),r/2-margin,r/2-margin);

    painter.setBrush(background);
    painter.drawEllipse(QPoint(0,0),r/2-margin-wheelWidth,r/2-margin-wheelWidth);


    wheelRegion = QRegion(r/2, r/2, r-2*margin, r-2*margin, QRegion::Ellipse);
    wheelRegion.translate(-(r-2*margin)/2, -(r-2*margin)/2);

    int tmp = 2*(margin+wheelWidth);
    QRegion subRe( r/2, r/2, r-tmp, r-tmp, QRegion::Ellipse );
    subRe.translate(-(r-tmp)/2, -(r-tmp)/2);
    wheelRegion -= subRe;
}

void ColorWheel::drawSquareImage(const int &hue)
{
    int w = qMin(width(), height());

    qreal r = w/2-margin;
    qreal ir = r-wheelWidth;
    qreal m = w/2.0-ir/qSqrt(2);


    QImage square(255,255,QImage::Format_ARGB32_Premultiplied);
    QColor color;
    QRgb vv;
    for(int i=0; i<255; ++i)
    {
        for(int j=0; j<255; ++j)
        {
            color = QColor::fromHsv(hue,i,j);
            vv = qRgb(color.red(),color.green(),color.blue());
            square.setPixel(i,j,vv);
        }
    }

    qreal SquareWidth = 2*ir/qSqrt(2);
    squareImage = square.scaled(SquareWidth,SquareWidth);
    squareRegion = QRegion(m, m, SquareWidth, SquareWidth);
}

void ColorWheel::drawIndicator(const int &hue)
{
    QPainter painter(&wheel);
    painter.setRenderHint(QPainter::Antialiasing);

    if(hue > 20 && hue < 200)
        painter.setPen(Qt::black);
    else
        painter.setPen(Qt::white);
    painter.setBrush(Qt::NoBrush);

    QPen pen = painter.pen();
    pen.setWidth(2);
    painter.setPen(pen);
    qreal r = qMin(height(), width()) / 2.0;
    painter.translate(r, r);
    painter.rotate(-hue);
    r = qMin(height(), width()) / 2.0 - margin - wheelWidth/2;
    painter.drawEllipse(QPointF(r,0.0), 5, 5);
}

void ColorWheel::drawPicker(const QColor &color)
{
    QPainter painter(&wheel);
    painter.setRenderHint(QPainter::Antialiasing);
    QPen pen;

    int w = qMin(width(), height());
    qreal r = w/2-margin;
    qreal ir = r-wheelWidth;
    qreal m = w/2.0-ir/qSqrt(2);
    painter.translate(m-5, m-5);
    qreal SquareWidth = 2*ir/qSqrt(2);
    qreal S = color.saturationF()*SquareWidth;
    qreal V = color.valueF()*SquareWidth;

    if(color.saturation() > 30 ||color.value() < 50)
        pen.setColor(Qt::white);
    pen.setWidth(2);
    painter.setPen(pen);
    painter.drawEllipse(S,V,10,10);
}

void ColorWheel::hueChanged(const int &hue)
{
    if(hue<0 || hue>359)    return;
    int s = current.saturation();
    int v = current.value();
    current.setHsv(hue, s, v);
    if(!isVisible())        return;
    drawSquareImage(hue);
    repaint();
    emit colorChange(current);
}

void ColorWheel::svChanged(const QColor &newcolor)
{
    int hue = current.hue();
    current.setHsv(hue, newcolor.saturation(),
                   newcolor.value());
    if(!isVisible()) return;
    repaint();
    emit colorChange(current);
}
