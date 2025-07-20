#pragma once

#include "abstractPropertyEditor.h"
#include "customWidgets/valueEntryLineEdit.h"

#include <QtGui/QVector3D>
#include <QtWidgets/QWidget>
#include <QtWidgets/QHBoxLayout>

namespace TINKERUSD_NS
{
// Abstract base class for 3D vector widget components
/**
 * @class AbstractVector3DWidget
 * @brief A base class for for 3D vector widget components.
 */
class AbstractVector3DWidget : public QWidget
{
    Q_OBJECT
public:
    
    AbstractVector3DWidget(QWidget *parent = nullptr) : QWidget(parent) {}

    virtual ~AbstractVector3DWidget() = default;

    // get the current 3D vector value
    virtual QVector3D getValue() const = 0;

    // set a new 3D vector value
    virtual void setValue(const QVector3D &vec) = 0;

signals:
    // signal emitted when the data should be committed.
    void commitData();

public slots:
    void onValueEntryChanged() { emit commitData(); }
};

/**
 * @class Vector3DEditorWidget
 * @brief A template-based widget for a Vector3D.
 * 
 */
template<typename T>
class Vector3DEditorWidget : public AbstractVector3DWidget
{
public:
    
    Vector3DEditorWidget(QWidget *parent = nullptr);

    virtual ~Vector3DEditorWidget() = default;

    // get the current 3D vector value
    QVector3D getValue() const override;

    // set a new 3D vector value
    void setValue(const QVector3D& value) override;

    // sets the range for each component of the vector
    void setRange(const QPair<T, T>& range);

private:
    void addComponentToLayout(QHBoxLayout* layout, 
                              const QString& labelText, 
                              DoubleValueLineEdit* lineEdit, 
                              const QColor& backgroundColor);

    DoubleValueLineEdit* m_xLineEdit;
    DoubleValueLineEdit* m_yLineEdit;
    DoubleValueLineEdit* m_zLineEdit;
};

/**
 * @struct Vector3dData
 * @brief A structure to hold data for 3D vectors
 *
 */
template<typename T>
struct Vector3dData
{
    QPair<T, T> m_range;
    QVector3D m_defaultValue;

    Vector3dData() : m_defaultValue(QVector3D()) {}
    Vector3dData(const QPair<T, T>& range, const QVector3D& defaultValue) : m_range(range), m_defaultValue(defaultValue) {}
};

Q_DECLARE_METATYPE(Vector3dData<float>)
Q_DECLARE_METATYPE(Vector3dData<double>)

/**
 * @class Vector3DEditor
 * @brief A 3D vector editor.
 * 
 */
template<typename T>
class Vector3DEditor : public AbstractPropertyEditor
{
public:
    Vector3DEditor(const QString& name, 
                   const Vector3dData<T>& currentValue, 
                   const QString& tooltip = QString());
    
    virtual ~Vector3DEditor() = default;

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

using Vector3dDataFloat    = Vector3dData<float>;
using Vector3dDataDouble   = Vector3dData<double>;

using Vector3DEditorFloat  = Vector3DEditor<float>;
using Vector3DEditorDouble = Vector3DEditor<double>;

} // namespace TINKERUSD_NS