#include "propertyDelegate.h"

#include "common/utils.h"
#include "valueEditors/abstractPropertyEditor.h"
#include "valueEditors/booleanEditor.h"

#include <QAbstractProxyModel>
#include <QApplication>
#include <QDebug>
#include <QStandardItem>
#include <QStandardItemModel>

namespace TINKERUSD_NS
{

PropertyDelegate::PropertyDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
    , m_signalMapper(new QSignalMapper(this))
{
    connect(
        m_signalMapper,
        &QSignalMapper::mappedObject,
        this,
        &PropertyDelegate::commitAndCloseEditor);
}

QWidget* PropertyDelegate::createEditor(
    QWidget*                    parent,
    const QStyleOptionViewItem& option,
    const QModelIndex&          index) const
{
    auto propertyEditor = TINKERUSD_NS::Utils::getEditorFromIndex(index);

    // do not create an editor for group items
    if (!propertyEditor)
    {
        return nullptr;
    }

    QWidget* editor = propertyEditor->createEditor(parent);

    // if the editor widget has a commitData signal, it connects the signal to the QSignalMapper.
    // when QSignalMapper emits the mapped signal, the commitData slot in PropertyDelegate is
    // called. This slot emits the commitData and closeEditor signals.
    if (editor && editor->metaObject()->indexOfSignal("commitData()") >= 0)
    {
        // HS : I had to use old syntax to ensure this works with custom editor widgets.
        // This is because QWidget itself does not have the commitData signal,
        // so using the new syntax directly would not work in this case.
        connect(editor, SIGNAL(commitData()), m_signalMapper, SLOT(map()));
        m_signalMapper->setMapping(editor, editor);
        return editor;
    }

    return QStyledItemDelegate::createEditor(parent, option, index);
}

void PropertyDelegate::commitAndCloseEditor(QObject* object)
{
    QWidget* editor = qobject_cast<QWidget*>(object);

    if (editor && editor->isVisible() && editor->parentWidget())
    {
        // qDebug() << "Emitting commitData for editor:" << editor;
        emit commitData(editor);
        emit closeEditor(editor);
    }
}

void PropertyDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    auto propertyEditor = TINKERUSD_NS::Utils::getEditorFromIndex(index);
    if (propertyEditor)
    {
        propertyEditor->setEditorData(editor, index.model()->data(index, Qt::EditRole));
    }
    else
    {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

void PropertyDelegate::setModelData(
    QWidget*            editor,
    QAbstractItemModel* model,
    const QModelIndex&  index) const
{
    auto propertyEditor = TINKERUSD_NS::Utils::getEditorFromIndex(index);
    if (propertyEditor)
    {
        model->setData(index, propertyEditor->editorData(editor), Qt::EditRole);
    }
    else
    {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}

void PropertyDelegate::updateEditorGeometry(
    QWidget*                    editor,
    const QStyleOptionViewItem& option,
    const QModelIndex&          index) const
{
    editor->setGeometry(option.rect);
}

void PropertyDelegate::paint(
    QPainter*                   painter,
    const QStyleOptionViewItem& option,
    const QModelIndex&          index) const
{
    // // Get the source model if we're using a proxy model
    // const QAbstractItemModel* model = index.model();
    // QModelIndex sourceIndex = index;

    // // Traverse through proxy models to get to the source model
    // while (const QAbstractProxyModel* proxyModel = qobject_cast<const
    // QAbstractProxyModel*>(model)) {
    //     sourceIndex = proxyModel->mapToSource(sourceIndex);
    //     model = proxyModel->sourceModel();
    // }

    // // Now we should have the source model, which should be a QStandardItemModel
    // const QStandardItemModel* standardModel = qobject_cast<const QStandardItemModel*>(model);

    // if (standardModel) {
    //     QStandardItem* item = standardModel->itemFromIndex(sourceIndex.sibling(sourceIndex.row(),
    //     0));

    //     // Check if it's a group item (has children)
    //     if (item && item->hasChildren()) {
    //         // Draw the background with a slight reduction in height
    //         painter->save();
    //         QRect backgroundRect = option.rect;
    //         backgroundRect.setHeight(backgroundRect.height() - 2); // Reduce height by 2 pixels
    //         painter->fillRect(backgroundRect, QColor(220, 220, 220)); // Light gray color
    //         painter->restore();

    //         // Draw the text for the first column
    //         if (index.column() == 0) {
    //             // Remove the focus rectangle
    //             QStyleOptionViewItem optionCopy(option);
    //             optionCopy.state &= ~QStyle::State_HasFocus;
    //             QStyledItemDelegate::paint(painter, optionCopy, index);
    //         }
    //         // For the second column, we don't draw anything
    //         return;
    //     }
    // }

    // Existing logic for painting normal propertyEditor items
    auto propertyEditor = TINKERUSD_NS::Utils::getEditorFromIndex(index);
    if (propertyEditor && index.column() == 1
        && propertyEditor->paint(painter, option, index.data(Qt::DisplayRole)))
    {
        return;
    }

    // Remove the focus rectangle
    QStyleOptionViewItem optionCopy(option);
    if (optionCopy.state & QStyle::State_HasFocus)
    {
        optionCopy.state &= ~QStyle::State_HasFocus;
    }
    QStyledItemDelegate::paint(painter, optionCopy, index);
}

QSize PropertyDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QSize size = QStyledItemDelegate::sizeHint(option, index);

    // Get the source index and model
    QModelIndex               sourceIndex = index;
    const QAbstractItemModel* model = index.model();

    // Traverse through proxy models to get to the source model
    while (const QAbstractProxyModel* proxyModel = qobject_cast<const QAbstractProxyModel*>(model))
    {
        sourceIndex = proxyModel->mapToSource(sourceIndex);
        model = proxyModel->sourceModel();
    }

    // Now we should have the source model, which should be a QStandardItemModel
    const QStandardItemModel* standardModel = qobject_cast<const QStandardItemModel*>(model);

    if (standardModel)
    {
        QStandardItem* item
            = standardModel->itemFromIndex(sourceIndex.sibling(sourceIndex.row(), 0));

        // Check if it's a group item (has children)
        if (item && item->hasChildren())
        {
            // Set a larger height for group items
            int groupExtraHeight = 4; // Extra spacing for group rows
            size.setHeight(size.height() + groupExtraHeight);
        }
        else
        {
            // For regular property items, use the original extra height
            int extraHeight = 10; // Extra spacing for regular rows
            size.setHeight(size.height() + extraHeight);
        }
    }

    return size;
}

} // namespace TINKERUSD_NS