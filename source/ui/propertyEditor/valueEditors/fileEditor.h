#pragma once

#include "abstractPropertyEditor.h"

#include <QFileDialog>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QToolButton>
#include <QWidget>

namespace TINKERUSD_NS
{

class FileEditorWidget : public QWidget
{
    Q_OBJECT
public:
    FileEditorWidget(QWidget* parent = nullptr);

    QString filePath() const;
    void    setFilePath(const QString& path);

signals:
    void commitData();

private slots:
    void browse();

private:
    QLineEdit*   m_lineEdit;
    QToolButton* m_browseButton;
};

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