#ifndef ASS_SYNTAX_HIGHLIGHTER_H
#define ASS_SYNTAX_HIGHLIGHTER_H

#include "ass_tag.h"

#include <QSyntaxHighlighter>
#include <QTextCharFormat>

#define COLOR_KEYWORD QColor(255, 0, 161)
#define COLOR_SECTION_LINE QColor(5, 40, 229)
#define COLOR_COMMENT_LINE QColor(127, 127, 127)
#define COLOR_FORMAT_LINE QColor(173, 86, 248)

QT_BEGIN_NAMESPACE
class QTextDocument;
QT_END_NAMESPACE

class AssSyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    AssSyntaxHighlighter(QTextDocument *parent = nullptr);
    void initSyntax(void);
    bool isHasRules(void);
    void completeSyntax(void);
    void clearSyntax(void);

    QTextCharFormat text_format;
    QTextCharFormat keyword_format;
    QTextCharFormat comment_line_format;
    QTextCharFormat section_line_format;
    QTextCharFormat format_line_format;
    QTextCharFormat style_line_format;

public slots:
    void addSyntaxKeywords(const QStringList& keywords);
    void addSyntaxKeywords(const QStringList& keywords, const QTextCharFormat& format);
    void addSyntax(const QString& pattern, const QTextCharFormat& format);

protected:
    void highlightBlock(const QString &text);

private:
    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;
    bool isEmpty;
};

#endif // ASS_SYNTAX_HIGHLIGHTER_H
