#include "fileEditor.h"

#include <QtWidgets/QFileDialog>
#include <QtWidgets/QHBoxLayout>

namespace TINKERUSD_NS
{

FileEditorWidget::FileEditorWidget(QWidget* parent)
    : QWidget(parent)
    , m_lineEdit(new QLineEdit(this))
    , m_browseButton(new QToolButton(this))
{
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0); // Adjust spacing here
    layout->addWidget(m_lineEdit);
    layout->addWidget(m_browseButton);

    // Set the icon for the browse button
    QIcon browseIcon(":/browse.png");
    m_browseButton->setIcon(browseIcon);
    // m_browseButton->setIconSize(m_browseButton->size());
    // m_browseButton->setFixedSize(24,24);
    m_browseButton->setIconSize(QSize(24, 24));
    m_browseButton->setToolTip("Browse");
    m_browseButton->setText("");
    m_browseButton->setAutoRaise(true);
    m_browseButton->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding));
    m_browseButton->setStyleSheet(
        "QToolButton { border: none; padding: 0px; } QToolButton::hover { background: none; }");
    m_lineEdit->setPlaceholderText("Path to a USD file...");

    connect(m_browseButton, &QToolButton::clicked, this, &FileEditorWidget::browse);
    connect(m_lineEdit, &QLineEdit::editingFinished, this, &FileEditorWidget::commitData);
}

QString FileEditorWidget::filePath() const { return m_lineEdit->text(); }

void FileEditorWidget::setFilePath(const QString& path) { m_lineEdit->setText(path); }

void FileEditorWidget::browse()
{
    QString filePath = QFileDialog::getOpenFileName(
        this, tr("Select a USD file"), "", tr("USD Files (*.usd *.usda *.usdc)"));
    if (!filePath.isEmpty())
    {
        setFilePath(filePath);
        emit commitData();
    }
}

FileEditor::FileEditor(const QString& name, const FileEditorData& data, const QString& tooltip)
    : AbstractPropertyEditor(name, data.m_defaultValue, tooltip)
    , m_defaultValue(data.m_defaultValue)
{
}

PXR_NS::VtValue FileEditor::toVtValue(const QVariant& value) const
{
    // Assuming the file path is stored as a string
    return PXR_NS::VtValue(PXR_NS::SdfAssetPath(value.toString().toStdString()));
}

QVariant FileEditor::fromVtValue(const PXR_NS::VtValue& value) const
{
    if (value.IsHolding<PXR_NS::SdfAssetPath>())
    {
        PXR_NS::SdfAssetPath assetPath = value.Get<PXR_NS::SdfAssetPath>();
        return QVariant(QString::fromStdString(assetPath.GetAssetPath()));
    }
    return QVariant();
}

QWidget* FileEditor::createEditor(QWidget* parent) const
{
    FileEditorWidget* widget = new FileEditorWidget(parent);
    widget->setToolTip(tooltip());
    widget->setFilePath(m_defaultValue);
    return widget;
}

void FileEditor::setEditorData(QWidget* editor, const QVariant& data) const
{
    FileEditorWidget* fileEditor = qobject_cast<FileEditorWidget*>(editor);
    if (fileEditor)
    {
        fileEditor->setFilePath(data.toString());
    }
}

QVariant FileEditor::editorData(QWidget* editor) const
{
    FileEditorWidget* fileEditor = qobject_cast<FileEditorWidget*>(editor);
    if (fileEditor)
    {
        return fileEditor->filePath();
    }
    return QVariant();
}

} // namespace TINKERUSD_NS