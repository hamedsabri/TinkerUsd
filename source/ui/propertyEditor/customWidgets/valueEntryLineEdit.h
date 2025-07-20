#pragma once

#include <QtWidgets/QWidget>
#include <QtWidgets/QLineEdit>
#include <QtGui/QValidator>

namespace TINKERUSD_NS
{

class DoubleValueLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    DoubleValueLineEdit(QWidget* parent = nullptr);

    void setMinimum(double min);
    double getMinimum() const { return m_minimum; }

    void setMaximum(double max);
    double getMaximum() const { return m_maximum; }

    void setRange(double min, double max);
    void setDecimals(int32_t decimals);
    double value() const { return m_value; }

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;

public slots:
    void setValue(double value);

signals:
    void valueChanged(double value);
    void contextMenuRequested(QContextMenuEvent* event);

private slots:
    void onEditingFinished();
    void onReturnPressed();

private:
    void validateText();

    QString m_previouslyValidatedText;
    QDoubleValidator* m_validator;
    double m_minimum;
    double m_maximum;
    double m_value;
    int32_t m_decimals;
};


class IntValueLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    IntValueLineEdit(QWidget* parent = nullptr);

    void setMinimum(int32_t min);
    int32_t getMinimum() const { return m_minimum; }

    void setMaximum(int32_t max);
    int32_t getMaximum() const { return m_maximum; }
    
    void setRange(int32_t min, int32_t max);
    double value() const { return m_value; }

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;

public slots:
    void setValue(int32_t value);

signals:
    void valueChanged(int32_t value);
    void contextMenuRequested(QContextMenuEvent* event);

private slots:
    void onEditingFinished();
    void onReturnPressed();

private:
    void validateText();

    QIntValidator* m_validator;
    int32_t m_minimum;
    int32_t m_maximum;
    int32_t m_value;
};

} // namespace TINKERUSD_NS