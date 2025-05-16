#pragma once

#include <QPlainTextEdit>
#include <QPoint>
#include <QTabWidget>
#include <QWidget>

namespace TINKERUSD_NS
{
class CodeEditor;
class ScriptEditor : public QWidget
{
    Q_OBJECT
public:
    ScriptEditor(QWidget* parent = nullptr);
    virtual ~ScriptEditor();

private slots:
    void runScript();
    void clearOutput();

private:
    void        initializePython();
    void        finalizePython();
    void        setupUI();
    void        closeTab(size_t index);
    void        renameCurrentTab();
    void        showTabContextMenu(const QPoint& point);
    void        addNewTab();
    void        clearCurrentEditor();
    CodeEditor* currentScriptEditor() const;

    QTabWidget*     m_tabWidget;
    QPlainTextEdit* m_outputConsole;
};

} // namespace TINKERUSD_NS