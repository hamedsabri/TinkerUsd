#pragma once

#include <QDialog>
#include <QPlainTextEdit>

namespace TINKERUSD_NS
{
class ContentBrowser : public QDialog
{
    Q_OBJECT
public:
    ContentBrowser(const QString& textData, QWidget* parent = nullptr);
    virtual ~ContentBrowser() = default;

private:
    QPlainTextEdit* m_editor;
};

} // namespace TINKERUSD_NS