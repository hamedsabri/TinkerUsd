// must be called before anything else
extern "C" {
#include <Python.h>
}

#include "codeEditor.h"
#include "scriptEditor.h"

#include <QHBoxLayout>
#include <QInputDialog>
#include <QLineEdit>
#include <QMenu>
#include <QPushButton>
#include <QSplitter>
#include <QTabBar>
#include <QToolButton>
#include <QVBoxLayout>

namespace TINKERUSD_NS
{

// https://docs.python.org/3/c-api/import.html
// Custom stream redirection to capture Python's stdout/stderr
class PythonRedirector
{
public:
    static PythonRedirector& instance()
    {
        static PythonRedirector instance;
        return instance;
    }

    void setOutputConsole(QPlainTextEdit* console) { outputConsole = console; }

    void write(const QString& text)
    {
        if (outputConsole)
            outputConsole->appendPlainText(text);
    }

private:
    PythonRedirector()
        : outputConsole(nullptr)
    {
    }
    QPlainTextEdit* outputConsole;
};

// Python module to redirect stdout/stderr
static PyObject* redirector_write(PyObject* self, PyObject* args)
{
    const char* text;
    if (!PyArg_ParseTuple(args, "s", &text))
    {
        return nullptr;
    }

    PythonRedirector::instance().write(QString(text));
    return Py_BuildValue("");
}

static PyMethodDef RedirectorMethods[] = { { "write", redirector_write, METH_VARARGS, "Write to Qt console" },
                                           { nullptr, nullptr, 0, nullptr } };

static PyModuleDef RedirectorModule
    = { PyModuleDef_HEAD_INIT, "redirector", "Redirector module for Python output", -1, RedirectorMethods };

static PyObject* PyInit_Redirector(void) { return PyModule_Create(&RedirectorModule); }

ScriptEditor::ScriptEditor(QWidget* parent)
    : QWidget(parent)
{
    initializePython();
    setupUI();
}

ScriptEditor::~ScriptEditor() { finalizePython(); }

void ScriptEditor::initializePython()
{
    PyImport_AppendInittab("redirector", &PyInit_Redirector);
    Py_Initialize();
    PyRun_SimpleString("import sys\n"
                       "import redirector\n"
                       "class StdoutRedirector:\n"
                       "    def write(self, text):\n"
                       "        redirector.write(text)\n"
                       "    def flush(self):\n"
                       "        pass\n"
                       "sys.stdout = StdoutRedirector()\n"
                       "sys.stderr = StdoutRedirector()\n");
}

void ScriptEditor::finalizePython()
{
    if (Py_IsInitialized())
    {
        Py_Finalize();
    }
}

void ScriptEditor::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QSplitter* splitter = new QSplitter(Qt::Vertical);

    m_tabWidget = new QTabWidget();
    m_tabWidget->setTabsClosable(true);
    m_tabWidget->setMovable(true);

    m_tabWidget->tabBar()->setContextMenuPolicy(Qt::CustomContextMenu);

    QToolButton* addTabButton = new QToolButton(this);
    addTabButton->setText("+");
    addTabButton->setToolTip("Add new tab");
    m_tabWidget->setCornerWidget(addTabButton, Qt::TopRightCorner);

    QTabBar* tabBar = m_tabWidget->tabBar();
    tabBar->setStyleSheet(R"(
        QTabBar::close-button {
            image: url(:/close.png);
            margin-left: 2px;
        }
        QTabBar::close-button:hover {
            image: url(:/close.png);
        }
        QTabBar::close-button:pressed {
            image: url(:/close.png);
        }
    )");

    CodeEditor* firstEditor = new CodeEditor();

    m_tabWidget->addTab(firstEditor, QString("Python"));

    m_outputConsole = new QPlainTextEdit();
    m_outputConsole->setReadOnly(true);
    m_outputConsole->setMaximumBlockCount(40000);

    PythonRedirector::instance().setOutputConsole(m_outputConsole);

    QToolButton* runButton = new QToolButton();
    QIcon        runIcon(":/run_script.png");
    runButton->setIcon(runIcon);
    runButton->setToolTip("Run Script");
    runButton->setFixedSize(24, 24);       // Optional: to match icon size
    runButton->setIconSize(QSize(24, 24)); // Match your icon's viewBox size
    runButton->setAutoRaise(true);
    runButton->setStyleSheet("background: transparent;");

    QToolButton* clearOutputButton = new QToolButton();
    QIcon        clearOutputIcon(":/clear_output.png");
    clearOutputButton->setIcon(clearOutputIcon);
    clearOutputButton->setToolTip("clear output");
    clearOutputButton->setFixedSize(24, 24);       // Optional: to match icon size
    clearOutputButton->setIconSize(QSize(24, 24)); // Match your icon's viewBox size
    clearOutputButton->setStyleSheet("background: transparent;");

