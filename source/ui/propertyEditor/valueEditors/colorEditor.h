#pragma once

#include "abstractPropertyEditor.h"

#include <QColor>
#include <QPushButton>

namespace TINKERUSD_NS
{

class ColorSwatchWidget : public QWidget
{
    Q_OBJECT
public:
    ColorSwatchWidget(QWidget* parent = nullptr);
    QColor color() const;
    void   setColor(const QColor& color);

signals:
    void colorChanged(const QColor& color);
    void commitData();

private slots:
    void onButtonClicked();

private:
    QColor       m_color;
    QPushButton* m_colorButton;
};

class ColorEditor : public AbstractPropertyEditor
{
public:
    ColorEditor(const QString& name, const QColor& value, const QString& tooltip = QString());

    PXR_NS::VtValue toVtValue(const QVariant& value) const override;
    QVariant        fromVtValue(const PXR_NS::VtValue& value) const override;

    QWidget* createEditor(QWidget* parent) const override;
    void     setEditorData(QWidget* editor, const QVariant& data) const override;
    QVariant editorData(QWidget* editor) const override;
};

} // namespace TINKERUSD_NS
