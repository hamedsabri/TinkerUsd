#pragma once

#include "valueEntryLineEdit.h"

#include <QSlider>
#include <QWidget>

namespace TINKERUSD_NS
{

class IncrementalSliderGroup : public QWidget
{
    Q_OBJECT
public:
    IncrementalSliderGroup(bool isDouble = false, QWidget* parent = nullptr);
    virtual ~IncrementalSliderGroup() = default;
    void   setRange(double min, double max);
    void   setValue(double value);
    void   setStepSize(double stepSize);
    double value() const { return m_value; }

signals:
    void valueChanged(double newValue);

protected:
    void setupUI();

private:
    bool                m_isDouble;
    QSlider*            m_slider;
    ValueEntryLineEdit* m_valueEntry;
    double              m_minimum;
    double              m_maximum;
    double              m_value;
    double              m_stepSize;
};

} // namespace TINKERUSD_NS