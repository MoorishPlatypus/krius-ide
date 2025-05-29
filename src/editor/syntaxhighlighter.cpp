#include "syntaxhighlighter.h"

SyntaxHighlighter::SyntaxHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    // Set up formats
    m_keywordFormat.setForeground(QColor(86, 156, 214)); // Blue
    m_keywordFormat.setFontWeight(QFont::Bold);

    m_classFormat.setForeground(QColor(78, 201, 176)); // Teal
    m_classFormat.setFontWeight(QFont::Bold);

    m_singleLineCommentFormat.setForeground(QColor(106, 153, 85)); // Green
    m_singleLineCommentFormat.setFontItalic(true);

    m_multiLineCommentFormat.setForeground(QColor(106, 153, 85)); // Green
    m_multiLineCommentFormat.setFontItalic(true);

    m_quotationFormat.setForeground(QColor(206, 145, 120)); // Orange
    
    m_functionFormat.setForeground(QColor(220, 220, 170)); // Yellow
    m_functionFormat.setFontWeight(QFont::Bold);
    
    m_numberFormat.setForeground(QColor(181, 206, 168)); // Light green
    
    m_preprocessorFormat.setForeground(QColor(155, 155, 155)); // Gray
    
    // Default to C++ highlighting
    setupCppHighlighting();
}

void SyntaxHighlighter::setLanguage(const QString &language)
{
    if (m_currentLanguage == language) {
        return;
    }
    
    m_currentLanguage = language;
    m_highlightingRules.clear();
    
    if (language == "cpp" || language == "h" || language == "c" || language == "hpp") {
        setupCppHighlighting();
    } else if (language == "py") {
        setupPythonHighlighting();
    } else if (language == "js" || language == "ts") {
        setupJavaScriptHighlighting();
    } else {
        setupGenericHighlighting();
    }
    
    rehighlight();
}

void SyntaxHighlighter::setupCppHighlighting()
{
    HighlightingRule rule;

    // C++ keywords
    QStringList keywordPatterns;
    keywordPatterns << "\\bauto\\b" << "\\bbool\\b" << "\\bbreak\\b" << "\\bcase\\b"
                    << "\\bcatch\\b" << "\\bchar\\b" << "\\bclass\\b" << "\\bconst\\b"
                    << "\\bconstexpr\\b" << "\\bcontinue\\b" << "\\bdefault\\b" << "\\bdelete\\b"
                    << "\\bdo\\b" << "\\bdouble\\b" << "\\belse\\b" << "\\benum\\b"
                    << "\\bexplicit\\b" << "\\bextern\\b" << "\\bfalse\\b" << "\\bfloat\\b"
                    << "\\bfor\\b" << "\\bfriend\\b" << "\\bif\\b" << "\\binline\\b"
                    << "\\bint\\b" << "\\blong\\b" << "\\bnamespace\\b" << "\\bnew\\b"
                    << "\\bnoexcept\\b" << "\\bnullptr\\b" << "\\boperator\\b" << "\\bprivate\\b"
                    << "\\bprotected\\b" << "\\bpublic\\b" << "\\breturn\\b" << "\\bshort\\b"
                    << "\\bsigned\\b" << "\\bsizeof\\b" << "\\bstatic\\b" << "\\bstruct\\b"
                    << "\\bswitch\\b" << "\\btemplate\\b" << "\\bthis\\b" << "\\bthrow\\b"
                    << "\\btrue\\b" << "\\btry\\b" << "\\btypedef\\b" << "\\btypename\\b"
                    << "\\bunion\\b" << "\\bunsigned\\b" << "\\busing\\b" << "\\bvirtual\\b"
                    << "\\bvoid\\b" << "\\bvolatile\\b" << "\\bwhile\\b";

    foreach (const QString &pattern, keywordPatterns) {
        rule.pattern = QRegularExpression(pattern);
        rule.format = m_keywordFormat;
        m_highlightingRules.append(rule);
    }

    // Class names
    rule.pattern = QRegularExpression("\\b[A-Z][A-Za-z0-9_]*\\b");
    rule.format = m_classFormat;
    m_highlightingRules.append(rule);

    // Single line comments
    rule.pattern = QRegularExpression("//[^\n]*");
    rule.format = m_singleLineCommentFormat;
    m_highlightingRules.append(rule);

    // Multi-line comments
    m_commentStartExpression = QRegularExpression("/\\*");
    m_commentEndExpression = QRegularExpression("\\*/");

    // Quotations
    rule.pattern = QRegularExpression("\".*\"");
    rule.format = m_quotationFormat;
    m_highlightingRules.append(rule);

    // Functions
    rule.pattern = QRegularExpression("\\b[A-Za-z0-9_]+(?=\\()");
    rule.format = m_functionFormat;
    m_highlightingRules.append(rule);
    
    // Numbers
    rule.pattern = QRegularExpression("\\b[0-9]+\\.?[0-9]*\\b");
    rule.format = m_numberFormat;
    m_highlightingRules.append(rule);
    
    // Preprocessor directives
    rule.pattern = QRegularExpression("#[A-Za-z0-9_]+");
    rule.format = m_preprocessorFormat;
    m_highlightingRules.append(rule);
}

