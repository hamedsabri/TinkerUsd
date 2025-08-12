#include "loggerWidget.h"

#include <QDateTime>
#include <QTextEdit>
#include <QFile>
#include <QTextStream>
#include <QColor>
#include <QContextMenuEvent>
#include <QMenu>

namespace TINKERUSD_NS
{

LogWidget* LogWidget::s_instance = nullptr;

LogWidget::LogWidget(QWidget* parent)
    : QPlainTextEdit(parent)
{
    setReadOnly(true);
}

LogWidget& LogWidget::instance(QWidget* parent)
{
    if (!s_instance) {
        s_instance = new LogWidget(parent);
    }
    return *s_instance;
}

void LogWidget::installMessageHandler()
{
    qInstallMessageHandler(LogWidget::messageHandler);
}

void LogWidget::setToStdout(bool enable)
{
    m_stdout = enable;
}

void LogWidget::setToFile(const QString& filePath)
{
    QMutexLocker locker(&m_mutex);
    if (m_file) {
        m_file->close();
        delete m_file;
        m_file = nullptr;
    }
    m_file = new QFile(filePath);
    if (!m_file->open(QIODevice::Append | QIODevice::Text)) {
        delete m_file;
        m_file = nullptr;
    }
}

void LogWidget::clearLog()
{
    QMetaObject::invokeMethod(this, &QPlainTextEdit::clear, Qt::QueuedConnection);
}

void LogWidget::messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    Q_UNUSED(context);
    instance().appendMessage(type, msg);
}

void LogWidget::appendMessage(QtMsgType type, const QString& msg)
{
    QMutexLocker locker(&m_mutex);

    QString time = QDateTime::currentDateTime().toString("HH:mm:ss");
    QString level;
    QString color;

    // https://doc.qt.io/qt-6/qtlogging.html
    switch (type) {
        case QtDebugMsg:    level = "DEBUG";   color = "lightgray"; break;
        case QtInfoMsg:     level = "INFO";    color = "white";     break;
        case QtWarningMsg:  level = "WARN";    color = "orange";    break;
        case QtCriticalMsg: level = "ERROR";   color = "red";       break;
        case QtFatalMsg:    level = "FATAL";   color = "darkred";   break;
    }

    QString formatted = QString("[%1] [%2] %3").arg(time, level, msg);
    QString escaped = formatted.toHtmlEscaped();
    QString html = QString("<span style='color:%1;'>%2</span>").arg(color, escaped);

    QMetaObject::invokeMethod(this, [this, html]() {
        this->appendHtml(html);
    }, Qt::QueuedConnection);

    if (m_stdout) {
        QTextStream(stdout) << formatted << Qt::endl;
    }

    if (m_file) {
        QTextStream(m_file) << formatted << "\n";
        m_file->flush();
    }

    if (type == QtFatalMsg) {
        abort();
    }
}

void LogWidget::contextMenuEvent(QContextMenuEvent* event)
{
    QMenu menu(this);

    menu.addAction(tr("Clear Log"), this, &LogWidget::clearLog);

    menu.addSeparator();
    menu.addActions(createStandardContextMenu()->actions());

    menu.exec(event->globalPos());
}

} // namespace TINKERUSD_NS