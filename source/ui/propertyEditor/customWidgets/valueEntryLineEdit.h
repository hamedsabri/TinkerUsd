#pragma once

#include <QLineEdit>
#include <QPoint>

namespace TINKERUSD_NS
{

class ValueEntryLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    ValueEntryLineEdit(bool isDouble = false, QWidget* parent = nullptr);
    virtual ~ValueEntryLineEdit() = default;

    void setValue(double value);
    double value() const { return m_value; }
    
    void setPrecision(int decimals) { m_decimals = decimals; }
    void setStepSize(double stepSize);
    
    double stepSize() const { return m_stepSize; }
    void setRange(double min, double max);

signals:
    void valueChanged(double newValue);

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    bool   m_isDouble;
    double m_value;
    int    m_decimals;
    double m_stepSize;
    double m_minimum;
    double m_maximum;
    QPoint m_lastMousePos;
    bool   m_dragging;
};

} // TINKERUSD_NS