#pragma once

#include <QtGui/QWheelEvent>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QToolButton>

namespace TINKERUSD_NS
{

/**
 * @class NoWheelComboBox
 * @brief custom QComboBox with wheelevent disabled.
 *
 */
class NoWheelComboBox : public QComboBox
{
    Q_OBJECT
public:
    NoWheelComboBox(QWidget* parent = nullptr) : QComboBox(parent) {}

protected:
    void wheelEvent(QWheelEvent* e) override {
        e->ignore();
    }
};

/**
 * @struct EnumData
 * @brief A structure to hold enum data
 *
 */
struct EnumData 
{
    QStringList options;
    QString currentText;

    EnumData() : currentText("") {}
    EnumData(const QStringList& opts, const QString& ctext) : options(opts), currentText(ctext) {}
};
Q_DECLARE_METATYPE(EnumData)

/**
 * @class PropertyComboBoxWidget
 * @brief ComboBox Widget
 *
 */
class PropertyComboBoxWidget : public QWidget
{
    Q_OBJECT
public :
    PropertyComboBoxWidget(QWidget* parent = nullptr);

    NoWheelComboBox* comboBox() const;

signals:
    // signal emitted when the data needs to be committed.
    void commitData();

private:
    NoWheelComboBox* m_comboBox;
};

} // namespace TINKERUSD_NS