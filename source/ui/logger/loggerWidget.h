#pragma once

#include <QPlainTextEdit>
#include <QMutex>
#include <QFile>

namespace TINKERUSD_NS
{

class LogWidget : public QPlainTextEdit
{
    Q_OBJECT

public:
    static LogWidget& instance(QWidget* parent = nullptr);

    void installMessageHandler();
    void setToStdout(bool enable);
    void setToFile(const QString& filePath);
    void clearLog();

protected:
    void contextMenuEvent(QContextMenuEvent* event) override;
        
private:
    explicit LogWidget(QWidget* parent = nullptr);
    static void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg);
    void appendMessage(QtMsgType type, const QString& msg);

    QMutex m_mutex;
    bool m_stdout = true;
    QFile* m_file = nullptr;
    static LogWidget* s_instance;
};

} // namespace TINKERUSD_NS