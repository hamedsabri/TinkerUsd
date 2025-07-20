#pragma once

#include "abstractPropertyEditor.h"
#include "customWidgets/valueEntryLineEdit.h"

#include <QtGui/QVector2D>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QWidget>

namespace TINKERUSD_NS
{

// Abstract base class for 2D vector widget components
/**
 * @class AbstractVector2DWidget
 * @brief A base class for for 2D vector widget components.
 */
class AbstractVector2DWidget : public QWidget
{
    Q_OBJECT
public:
    AbstractVector2DWidget(QWidget *parent = nullptr) : QWidget(parent) {}
    virtual ~AbstractVector2DWidget() = default;

    virtual QVector2D getValue() const = 0;
    virtual void setValue(const QVector2D &vec) = 0;

signals:
    // signal emitted when the data needs to be committed.
    void commitData();

public slots:
    void onValueEntryChanged() { emit commitData(); }
};

/**
 * @class Vector2DEditorWidget
 * @brief A template-based widget for a Vector2D.
 * 
 */
template<typename T>
class Vector2DEditorWidget : public AbstractVector2DWidget
{
public:
    Vector2DEditorWidget(QWidget *parent = nullptr);
    virtual ~Vector2DEditorWidget() = default;

    QVector2D getValue() const override;
    void setValue(const QVector2D& value) override;

    void setRange(const QPair<T, T>& range);

private:
    void addComponentToLayout(QHBoxLayout* layout, 
                              const QString& labelText, 
                              DoubleValueLineEdit* lineEdit, 
                              const QColor& backgroundColor);

    DoubleValueLineEdit* m_xLineEdit;
    DoubleValueLineEdit* m_yLineEdit;
};

/**
 * @struct Vector2dData
 * @brief A structure to hold data for 2D vectors
 *
 */
template<typename T>
struct Vector2dData
{
    QPair<T, T> m_range;
    QVector2D m_defaultValue;

    Vector2dData() : m_defaultValue(QVector2D()) {}
    Vector2dData(const QPair<T, T>& range, const QVector2D& defaultValue) : m_range(range), m_defaultValue(defaultValue) {}
};

Q_DECLARE_METATYPE(Vector2dData<float>)
Q_DECLARE_METATYPE(Vector2dData<double>)

/**
 * @class Vector2DEditor
 * @brief A 2D vector editor.
 * 
 */
template<typename T>
class Vector2DEditor : public AbstractPropertyEditor
{
public:
    Vector2DEditor(const QString &name, const Vector2dData<T>& currentValue, const QString &tooltip = QString());
    virtual ~Vector2DEditor() = default;

    // convert QVariant to Pixar's VtValue
    PXR_NS::VtValue toVtValue(const QVariant& value) const override;

    // convert Pixar's VtValue back to QVariant
    QVariant fromVtValue(const PXR_NS::VtValue& value) const override;

    // create the editor widget for the property
    QWidget* createEditor(QWidget* parent) const override;

    // set the data for the editor widget
    void setEditorData(QWidget* editor, const QVariant& data) const override;

    // get the current data from the editor widget
    QVariant editorData(QWidget* editor) const override;

private:
    QPair<T, T> m_range;
};

using Vector2dDataFloat    = Vector2dData<float>;
using Vector2dDataDouble   = Vector2dData<double>;

using Vector2DEditorFloat  = Vector2DEditor<float>;
using Vector2DEditorDouble = Vector2DEditor<double>;

} // namespace TINKERUSD_NS