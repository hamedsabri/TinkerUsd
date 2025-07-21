#include "colorEditor.h"

#include <QColorDialog>
#include <QMouseEvent>
#include <QVBoxLayout>

namespace TINKERUSD_NS
{

ColorSwatchWidget::ColorSwatchWidget(QWidget* parent)
    : QWidget(parent)
    , m_color(Qt::white)
    , m_colorButton(new QPushButton(this))
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_colorButton);

    m_colorButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_colorButton->setStyleSheet("QPushButton { border: 1px solid #CCCCCC; background-color: white; }");

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(m_colorButton, &QPushButton::clicked, this, &ColorSwatchWidget::onButtonClicked);
}

QColor ColorSwatchWidget::color() const { return m_color; }

void ColorSwatchWidget::setColor(const QColor& color)
{
    if (m_color != color)
    {
        m_color = color;
        m_colorButton->setStyleSheet(
            QString("QPushButton { border: 1px solid #CCCCCC; background-color: %1; }").arg(m_color.name()));
        emit colorChanged(m_color);
        emit commitData();
    }
}

void ColorSwatchWidget::onButtonClicked()
{
    QColor newColor = QColorDialog::getColor(m_color, this, tr("Select Color"));

    if (newColor.isValid())
    {
        setColor(newColor);
    }
}

ColorEditor::ColorEditor(const QString& name, const QColor& value, const QString& tooltip)
    : AbstractPropertyEditor(name, value, tooltip)
{
}

PXR_NS::VtValue ColorEditor::toVtValue(const QVariant& value) const
{
    QColor color = value.value<QColor>();
    auto   typeNameHash = usdAttributeWrapper()->usdAttributeType().GetHash();
    if (typeNameHash == PXR_NS::SdfValueTypeNames->Color3f.GetHash())
    {
        return PXR_NS::VtValue(PXR_NS::GfVec3f(color.redF(), color.greenF(), color.blueF()));
    }
    return PXR_NS::VtValue(PXR_NS::GfVec4f(color.redF(), color.greenF(), color.blueF(), color.alphaF()));
}

QVariant ColorEditor::fromVtValue(const PXR_NS::VtValue& value) const
{
    if (value.IsHolding<PXR_NS::GfVec3f>())
    {
        PXR_NS::GfVec3f color = value.Get<PXR_NS::GfVec3f>();
        return QVariant(QColor::fromRgbF(color[0], color[1], color[2]));
    }
    else if (value.IsHolding<PXR_NS::GfVec4f>())
    {
        PXR_NS::GfVec4f color = value.Get<PXR_NS::GfVec4f>();
        return QVariant(QColor::fromRgbF(color[0], color[1], color[2], color[3]));
    }
    return QVariant();
}

QWidget* ColorEditor::createEditor(QWidget* parent) const
{
    ColorSwatchWidget* widget = new ColorSwatchWidget(parent);
    widget->setToolTip(tooltip());
    widget->setColor(currentValue().value<QColor>());
    return widget;
}

void ColorEditor::setEditorData(QWidget* editor, const QVariant& data) const
{
    ColorSwatchWidget* swatch = qobject_cast<ColorSwatchWidget*>(editor);
    if (swatch)
    {
        swatch->setColor(data.value<QColor>());
    }
}

QVariant ColorEditor::editorData(QWidget* editor) const
{
    ColorSwatchWidget* swatch = qobject_cast<ColorSwatchWidget*>(editor);
    if (swatch)
    {
        return swatch->color();
    }
    return QVariant();
}

} // namespace TINKERUSD_NS