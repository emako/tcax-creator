#ifndef ASSEDITOR_H
#define ASSEDITOR_H

#include <QPlainTextEdit>
#include <QObject>

class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;

class LineNumberArea;


class AssEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    AssEditor(QWidget *parent = nullptr);

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();
    QString getCurrentCursorText(void);

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &, int);

private:
    QWidget *lineNumberArea;
};


class LineNumberArea : public QWidget
{
public:
    LineNumberArea(AssEditor *editor) : QWidget(editor) {
        AssEditor = editor;
    }

    QSize sizeHint() const override {
        return QSize(AssEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        AssEditor->lineNumberAreaPaintEvent(event);
    }

private:
    AssEditor *AssEditor;
};

enum class ColorLineNumArea{
    r = 240,
    g = 240,
    b = 240,
};

enum class ColorLineNum{
    r = 140,
    g = 140,
    b = 140,
};

enum class ColorHighlight{
    r = 232,
    g = 232,
    b = 255,
};


#endif // ASSEDITOR_H
