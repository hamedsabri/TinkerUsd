#include "pythonHighlighter.h"

namespace TINKERUSD_NS
{

PythonHighlighter::PythonHighlighter(QTextDocument* parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    // Keyword format
    keywordFormat.setForeground(QColor(81, 95, 141));
    keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns
        = { "\\band\\b",   "\\bas\\b",     "\\bassert\\b",   "\\bbreak\\b", "\\bclass\\b",  "\\bcontinue\\b",
            "\\bdef\\b",   "\\bdel\\b",    "\\belif\\b",     "\\belse\\b",  "\\bexcept\\b", "\\bfinally\\b",
            "\\bfor\\b",   "\\bfrom\\b",   "\\bglobal\\b",   "\\bif\\b",    "\\bimport\\b", "\\bin\\b",
            "\\bis\\b",    "\\blambda\\b", "\\bnonlocal\\b", "\\bnot\\b",   "\\bor\\b",     "\\bpass\\b",
            "\\braise\\b", "\\breturn\\b", "\\btry\\b",      "\\bwhile\\b", "\\bwith\\b",   "\\byield\\b",
            "\\bTrue\\b",  "\\bFalse\\b",  "\\bNone\\b" };
    for (const QString& pattern : keywordPatterns)
    {
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    // String format (handles both single and double quotes)
    stringFormat.setForeground(QColor(206, 145, 120));
    rule.pattern = QRegularExpression("\"(?:[^\"\\\\]|\\\\.)*\"|'(?:[^'\\\\]|\\\\.)*'");
    rule.format = stringFormat;
    highlightingRules.append(rule);

    // Number format (integers, floats, scientific notation)
    numberFormat.setForeground(QColor(104, 151, 187));
    rule.pattern = QRegularExpression("\\b[0-9]+\\.?[0-9]*[eE]?[-+]?[0-9]*\\b");
    rule.format = numberFormat;
    highlightingRules.append(rule);

    // Function format
    functionFormat.setForeground(QColor(220, 220, 170));
    rule.pattern = QRegularExpression("\\b[A-Za-z0-9_]+(?=\\()");
    rule.format = functionFormat;
    highlightingRules.append(rule);

    // Comment format
    commentFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegularExpression("#[^\n]*");
    rule.format = commentFormat;
    highlightingRules.append(rule);
}

void PythonHighlighter::highlightBlock(const QString& text)
{
    // Apply highlighting rules
    for (const HighlightingRule& rule : highlightingRules)
    {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext())
        {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    // Handle multi-line strings (triple quotes)
    setCurrentBlockState(0);
    int startIndex = 0;
    if (previousBlockState() != 1)
    {
        startIndex = text.indexOf(QRegularExpression("'''|\"\"\""));
    }

    while (startIndex >= 0)
    {
        QRegularExpression      endExpr("'''|\"\"\"");
        QRegularExpressionMatch endMatch = endExpr.match(text, startIndex + 3);
        int                     endIndex = endMatch.capturedStart();
        int                     stringLength;

        if (endIndex == -1)
        {
            setCurrentBlockState(1);
            stringLength = text.length() - startIndex;
        }
        else
        {
            stringLength = endIndex - startIndex + endMatch.capturedLength();
        }

        setFormat(startIndex, stringLength, stringFormat);
        startIndex = text.indexOf(QRegularExpression("'''|\"\"\""), startIndex + stringLength);
    }
}

} // namespace TINKERUSD_NS