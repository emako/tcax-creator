#include <QtGui>
#include "ass_syntax_highlighter.h"

AssSyntaxHighlighter::AssSyntaxHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    this->initSyntax();
}

void AssSyntaxHighlighter::initSyntax(void)
{
    isEmpty = true;
    keyword_format.setForeground(COLOR_KEYWORD);
    section_line_format.setForeground(COLOR_SECTION_LINE);
    section_line_format.setFontWeight(QFont::Bold);
    comment_line_format.setForeground(COLOR_COMMENT_LINE);
    format_line_format.setForeground(COLOR_FORMAT_LINE);
    format_line_format.setFontWeight(QFont::Bold);
}

void AssSyntaxHighlighter::highlightBlock(const QString &text)
{
    for(const HighlightingRule &rule : highlightingRules)
    {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while(index >= 0)
        {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }
    setCurrentBlockState(0);
}

void AssSyntaxHighlighter::addSyntaxKeywords(const QStringList& keywords)
{
    HighlightingRule rule;
    for(QString keyword : keywords)
    {
        QString pattern = QString(ASS_TAG_KEYWORD_QSTR).arg(keyword);
        rule.pattern = QRegExp(pattern);
        rule.format = keyword_format;
        highlightingRules.append(rule);
    }
    this->completeSyntax();
}

void AssSyntaxHighlighter::addSyntaxKeywords(const QStringList& keywords, const QTextCharFormat& format)
{
    HighlightingRule rule;
    for(QString keyword : keywords)
    {
        QString pattern = QString(ASS_TAG_KEYWORD_QSTR).arg(keyword);
        rule.pattern = QRegExp(pattern);
        rule.format = format;
        highlightingRules.append(rule);
    }
    this->completeSyntax();
}

void AssSyntaxHighlighter::addSyntax(const QString& pattern, const QTextCharFormat& format)
{
    HighlightingRule rule;
    rule.pattern = QRegExp(pattern);
    rule.format = format;
    highlightingRules.append(rule);
    this->completeSyntax();
}

void AssSyntaxHighlighter::completeSyntax(void)
{
    isEmpty = false;
}

void AssSyntaxHighlighter::clearSyntax(void)
{
    highlightingRules.clear();
    isEmpty = true;
}

bool AssSyntaxHighlighter::isHasRules(void)
{
    return isEmpty;
}
