#pragma once

#include <QObject>
#include <QPlainTextEdit>

namespace TINKERUSD_NS
{

class LineNumberArea;

class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    CodeEditor(QWidget* parent = nullptr);
    virtual ~CodeEditor() = default;

    void lineNumberAreaPaintEvent(QPaintEvent* event);
    int  lineNumberAreaWidth();

protected:
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect& rect, int dy);

private:
    QWidget* lineNumberArea;
};

class LineNumberArea : public QWidget
{
public:
    LineNumberArea(CodeEditor* editor)
        : QWidget(editor)
        , codeEditor(editor)
    {
    }

    QSize sizeHint() const override { return QSize(codeEditor->lineNumberAreaWidth(), 0); }

protected:
    void paintEvent(QPaintEvent* event) override { codeEditor->lineNumberAreaPaintEvent(event); }

private:
    CodeEditor* codeEditor;
};

} // namespace TINKERUSD_NS