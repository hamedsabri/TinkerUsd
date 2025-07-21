#include "vector3dEditor.h"

#include <QtWidgets/QLabel>

namespace TINKERUSD_NS
{

template <typename T>
Vector3DEditorWidget<T>::Vector3DEditorWidget(QWidget* parent)
    : AbstractVector3DWidget(parent)
    , m_xLineEdit(new ValueLineEdit<T>(this))
    , m_yLineEdit(new ValueLineEdit<T>(this))
    , m_zLineEdit(new ValueLineEdit<T>(this))
{
    m_xLineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_yLineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_zLineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    m_xLineEdit->setFixedHeight(20);
    m_yLineEdit->setFixedHeight(20);
    m_zLineEdit->setFixedHeight(20);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(5);

    addComponentToLayout(layout, "", m_xLineEdit, QColor(192, 57, 43));
    addComponentToLayout(layout, "", m_yLineEdit, QColor(39, 174, 96));
    addComponentToLayout(layout, "", m_zLineEdit, QColor(41, 128, 185));

    setLayout(layout);

    connect(m_xLineEdit, &ValueLineEdit<T>::valueChanged, this, &AbstractVector3DWidget::onValueEntryChanged);
    connect(m_yLineEdit, &ValueLineEdit<T>::valueChanged, this, &AbstractVector3DWidget::onValueEntryChanged);
    connect(m_zLineEdit, &ValueLineEdit<T>::valueChanged, this, &AbstractVector3DWidget::onValueEntryChanged);
}

template <typename T>
void Vector3DEditorWidget<T>::addComponentToLayout(
    QHBoxLayout*      layout,
    const QString&    labelText,
    ValueLineEdit<T>* lineEdit,
    const QColor&     backgroundColor)
{
    QLabel* label = new QLabel(labelText, this);
    label->setFixedHeight(m_xLineEdit->height());
    label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet(QString("background-color: %1; color: white; padding: 1px; "
                                 "border-top-left-radius: 1px; border-bottom-left-radius: 1px;")
                             .arg(backgroundColor.name()));

    QWidget*     container = new QWidget(this);
    QHBoxLayout* containerLayout = new QHBoxLayout(container);
    containerLayout->setContentsMargins(0, 0, 0, 0);
    containerLayout->setSpacing(0);
    containerLayout->addWidget(label);
    containerLayout->addWidget(lineEdit);

    container->setLayout(containerLayout);
    layout->addWidget(container);
}

template <typename T> QVector3D Vector3DEditorWidget<T>::getValue() const
{
    return QVector3D(m_xLineEdit->value(), m_yLineEdit->value(), m_zLineEdit->value());
}

template <typename T> void Vector3DEditorWidget<T>::setValue(const QVector3D& vec)
{
    m_xLineEdit->setValue(vec.x());
    m_yLineEdit->setValue(vec.y());
    m_zLineEdit->setValue(vec.z());
}

template <typename T> void Vector3DEditorWidget<T>::setRange(const QPair<T, T>& range)
{
    m_xLineEdit->setRange(range.first, range.second);
    m_yLineEdit->setRange(range.first, range.second);
    m_zLineEdit->setRange(range.first, range.second);
}

template <typename T>
Vector3DEditor<T>::Vector3DEditor(const QString& name, const Vector3dData<T>& data, const QString& tooltip)
    : AbstractPropertyEditor(name, QVariant::fromValue(data.m_defaultValue), tooltip)
    , m_range(data.m_range)
{
}

template <typename T> QWidget* Vector3DEditor<T>::createEditor(QWidget* parent) const
{
    Vector3DEditorWidget<T>* widgetVec3D = new Vector3DEditorWidget<T>(parent);
    widgetVec3D->setValue(currentValue().value<QVector3D>());
    widgetVec3D->setRange(m_range);
    widgetVec3D->setToolTip(tooltip());
    return widgetVec3D;
}

template <typename T> PXR_NS::VtValue Vector3DEditor<T>::toVtValue(const QVariant& value) const
{
    QVector3D vec = value.value<QVector3D>();
    if constexpr (std::is_same_v<T, float>)
    {
        return PXR_NS::VtValue(PXR_NS::GfVec3f(vec.x(), vec.y(), vec.z()));
    }
    else
    {
        return PXR_NS::VtValue(PXR_NS::GfVec3d(vec.x(), vec.y(), vec.z()));
    }
}

template <typename T> QVariant Vector3DEditor<T>::fromVtValue(const PXR_NS::VtValue& value) const
{
    if constexpr (std::is_same_v<T, float>)
    {
        if (value.IsHolding<PXR_NS::GfVec3f>())
        {
            PXR_NS::GfVec3f vec = value.Get<PXR_NS::GfVec3f>();
            return QVariant(QVector3D(vec[0], vec[1], vec[2]));
        }
    }
    else
    {
        if (value.IsHolding<PXR_NS::GfVec3d>())
        {
            PXR_NS::GfVec3d vec = value.Get<PXR_NS::GfVec3d>();
            return QVariant(QVector3D(vec[0], vec[1], vec[2]));
        }
    }
    return QVariant();
}

template <typename T> void Vector3DEditor<T>::setEditorData(QWidget* editor, const QVariant& data) const
{
    AbstractVector3DWidget* abstractVector3DWidget = dynamic_cast<AbstractVector3DWidget*>(editor);
    if (abstractVector3DWidget)
    {
        abstractVector3DWidget->setValue(data.value<QVector3D>());
    }
}

template <typename T> QVariant Vector3DEditor<T>::editorData(QWidget* editor) const
{
    AbstractVector3DWidget* abstractVector3DWidget = dynamic_cast<AbstractVector3DWidget*>(editor);
    if (abstractVector3DWidget)
    {
        return abstractVector3DWidget->getValue();
    }
    return QVariant();
}

// explicit instantiation
template class Vector3DEditorWidget<float>;
template class Vector3DEditorWidget<double>;

template class Vector3DEditor<float>;
template class Vector3DEditor<double>;

} // namespace TINKERUSD_NS