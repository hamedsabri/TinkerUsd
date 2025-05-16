#pragma once

#include <QtCore/QSignalMapper>
#include <QtWidgets/QStyledItemDelegate>

namespace TINKERUSD_NS
{

class PropertyDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    PropertyDelegate(QObject* parent = nullptr);

    QWidget* createEditor(
        QWidget*                    parent,
        const QStyleOptionViewItem& option,
        const QModelIndex&          index) const override;
    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index)
        const override;
    void updateEditorGeometry(
        QWidget*                    editor,
        const QStyleOptionViewItem& option,
        const QModelIndex&          index) const override;
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index)
        const override;
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;

private slots:
    void commitAndCloseEditor(QObject* object);

private:
    // This is used for managing the commitData signal from custom editors
    // The QSignalMapper allows you to map multiple signals to a single slot,
    // which can then distinguish the sender.
    QSignalMapper* m_signalMapper;
};

} // namespace TINKERUSD_NS
