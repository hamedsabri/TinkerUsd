#include "enumWidget.h"

#include <QtWidgets/QVBoxLayout>

namespace TINKERUSD_NS
{

PropertyComboBoxWidget::PropertyComboBoxWidget(QWidget* parent)
    : QWidget(parent)
    , m_comboBox(new NoWheelComboBox(this))
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_comboBox);
    setLayout(layout);

    connect(
        m_comboBox,
        static_cast<void (NoWheelComboBox::*)(const QString&)>(&NoWheelComboBox::currentTextChanged),
        [=](const QString&) { emit commitData(); });
}

NoWheelComboBox* PropertyComboBoxWidget::comboBox() const { return m_comboBox; }

} // namespace TINKERUSD_NS