    QToolButton* clearEditorButton = new QToolButton();
    QIcon        clearEditorIcon(":/clear_input.png");
    clearEditorButton->setIcon(clearEditorIcon);
    clearEditorButton->setToolTip("clear input");
    clearEditorButton->setFixedSize(24, 24);       // Optional: to match icon size
    clearEditorButton->setIconSize(QSize(24, 24)); // Match your icon's viewBox size
    clearEditorButton->setStyleSheet("background: transparent;");

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(runButton);
    buttonLayout->addWidget(clearEditorButton);
    buttonLayout->addWidget(clearOutputButton);
    buttonLayout->addStretch();

    splitter->addWidget(m_outputConsole);
    splitter->addWidget(m_tabWidget);

    splitter->setStretchFactor(0, 2);
    splitter->setStretchFactor(1, 1);

    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(splitter);

    connect(runButton, &QPushButton::clicked, this, &ScriptEditor::runScript);
    connect(clearOutputButton, &QPushButton::clicked, this, &ScriptEditor::clearOutput);
    connect(clearEditorButton, &QPushButton::clicked, this, &ScriptEditor::clearCurrentEditor);
    connect(addTabButton, &QToolButton::clicked, this, &ScriptEditor::addNewTab);
    connect(
        m_tabWidget->tabBar(), &QTabBar::customContextMenuRequested, this, &ScriptEditor::showTabContextMenu);
    connect(m_tabWidget, &QTabWidget::tabCloseRequested, this, &ScriptEditor::closeTab);
}

CodeEditor* ScriptEditor::currentScriptEditor() const
{
    // Get the current tab's script editor
    int currentIndex = m_tabWidget->currentIndex();
    if (currentIndex >= 0)
    {
        return qobject_cast<CodeEditor*>(m_tabWidget->widget(currentIndex));
    }
    return nullptr;
}

void ScriptEditor::runScript()
{
    CodeEditor* scriptEditor = currentScriptEditor();
    if (!scriptEditor)
        return;

    QString script = scriptEditor->toPlainText();
    if (script.isEmpty())
        return;

    // Execute the Python script
    PyObject* main = PyImport_AddModule("__main__");
    PyObject* globals = PyModule_GetDict(main);
    PyObject* locals = PyDict_New();

    PyObject* result = PyRun_String(script.toUtf8().constData(), Py_file_input, globals, locals);

    if (result)
    {
        Py_DECREF(result);
        m_outputConsole->appendPlainText(script);
    }
    else
    {
        PyErr_Print();
    }

    Py_DECREF(locals);
}

void ScriptEditor::clearOutput()
{
    if (m_outputConsole)
    {
        m_outputConsole->clear();
    }
}

void ScriptEditor::clearCurrentEditor()
{
    CodeEditor* scriptEditor = currentScriptEditor();
    if (scriptEditor)
    {
        scriptEditor->clear();
    }
}

void ScriptEditor::addNewTab()
{
    CodeEditor* newEditor = new CodeEditor();

    int newIndex = m_tabWidget->addTab(newEditor, QString("Python"));

    m_tabWidget->setCurrentIndex(newIndex);
}

void ScriptEditor::showTabContextMenu(const QPoint& point)
{
    int tabIndex = m_tabWidget->tabBar()->tabAt(point);
    if (tabIndex < 0)
        return;

    QMenu    menu(this);
    QAction* renameAction = menu.addAction("Rename Tab");
    QAction* closeAction = menu.addAction("Close Tab");

    connect(renameAction, &QAction::triggered, this, &ScriptEditor::renameCurrentTab);

    // don't allow closing the last tab
    if (m_tabWidget->count() > 1)
    {
        connect(closeAction, &QAction::triggered, [=]() { this->closeTab(tabIndex); });
    }
    else
    {
        closeAction->setEnabled(false);
    }

    menu.exec(m_tabWidget->tabBar()->mapToGlobal(point));
}

void ScriptEditor::renameCurrentTab()
{
    auto currentIndex = m_tabWidget->currentIndex();
    if (currentIndex >= 0)
    {
        QString currentName = m_tabWidget->tabText(currentIndex);

        bool    ok;
        QString newName = QInputDialog::getText(
            this, tr("Rename Tab"), tr("Tab name:"), QLineEdit::Normal, currentName, &ok);
        if (ok && !newName.isEmpty())
        {
            m_tabWidget->setTabText(currentIndex, newName);
        }
    }
}

void ScriptEditor::closeTab(size_t index)
{
    if (m_tabWidget->count() <= 1)
    {
        return;
    }

    QWidget* widget = m_tabWidget->widget(index);
    m_tabWidget->removeTab(index);
    delete widget;
}

} // namespace TINKERUSD_NS
