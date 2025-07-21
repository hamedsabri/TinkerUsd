#include "abstractPropertyEditor.h"

namespace TINKERUSD_NS
{

AbstractPropertyEditor::AbstractPropertyEditor(
    const QString&  name,
    const QVariant& currentValue,
    const QString&  tooltip)
    : m_name(name)
    , m_tooltip(tooltip)
    , m_currentValue(currentValue)
    , m_defaultValue(currentValue)
{
}

QString AbstractPropertyEditor::name() const { return m_name; }

void AbstractPropertyEditor::setName(const QString& name) { m_name = name; }

QVariant AbstractPropertyEditor::currentValue() const { return m_currentValue; }

void AbstractPropertyEditor::setCurrentValue(const QVariant& value) { m_currentValue = value; }

bool AbstractPropertyEditor::paint(
    QPainter*                   painter,
    const QStyleOptionViewItem& option,
    const QVariant&             value) const
{
    return false;
}

QString AbstractPropertyEditor::tooltip() const { return m_tooltip; }

bool AbstractPropertyEditor::isDefault() const { return m_currentValue == m_defaultValue; }

QVariant AbstractPropertyEditor::defaultValue() const { return m_defaultValue; }

void AbstractPropertyEditor::setAttributeWrapper(UsdAttributeWrapper::Ptr attrWrapper)
{
    m_usdAttributeWrapper = std::move(attrWrapper);
}

UsdAttributeWrapper* AbstractPropertyEditor::usdAttributeWrapper() const
{
    return m_usdAttributeWrapper.get();
}

} // namespace TINKERUSD_NS
