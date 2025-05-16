#pragma once

#include <QWheelEvent>
#include <QtWidgets/QComboBox>

namespace TINKERUSD_NS
{

class NoWheelComboBox : public QComboBox
{
    Q_OBJECT
public:
    NoWheelComboBox(QWidget* parent = nullptr)
        : QComboBox(parent)
    {
    }

protected:
    void wheelEvent(QWheelEvent* e) override
    {
        // Ignore the wheel event
        e->ignore();
    }
};

class PropertyComboBoxWidget : public QWidget
{
    Q_OBJECT
public:
    PropertyComboBoxWidget(QWidget* parent = nullptr);

    NoWheelComboBox* comboBox() const;

signals:
    void commitData();

private:
    NoWheelComboBox* m_comboBox;
};

struct EnumData
{
    QStringList options;
    QString     currentText;

    EnumData()
        : currentText("")
    {
    }
    EnumData(const QStringList& opts, const QString& ctext)
        : options(opts)
        , currentText(ctext)
    {
    }
};
Q_DECLARE_METATYPE(EnumData)

} // namespace TINKERUSD_NS