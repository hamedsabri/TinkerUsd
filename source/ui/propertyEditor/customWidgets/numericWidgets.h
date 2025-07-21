#pragma once

#include <QtCore/QVariant>
#include <QtGui/QMouseEvent>
#include <QtGui/QValidator>
#include <QtGui/QWheelEvent>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSlider>
#include <QtWidgets/QWidget>
#include <algorithm>
#include <cmath>
#include <type_traits>

namespace TINKERUSD_NS
{

class AttributeEditorSliderClass : public QSlider
{
    Q_OBJECT
public:
    AttributeEditorSliderClass(QWidget* parent = nullptr)
        : QSlider(parent)
    {
        setFocusPolicy(Qt::StrongFocus);
    }

protected:
    void wheelEvent(QWheelEvent* event) override { event->ignore(); }
    void mousePressEvent(QMouseEvent* event) override
    {
        setSliderDown(true);
        QSlider::mousePressEvent(event);
    }
};

class ValueLineEditBase : public QLineEdit
{
    Q_OBJECT
public:
    ValueLineEditBase(QWidget* parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;

public slots:
    virtual void setValue(const QVariant& value) = 0;

signals:
    void valueChanged(const QVariant& value);
    void contextMenuRequested(QContextMenuEvent* event);

protected slots:
    virtual void validateText() = 0;
    void         onEditingFinished();
    void         onReturnPressed();
};

template <typename T> class ValueLineEdit : public ValueLineEditBase
{
public:
    static_assert(std::is_arithmetic_v<T>, "ValueLineEdit requires arithmetic type");

    explicit ValueLineEdit(QWidget* parent = nullptr);

    void setMinimum(T min);
    T    getMinimum() const { return m_minimum; }

    void setMaximum(T max);
    T    getMaximum() const { return m_maximum; }

    void setRange(T min, T max);

    void    setDecimals(int32_t decimals);
    int32_t getDecimals() const { return m_decimals; }

    T value() const { return m_value; }

    void setValue(const QVariant& value) override;

protected:
    void validateText() override;

private:
    QString     m_previouslyValidatedText;
    QValidator* m_validator;
    T           m_minimum;
    T           m_maximum;
    T           m_value;
    int32_t     m_decimals = std::is_integral_v<T> ? 0 : 3;
};

class SliderGroupBase : public QFrame
{
    Q_OBJECT
public:
    SliderGroupBase(QWidget* parent = nullptr);

public slots:
    virtual void setValue(const QVariant& value) = 0;

signals:
    void sliderPressed();
    void sliderMoved(const QVariant& value);
    void sliderReleased();
    void valueUpdated(const QVariant& value);
    void contextMenuRequested(QContextMenuEvent* event);

protected slots:
    virtual void onSliderPressed() = 0;
    virtual void onSliderMoved(int value) = 0;
    virtual void onSliderReleased() = 0;
    virtual void onSpinBoxEditingFinished() = 0;
};

template <typename T> class SliderGroup : public SliderGroupBase
{
public:
    static_assert(std::is_arithmetic_v<T>, "SliderGroup requires arithmetic type");

    SliderGroup(T min, T max, T value, QWidget* parent = nullptr);
    explicit SliderGroup(QWidget* parent = nullptr);

    ValueLineEdit<T>*           valueEntry() const { return m_valueEntry; }
    AttributeEditorSliderClass* slider() const { return m_slider; }

    void setMinimum(T min);
    T    getMinimum() const { return m_minimum; }

    void setMaximum(T max);
    T    getMaximum() const { return m_maximum; }

    void setRange(T min, T max);

    void setSoftMinimum(T min);
    T    getSoftMinimum() const { return m_softMinimum; }

    void setSoftMaximum(T max);
    T    getSoftMaximum() const { return m_softMaximum; }

    void setUseSoftMinimum(bool use);
    bool isUsingSoftMinimum() const { return m_useSoftMinimum; }

    void setUseSoftMaximum(bool use);
    bool isUsingSoftMaximum() const { return m_useSoftMaximum; }

    void    setDecimals(int32_t decimals);
    int32_t getDecimals() const { return m_decimals; }

    void setFieldWidth(int32_t fieldWidth) { m_valueEntry->setFixedWidth(fieldWidth); }

    void setValue(const QVariant& value) override;

protected:
    void onSliderPressed() override;
    void onSliderMoved(int value) override;
    void onSliderReleased() override;
    void onSpinBoxEditingFinished() override;

private:
    void setupUI();

    ValueLineEdit<T>*           m_valueEntry;
    AttributeEditorSliderClass* m_slider;
    T                           m_minimum;
    T                           m_softMinimum;
    T                           m_maximum;
    T                           m_softMaximum;
    bool                        m_useSoftMinimum = false;
    bool                        m_useSoftMaximum = false;
    T                           m_value;
    T                           m_sliderBeginValue;
    int32_t                     m_decimals = std::is_integral_v<T> ? 0 : 3;
    double                      m_sliderFloatLimitWithDecimals = INT_MAX * std::pow(0.1, m_decimals);
    double m_sliderValueConversionRatio = std::is_integral_v<T> ? 1.0 : std::pow(10.0, m_decimals - 1);
};

} // namespace TINKERUSD_NS