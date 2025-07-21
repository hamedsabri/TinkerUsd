#pragma once

#include "abstractPropertyEditor.h"

#include <QtWidgets/QLineEdit>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QWidget>

namespace TINKERUSD_NS
{

/**
 * @struct FileEditorData
 * @brief Data structure to hold the default file path value.
 *
 */
struct FileEditorData
{
    QString m_defaultValue;

    FileEditorData()
        : m_defaultValue("")
    {
    }
    FileEditorData(const QString& defaultValue)
        : m_defaultValue(defaultValue)
    {
    }
};
Q_DECLARE_METATYPE(FileEditorData)

/**
 * @class FileEditorWidget
 * @brief A custom widget that provides a file path editor with a browse button.
 *
 */
class FileEditorWidget : public QWidget
{
    Q_OBJECT
public:
    FileEditorWidget(QWidget* parent = nullptr);

    QString filePath() const;
    void    setFilePath(const QString& path);

signals:
    // signal emitted when the data needs to be committed.
    void commitData();

private slots:
    void browse();

private:
    QLineEdit*   m_lineEdit;
    QToolButton* m_browseButton;
};

/**
 * @class FileEditorWidget
 * @brief A custom widget that provides a file path editor with a browse button.
 *
 */
class FileEditor : public AbstractPropertyEditor
{
public:
    FileEditor(const QString& name, const FileEditorData& data, const QString& tooltip = QString());

    PXR_NS::VtValue toVtValue(const QVariant& value) const override;
    QVariant        fromVtValue(const PXR_NS::VtValue& value) const override;

    QWidget* createEditor(QWidget* parent) const override;
    void     setEditorData(QWidget* editor, const QVariant& data) const override;
    QVariant editorData(QWidget* editor) const override;

private:
    QString m_defaultValue;
};

} // namespace TINKERUSD_NS