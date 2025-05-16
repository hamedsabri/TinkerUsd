#include "arrayEditor.h"

#include <QtWidgets/QLineEdit>
#include <QtWidgets/QVBoxLayout>
#include <ranges>

static constexpr auto kMaxFullDisplay = 20;  // show full array if size <= this value
static constexpr auto kPreviewElements = 3;  // number of elements to show at start and end
static constexpr auto kDecimalPercision = 6; // control decimal percision

namespace TINKERUSD_NS
{

template <typename T>
ArrayEditorWidget<T>::ArrayEditorWidget(
    QWidget*                  parent,
    const PXR_NS::VtArray<T>& array,
    const QString&            typeName)
    : ArrayEditorWidgetBase(parent)
    , m_array(array)
    , m_typeDisplayName(typeName)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QVBoxLayout* layout = new QVBoxLayout(this);
    m_lineEdit = new QLineEdit(this);
    m_lineEdit->setText(displayData(m_array));
    m_lineEdit->setReadOnly(true);
    m_lineEdit->setCursorPosition(0);
    m_lineEdit->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    layout->addWidget(m_lineEdit);
    layout->setContentsMargins(0, 0, 0, 0);
}

template <typename T> void ArrayEditorWidget<T>::setArrayData(const QVariant& data)
{
    m_array = data.value<PXR_NS::VtArray<T>>();
}

template <typename T> QVariant ArrayEditorWidget<T>::arrayData() const
{
    return QVariant::fromValue(m_array);
}

// specialization for various types
template <> QString ArrayEditorWidget<bool>::formatToString(bool value) const
{
    return value ? "true" : "false";
}

template <> QString ArrayEditorWidget<int>::formatToString(int32_t value) const
{
    return QString::number(value);
}

template <> QString ArrayEditorWidget<uint32_t>::formatToString(uint32_t value) const
{
    return QString::number(value);
}

template <> QString ArrayEditorWidget<float>::formatToString(float value) const
{
    return QString::fromStdString(std::format("{:.1f}", value));
}

template <> QString ArrayEditorWidget<double>::formatToString(double value) const
{
    return QString::fromStdString(std::format("{:.{}g}", value, kDecimalPercision));
}

template <> QString ArrayEditorWidget<std::string>::formatToString(const std::string& value) const
{
    return QString::fromStdString(value);
}

template <>
QString ArrayEditorWidget<PXR_NS::GfVec2f>::formatToString(const PXR_NS::GfVec2f& value) const
{
    return QString::fromStdString(std::format(
        "({:.{}f}, {:.{}f})", value[0], kDecimalPercision, value[1], kDecimalPercision));
}

template <>
QString ArrayEditorWidget<PXR_NS::GfVec2d>::formatToString(const PXR_NS::GfVec2d& value) const
{
    return QString::fromStdString(std::format(
        "({:.{}g}, {:.{}g})", value[0], kDecimalPercision, value[1], kDecimalPercision));
}

template <>
QString ArrayEditorWidget<PXR_NS::GfVec3f>::formatToString(const PXR_NS::GfVec3f& value) const
{
    return QString::fromStdString(std::format(
        "({:.{}f}, {:.{}f}, {:.{}f})",
        value[0],
        kDecimalPercision,
        value[1],
        kDecimalPercision,
        value[2],
        kDecimalPercision));
}

template <>
QString ArrayEditorWidget<PXR_NS::GfVec3d>::formatToString(const PXR_NS::GfVec3d& value) const
{
    return QString::fromStdString(std::format(
        "({:.{}g}, {:.{}g}, {:.{}g})",
        value[0],
        kDecimalPercision,
        value[1],
        kDecimalPercision,
        value[2],
        kDecimalPercision));
}

template <>
QString ArrayEditorWidget<PXR_NS::TfToken>::formatToString(const PXR_NS::TfToken& value) const
{
    return QString::fromStdString(value.GetString());
}

