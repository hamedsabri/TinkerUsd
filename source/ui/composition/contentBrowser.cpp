#include "contentBrowser.h"

#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

namespace TINKERUSD_NS
{

ContentBrowser::ContentBrowser(const QString& textData, QWidget* parent)
    : QDialog(parent)
    , m_editor(new QPlainTextEdit(this))
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle("Content Browser");

    QVBoxLayout* verticalLayout = new QVBoxLayout(this);
    setLayout(verticalLayout);

    // editor
    m_editor->setLineWrapMode(QPlainTextEdit::NoWrap);
    m_editor->setReadOnly(true);

    verticalLayout->addWidget(m_editor);

    QLineEdit*   searchLineEdit = new QLineEdit(this);
    QPushButton* searchButton = new QPushButton("Find:", this);

    QHBoxLayout* searchLayout = new QHBoxLayout;
    searchLayout->addWidget(searchButton);
    searchLayout->addWidget(searchLineEdit);
    verticalLayout->addLayout(searchLayout);

    // load data
    m_editor->setPlainText(textData);
}

} // namespace TINKERUSD_NS