void SyntaxHighlighter::setupPythonHighlighting()
{
    HighlightingRule rule;

    // Python keywords
    QStringList keywordPatterns;
    keywordPatterns << "\\band\\b" << "\\bas\\b" << "\\bassert\\b" << "\\bbreak\\b"
                    << "\\bclass\\b" << "\\bcontinue\\b" << "\\bdef\\b" << "\\bdel\\b"
                    << "\\belif\\b" << "\\belse\\b" << "\\bexcept\\b" << "\\bexec\\b"
                    << "\\bfinally\\b" << "\\bfor\\b" << "\\bfrom\\b" << "\\bglobal\\b"
                    << "\\bif\\b" << "\\bimport\\b" << "\\bin\\b" << "\\bis\\b"
                    << "\\blambda\\b" << "\\bnot\\b" << "\\bor\\b" << "\\bpass\\b"
                    << "\\bprint\\b" << "\\braise\\b" << "\\breturn\\b" << "\\btry\\b"
                    << "\\bwhile\\b" << "\\bwith\\b" << "\\byield\\b" << "\\bTrue\\b"
                    << "\\bFalse\\b" << "\\bNone\\b";

    foreach (const QString &pattern, keywordPatterns) {
        rule.pattern = QRegularExpression(pattern);
        rule.format = m_keywordFormat;
        m_highlightingRules.append(rule);
    }

    // Single line comments
    rule.pattern = QRegularExpression("#[^\n]*");
    rule.format = m_singleLineCommentFormat;
    m_highlightingRules.append(rule);

    // Quotations
    rule.pattern = QRegularExpression("\".*\"|'.*'");
    rule.format = m_quotationFormat;
    m_highlightingRules.append(rule);

    // Functions
    rule.pattern = QRegularExpression("\\b[A-Za-z0-9_]+(?=\\()");
    rule.format = m_functionFormat;
    m_highlightingRules.append(rule);
    
    // Numbers
    rule.pattern = QRegularExpression("\\b[0-9]+\\.?[0-9]*\\b");
    rule.format = m_numberFormat;
    m_highlightingRules.append(rule);
}

void SyntaxHighlighter::setupJavaScriptHighlighting()
{
    HighlightingRule rule;

    // JavaScript keywords
    QStringList keywordPatterns;
    keywordPatterns << "\\bbreak\\b" << "\\bcase\\b" << "\\bcatch\\b" << "\\bclass\\b"
                    << "\\bconst\\b" << "\\bcontinue\\b" << "\\bdefault\\b" << "\\bdelete\\b"
                    << "\\bdo\\b" << "\\belse\\b" << "\\bexport\\b" << "\\bextends\\b"
                    << "\\bfalse\\b" << "\\bfinally\\b" << "\\bfor\\b" << "\\bfunction\\b"
                    << "\\bif\\b" << "\\bimport\\b" << "\\bin\\b" << "\\binstanceof\\b"
                    << "\\blet\\b" << "\\bnew\\b" << "\\bnull\\b" << "\\breturn\\b"
                    << "\\bsuper\\b" << "\\bswitch\\b" << "\\bthis\\b" << "\\bthrow\\b"
                    << "\\btrue\\b" << "\\btry\\b" << "\\btypeof\\b" << "\\bundefined\\b"
                    << "\\bvar\\b" << "\\bvoid\\b" << "\\bwhile\\b" << "\\bwith\\b"
                    << "\\byield\\b";

    foreach (const QString &pattern, keywordPatterns) {
        rule.pattern = QRegularExpression(pattern);
        rule.format = m_keywordFormat;
        m_highlightingRules.append(rule);
    }

    // Single line comments
    rule.pattern = QRegularExpression("//[^\n]*");
    rule.format = m_singleLineCommentFormat;
    m_highlightingRules.append(rule);

    // Multi-line comments
    m_commentStartExpression = QRegularExpression("/\\*");
    m_commentEndExpression = QRegularExpression("\\*/");

    // Quotations
    rule.pattern = QRegularExpression("\".*\"|'.*'|`.*`");
    rule.format = m_quotationFormat;
    m_highlightingRules.append(rule);

    // Functions
    rule.pattern = QRegularExpression("\\b[A-Za-z0-9_]+(?=\\()");
    rule.format = m_functionFormat;
    m_highlightingRules.append(rule);
    
    // Numbers
    rule.pattern = QRegularExpression("\\b[0-9]+\\.?[0-9]*\\b");
    rule.format = m_numberFormat;
    m_highlightingRules.append(rule);
}

void SyntaxHighlighter::setupGenericHighlighting()
{
    HighlightingRule rule;

    // Numbers
    rule.pattern = QRegularExpression("\\b[0-9]+\\.?[0-9]*\\b");
    rule.format = m_numberFormat;
    m_highlightingRules.append(rule);

    // Quotations
    rule.pattern = QRegularExpression("\".*\"|'.*'");
    rule.format = m_quotationFormat;
    m_highlightingRules.append(rule);
}

void SyntaxHighlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, m_highlightingRules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    // Handle multi-line comments
    setCurrentBlockState(0);

    QRegularExpressionMatch startMatch = m_commentStartExpression.match(text);
    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = startMatch.capturedStart();

    while (startIndex >= 0) {
        QRegularExpressionMatch endMatch = m_commentEndExpression.match(text, startIndex);
        int endIndex = endMatch.capturedStart();
        int commentLength = 0;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex + endMatch.capturedLength();
        }
        setFormat(startIndex, commentLength, m_multiLineCommentFormat);
        startMatch = m_commentStartExpression.match(text, startIndex + commentLength);
        startIndex = startMatch.capturedStart();
    }
}