template <typename T>
QString ArrayEditorWidget<T>::displayData(const PXR_NS::VtArray<T>& array) const
{
    // mimicking how UsdView display vt arrays data:
    // type[size]:[first_N_elements ... last_N_elements]
    QString result = QString::fromStdString(
        std::format("{}[{}]: [", m_typeDisplayName.toStdString(), array.size()));

    // display all elements in the array if the array size is small enough (<= kMaxFullDisplay)
    if (array.size() <= kMaxFullDisplay)
    {
        for (const auto& elem : array)
        {
            if (!result.endsWith("["))
            {
                result += ", ";
            }
            result += formatToString(elem);
        }
    }
    else
    {
        // first kPreviewElements
        for (const auto& elem : array | std::views::take(kPreviewElements))
        {
            if (!result.endsWith("["))
            {
                result += ", ";
            }
            result += formatToString(elem);
        }
        result += ", ..., ";
        // last kPreviewElements
        for (const auto& elem : array | std::views::drop(array.size() - kPreviewElements))
        {
            if (!result.endsWith("["))
            {
                result += ", ";
            }
            result += formatToString(elem);
        }
    }

    result += "]";
    return result;
}

template <typename T>
ArrayEditor<T>::ArrayEditor(
    const QString&            name,
    const PXR_NS::VtArray<T>& value,
    const QString&            tooltip,
    const QString&            typeName)
    : ArrayEditorBase(name, QVariant::fromValue(value), tooltip)
    , m_vtArray(value)
    , m_typeDisplayName(typeName)
{
}

template <typename T> QWidget* ArrayEditor<T>::createEditor(QWidget* parent) const
{
    m_arrayEditorWidget = new ArrayEditorWidget<T>(parent, m_vtArray, m_typeDisplayName);
    return m_arrayEditorWidget;
}

template <typename T> QString ArrayEditor<T>::getRawArrayValue() const
{
    QString result = QString("[");

    for (const auto& elem : m_vtArray)
    {
        result += m_arrayEditorWidget->formatToString(elem);
        result += ", ";
    }

    // remove the last ", "
    result.chop(2);

    result += "]";
    return result;
}

template <typename T> PXR_NS::VtValue ArrayEditor<T>::toVtValue(const QVariant& value) const
{
    return PXR_NS::VtValue(value.value<PXR_NS::VtArray<T>>());
}

template <typename T> QVariant ArrayEditor<T>::fromVtValue(const PXR_NS::VtValue& value) const
{
    return QVariant::fromValue(value.Get<PXR_NS::VtArray<T>>());
}

template <typename T>
void ArrayEditor<T>::setEditorData(QWidget* editor, const QVariant& data) const
{
    auto* arrayEditor = dynamic_cast<ArrayEditorWidgetBase*>(editor);
    if (arrayEditor)
    {
        arrayEditor->setArrayData(data);
    }
}

template <typename T> QVariant ArrayEditor<T>::editorData(QWidget* editor) const
{
    auto* arrayEditor = dynamic_cast<ArrayEditorWidgetBase*>(editor);
    if (arrayEditor)
    {
        return arrayEditor->arrayData();
    }
    return QVariant();
}

// explicit instantiations
template class ArrayEditorWidget<bool>;
template class ArrayEditorWidget<int32_t>;
template class ArrayEditorWidget<uint32_t>;
template class ArrayEditorWidget<float>;
template class ArrayEditorWidget<double>;
template class ArrayEditorWidget<std::string>;
template class ArrayEditorWidget<PXR_NS::GfVec2f>;
template class ArrayEditorWidget<PXR_NS::GfVec2d>;
template class ArrayEditorWidget<PXR_NS::GfVec3f>;
template class ArrayEditorWidget<PXR_NS::GfVec3d>;
template class ArrayEditorWidget<PXR_NS::GfVec3f>;
template class ArrayEditorWidget<PXR_NS::GfVec3d>;
template class ArrayEditorWidget<PXR_NS::TfToken>;

template class ArrayEditor<bool>;
template class ArrayEditor<int32_t>;
template class ArrayEditor<uint32_t>;
template class ArrayEditor<float>;
template class ArrayEditor<double>;
template class ArrayEditor<std::string>;
template class ArrayEditor<PXR_NS::GfVec2f>;
template class ArrayEditor<PXR_NS::GfVec2d>;
template class ArrayEditor<PXR_NS::GfVec3f>;
template class ArrayEditor<PXR_NS::GfVec3d>;
template class ArrayEditor<PXR_NS::GfVec3f>;
template class ArrayEditor<PXR_NS::GfVec3d>;
template class ArrayEditor<PXR_NS::TfToken>;

} // namespace TINKERUSD_NS