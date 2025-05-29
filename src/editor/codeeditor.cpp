#include "codeeditor.h"
#include "syntaxhighlighter.h"

#include <QPainter>
#include <QTextBlock>
#include <QKeyEvent>
#include <QCompleter>
#include <QAbstractItemView>
#include <QScrollBar>
#include <QFileInfo>

CodeEditor::CodeEditor(QWidget *parent)
    : QPlainTextEdit(parent)
    , m_lineNumberArea(nullptr)
    , m_syntaxHighlighter(nullptr)
{
    setupEditor();
}

void CodeEditor::setupEditor()
{
    // Set up line number area
    m_lineNumberArea = new LineNumberArea(this);
    
    // Set up syntax highlighter
    m_syntaxHighlighter = new SyntaxHighlighter(document());
    
    // Connect signals
    connect(this, &CodeEditor::blockCountChanged, this, &CodeEditor::updateLineNumberAreaWidth);
    connect(this, &CodeEditor::updateRequest, this, &CodeEditor::updateLineNumberArea);
    connect(this, &CodeEditor::cursorPositionChanged, this, &CodeEditor::highlightCurrentLine);
    
    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
    
    // Set editor properties
    setTabStopWidth(4 * fontMetrics().width(' '));
    setLineWrapMode(QPlainTextEdit::NoWrap);
    
    // Set font
    QFont font("Consolas", 10);
    font.setFixedPitch(true);
    setFont(font);
    
    // Set colors for dark theme
    QPalette palette = this->palette();
    palette.setColor(QPalette::Base, QColor(30, 30, 30));
    palette.setColor(QPalette::Text, QColor(220, 220, 220));
    setPalette(palette);
}

void CodeEditor::setCurrentFile(const QString &fileName)
{
    m_currentFile = fileName;
    
    // Update syntax highlighter based on file extension
    QFileInfo fileInfo(fileName);
    QString extension = fileInfo.suffix().toLower();
    m_syntaxHighlighter->setLanguage(extension);
}

int CodeEditor::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;
    return space;
}

void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        m_lineNumberArea->scroll(0, dy);
    else
        m_lineNumberArea->update(0, rect.y(), m_lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void CodeEditor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    m_lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(50, 50, 50);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(m_lineNumberArea);
    painter.fillRect(event->rect(), QColor(40, 40, 40));

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(QColor(120, 120, 120));
            painter.drawText(0, top, m_lineNumberArea->width(), fontMetrics().height(),
                           Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}

void CodeEditor::keyPressEvent(QKeyEvent *event)
{
    // Handle auto-indentation
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        QPlainTextEdit::keyPressEvent(event);
        autoIndent();
        return;
    }
    
    // Handle tab completion
    if (event->key() == Qt::Key_Tab) {
        // Insert 4 spaces instead of tab
        insertPlainText("    ");
        return;
    }
    
    // Handle auto-completion for brackets
    if (event->text() == "{") {
        insertPlainText("{}");
        QTextCursor cursor = textCursor();
        cursor.movePosition(QTextCursor::Left);
        setTextCursor(cursor);
        return;
    }
    
    if (event->text() == "(") {
        insertPlainText("()");
        QTextCursor cursor = textCursor();
        cursor.movePosition(QTextCursor::Left);
        setTextCursor(cursor);
        return;
    }
    
    if (event->text() == "[") {
        insertPlainText("[]");
        QTextCursor cursor = textCursor();
        cursor.movePosition(QTextCursor::Left);
        setTextCursor(cursor);
        return;
    }
    
    if (event->text() == "\"") {
        insertPlainText("\"\"");
        QTextCursor cursor = textCursor();
        cursor.movePosition(QTextCursor::Left);
        setTextCursor(cursor);
        return;
    }
    
    QPlainTextEdit::keyPressEvent(event);
}

void CodeEditor::autoIndent()
{
    QTextCursor cursor = textCursor();
    QTextBlock previousBlock = cursor.block().previous();
    
    if (previousBlock.isValid()) {
        QString previousText = previousBlock.text();
        QString indent;
        
        // Extract indentation from previous line
        for (int i = 0; i < previousText.length(); ++i) {
            if (previousText[i] == ' ' || previousText[i] == '\t') {
                indent += previousText[i];
            } else {
                break;
            }
        }
        
        // Add extra indentation if previous line ends with {
        if (previousText.trimmed().endsWith('{')) {
            indent += "    ";
        }
        
        insertPlainText(indent);
    }
}