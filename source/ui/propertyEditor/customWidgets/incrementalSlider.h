#pragma once

#include <QFrame>
#include <QSlider>
#include <QWidget>
#include <QWheelEvent>
#include <QMouseEvent>

namespace TINKERUSD_NS
{

class DoubleValueLineEdit;
class IntValueLineEdit;

class AttributeEditorSlider : public QSlider
{
public:
    AttributeEditorSlider(QWidget* parent)
        : QSlider(parent)
    {
        setFocusPolicy(Qt::StrongFocus);
    }

protected:
    virtual void wheelEvent(QWheelEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
};

class FloatSliderGroup : public QFrame
{
    Q_OBJECT
public:
    FloatSliderGroup(float min, float max, float value, QWidget* parent = nullptr);
    FloatSliderGroup(QWidget* parent = nullptr);

    DoubleValueLineEdit* valueEntry() const { return m_valueEntry; }
    AttributeEditorSlider* slider() const {return m_slider; }

    void setMinimum(float min);
    float getMinimum() const { return m_minimum; }
    
    void setMaximum(float max);
    float getMaximum() const { return m_maximum; }
    
    void setRange(float min, float max);

    void setSoftMinimum(float min);
    float getSoftMinimum() const { return m_softMinimum; }

    void setSoftMaximum(float max);
    float getSoftMaximum() const { return m_softMaximum; }

    void setUseSoftMinimum(bool use);
    bool isUsingSoftMinimum() const { return m_useSoftMinimum; }

    void setUseSoftMaximum(bool use);
    bool isUsingSoftMaximum() const { return m_useSoftMaximum; }

    void setDecimals(int32_t decimals);
    int32_t getDecimals() const { return m_decimals; }

    void setFieldWidth(int32_t fieldWidth);

signals:
    void sliderPressed();
    void sliderMoved(float value);
    void sliderReleased();

    void valueUpdated(float value);
    void contextMenuRequested(QContextMenuEvent* event);

public slots:
    void setValue(float value);

private slots:
    void onSliderMoved(int value);
    void onSpinBoxEditingFinished();
    void onSliderPressed();
    void onSliderReleased();

private:
    void setupUI();

    DoubleValueLineEdit* m_valueEntry;
    AttributeEditorSlider* m_slider;

    float m_minimum;
    float m_softMinimum;
    float m_maximum;
    float m_softMaximum;
    bool m_useSoftMinimum;
    bool m_useSoftMaximum;
    float m_value;
    float m_sliderBeginValue;
    int32_t m_decimals;
    float m_sliderFloatLimitWithDecimals;
    float m_sliderValueConversionRatio;
};

class IntSliderGroup : public QFrame
{
    Q_OBJECT
public:
    IntSliderGroup(int min, int max, int value, QWidget* parent = nullptr);
    IntSliderGroup(QWidget* parent = nullptr);

    IntValueLineEdit* valueEntry() const { return m_valueEntry; }
    AttributeEditorSlider* slider() const {return m_slider; }

    void setMinimum(int min);
    int getMinimum() const { return m_minimum; }

    void setMaximum(int max);
    int getMaximum() const { return m_maximum; }

    void setRange(int min, int max);

    void setSoftMinimum(int min);
    int getSoftMinimum() const { return m_softMinimum; }

    void setSoftMaximum(int max);
    int getSoftMaximum() const { return m_softMaximum; }

    void setUseSoftMinimum(bool use);
    void setUseSoftMaximum(bool use);

    void setFieldWidth(int32_t fieldWidth);

signals:
    void sliderPressed();
    void sliderMoved(int value);
    void sliderReleased();

    void valueUpdated(int value); // Emitted when dragging is done or a value has been entered
    void contextMenuRequested(QContextMenuEvent* event);

public slots:
    void setValue(int value);

private slots:
    void onSliderPressed();
    void onSliderMoved(int value);
    void onSliderReleased();

    void onSpinBoxEditingFinished();

private:
    void setupUI();

private:
    IntValueLineEdit* m_valueEntry;
    AttributeEditorSlider* m_slider;
    int m_minimum;
    int m_softMinimum;
    int m_maximum;
    int m_softMaximum;
    bool m_useSoftMinimum;
    bool m_useSoftMaximum;
    int m_value;
    int m_sliderBeginValue;
};

} // namespace TINKERUSD_NS