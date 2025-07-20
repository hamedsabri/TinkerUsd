#include "vector2dEditor.h"

#include <QtWidgets/QLabel>

namespace TINKERUSD_NS
{

template<typename T>
Vector2DEditorWidget<T>::Vector2DEditorWidget(QWidget *parent)
    : AbstractVector2DWidget(parent)
    , m_xLineEdit(new DoubleValueLineEdit(this))
    , m_yLineEdit(new DoubleValueLineEdit(this))
{
    m_xLineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_yLineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    m_xLineEdit->setFixedHeight(20);
    m_yLineEdit->setFixedHeight(20);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(5);

    addComponentToLayout(layout, "", m_xLineEdit, QColor(192, 57, 42));
    addComponentToLayout(layout, "", m_yLineEdit, QColor(29, 174, 96));

    setLayout(layout);

    connect(m_xLineEdit, &DoubleValueLineEdit::valueChanged, this, &AbstractVector2DWidget::onValueEntryChanged);
    connect(m_yLineEdit, &DoubleValueLineEdit::valueChanged, this, &AbstractVector2DWidget::onValueEntryChanged);
}

template<typename T>
void Vector2DEditorWidget<T>::addComponentToLayout(QHBoxLayout* layout, const QString& labelText, DoubleValueLineEdit* lineEdit, const QColor& backgroundColor)
{
    QLabel* label = new QLabel(labelText, this);
    label->setFixedHeight(m_xLineEdit->height());
    label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet(QString("background-color: %1; color: white; padding: 1px; border-top-left-radius: 1px; border-bottom-left-radius: 1px;")
        .arg(backgroundColor.name()));

    QWidget* container = new QWidget(this);
    QHBoxLayout* containerLayout = new QHBoxLayout(container);
    containerLayout->setContentsMargins(0, 0, 0, 0);
    containerLayout->setSpacing(0);
    containerLayout->addWidget(label);
    containerLayout->addWidget(lineEdit);

    container->setLayout(containerLayout);
    layout->addWidget(container);
}

template<typename T>
QVector2D Vector2DEditorWidget<T>::getValue() const
{
    return QVector2D(m_xLineEdit->value(), m_yLineEdit->value());
}

template<typename T>
void Vector2DEditorWidget<T>::setValue(const QVector2D &vec)
{
    m_xLineEdit->setValue(vec.x());
    m_yLineEdit->setValue(vec.y());
}

template<typename T>
void Vector2DEditorWidget<T>::setRange(const QPair<T, T>& range)
{
    m_xLineEdit->setRange(range.first, range.second);
    m_yLineEdit->setRange(range.first, range.second);
}

template<typename T>
Vector2DEditor<T>::Vector2DEditor(const QString &name, const Vector2dData<T>& data, const QString &tooltip)
    : AbstractPropertyEditor(name, QVariant::fromValue(data.m_defaultValue), tooltip)
    , m_range(data.m_range)
{
}

template<typename T>
QWidget* Vector2DEditor<T>::createEditor(QWidget* parent) const
{
    Vector2DEditorWidget<T>* widgetVec2D = new Vector2DEditorWidget<T>(parent);
    widgetVec2D->setValue(currentValue().value<QVector2D>());
    widgetVec2D->setRange(m_range);
    widgetVec2D->setToolTip(tooltip());
    return widgetVec2D;
}

template<typename T>
PXR_NS::VtValue Vector2DEditor<T>::toVtValue(const QVariant& value) const
{
    QVector2D vec = value.value<QVector2D>();
    if constexpr (std::is_same_v<T, float>) {
        return PXR_NS::VtValue(PXR_NS::GfVec2f(vec.x(), vec.y()));
    } else {
        return PXR_NS::VtValue(PXR_NS::GfVec2d(vec.x(), vec.y()));
    }
}

template<typename T>
QVariant Vector2DEditor<T>::fromVtValue(const PXR_NS::VtValue& value) const
{
    if constexpr (std::is_same_v<T, float>) {
        if (value.IsHolding<PXR_NS::GfVec2f>()) {
            PXR_NS::GfVec2f vec = value.Get<PXR_NS::GfVec2f>();
            return QVariant(QVector2D(vec[0], vec[1]));
        }
    } else {
        if (value.IsHolding<PXR_NS::GfVec2d>()) {
            PXR_NS::GfVec2d vec = value.Get<PXR_NS::GfVec2d>();
            return QVariant(QVector2D(vec[0], vec[1]));
        }
    }
    return QVariant();
}

template<typename T>
void Vector2DEditor<T>::setEditorData(QWidget* editor, const QVariant& data) const
{
    AbstractVector2DWidget* vectorEditor = dynamic_cast<AbstractVector2DWidget*>(editor);
    if (vectorEditor) {
        vectorEditor->setValue(data.value<QVector2D>());
    }
}

template<typename T>
QVariant Vector2DEditor<T>::editorData(QWidget* editor) const
{
    AbstractVector2DWidget* vectorEditor = dynamic_cast<AbstractVector2DWidget*>(editor);
    if (vectorEditor) {
        return vectorEditor->getValue();
    }
    return QVariant();
}

// explicit instantiation 
template class Vector2DEditorWidget<float>;
template class Vector2DEditorWidget<double>;

template class Vector2DEditor<float>;
template class Vector2DEditor<double>;

} // namespace TINKERUSD_NS
