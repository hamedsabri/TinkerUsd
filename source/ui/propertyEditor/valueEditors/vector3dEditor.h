#pragma once

#include "abstractPropertyEditor.h"
#include "customWidgets/valueEntryLineEdit.h"

#include <QtGui/QVector3D>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>

namespace TINKERUSD_NS
{

class AbstractVector3DWidget : public QWidget
{
    Q_OBJECT
public:
    AbstractVector3DWidget(QWidget* parent = nullptr)
        : QWidget(parent)
    {
    }
    virtual ~AbstractVector3DWidget() = default;

    virtual QVector3D getValue() const = 0;
    virtual void      setValue(const QVector3D& vec) = 0;

signals:
    void commitData();

public slots:
    void onValueEntryChanged() { emit commitData(); }
};

template <typename T> class Vector3DEditorWidget : public AbstractVector3DWidget
{
public:
    Vector3DEditorWidget(QWidget* parent = nullptr);
    virtual ~Vector3DEditorWidget() = default;

    QVector3D getValue() const override;
    void      setValue(const QVector3D& value) override;

    void setRange(const QPair<T, T>& range);

private:
    void addComponentToLayout(
        QHBoxLayout*        layout,
        const QString&      labelText,
        ValueEntryLineEdit* lineEdit,
        const QColor&       backgroundColor);

    ValueEntryLineEdit* m_xLineEdit;
    ValueEntryLineEdit* m_yLineEdit;
    ValueEntryLineEdit* m_zLineEdit;
};

template <typename T> struct Vector3dData
{
    QPair<T, T> m_range;
    QVector3D   m_defaultValue;

    Vector3dData()
        : m_defaultValue(QVector3D())
    {
    }
    Vector3dData(const QPair<T, T>& range, const QVector3D& defaultValue)
        : m_range(range)
        , m_defaultValue(defaultValue)
    {
    }
};

Q_DECLARE_METATYPE(Vector3dData<float>)
Q_DECLARE_METATYPE(Vector3dData<double>)

template <typename T> class Vector3DEditor : public AbstractPropertyEditor
{
public:
    Vector3DEditor(
        const QString&         name,
        const Vector3dData<T>& currentValue,
        const QString&         tooltip = QString());
    virtual ~Vector3DEditor() = default;

    PXR_NS::VtValue toVtValue(const QVariant& value) const override;
    QVariant        fromVtValue(const PXR_NS::VtValue& value) const override;

    QWidget* createEditor(QWidget* parent) const override;
    void     setEditorData(QWidget* editor, const QVariant& data) const override;
    QVariant editorData(QWidget* editor) const override;

private:
    QPair<T, T> m_range;
};

using Vector3dDataFloat = Vector3dData<float>;
using Vector3dDataDouble = Vector3dData<double>;

using Vector3DEditorFloat = Vector3DEditor<float>;
using Vector3DEditorDouble = Vector3DEditor<double>;

} // namespace TINKERUSD_NS
