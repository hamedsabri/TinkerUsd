#pragma once

#include "modelview/usdAttributeWrapper.h"

#include <QtCore/QVariant>
#include <QtGui/QPainter>
#include <QtWidgets/QStyleOptionViewItem>
#include <QtWidgets/QWidget>

namespace TINKERUSD_NS
{

class AbstractPropertyEditor
{
public:
    AbstractPropertyEditor(
        const QString&  name,
        const QVariant& currentValue,
        const QString&  tooltip = QString());

    virtual PXR_NS::VtValue toVtValue(const QVariant& value) const = 0;
    virtual QVariant        fromVtValue(const PXR_NS::VtValue& value) const = 0;

    virtual QWidget* createEditor(QWidget* parent) const = 0;
    virtual void     setEditorData(QWidget* editor, const QVariant& data) const = 0;
    virtual QVariant editorData(QWidget* editor) const = 0;
    virtual bool
    paint(QPainter* painter, const QStyleOptionViewItem& option, const QVariant& value) const;

    QString name() const;
    void    setName(const QString& name);

    QVariant currentValue() const;
    void     setCurrentValue(const QVariant& value);

    QVariant defaultValue() const;
    bool     isDefault() const;

    QString tooltip() const;

    void                 setAttributeWrapper(UsdAttributeWrapper::Ptr usdAttrWrapper);
    UsdAttributeWrapper* usdAttributeWrapper() const;

private:
    QString                  m_name;
    QVariant                 m_currentValue;
    QVariant                 m_defaultValue;
    QString                  m_tooltip;
    UsdAttributeWrapper::Ptr m_usdAttributeWrapper;
};

} // namespace TINKERUSD_NS
