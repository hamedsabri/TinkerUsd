#pragma once

#include <QtCore/QSignalMapper>
#include <QtWidgets/QStyledItemDelegate>

namespace TINKERUSD_NS
{

/*
 * @class PropertyDelegate
 * @brief The class is used to display and edit data items from a model.
 */
class PropertyDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    PropertyDelegate(QObject* parent = nullptr);

    virtual ~PropertyDelegate() = default;

    // returns the editor to be used for editing the data item with the given index
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index)
        const override;

    // sets the contents of the given editor to the data for the item at the given index
    void setEditorData(QWidget* editor, const QModelIndex& index) const override;

    // sets the data for the item at the given index in the model to the contents of the given
    // editor.
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;

    // updates the geometry of the editor for the item with the given index
    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index)
        const override;

    // provides custom rendering. use the painter and style option to render the item specified by
    // the item index.
    void
    paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    // provides custom drawing.
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;

private slots:
    void commitAndCloseEditor(QObject* object);

private:
    // This is used for managing the commitData signal from custom editors
    // The QSignalMapper allows mapping multiple signals to a single slot,
    // which can then distinguish the sender.
    QSignalMapper* m_signalMapper;
};

} // namespace TINKERUSD_NS