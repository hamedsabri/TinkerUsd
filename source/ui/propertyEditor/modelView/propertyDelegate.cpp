#include "propertyDelegate.h"

#include "common/utils.h"
#include "valueEditors/abstractPropertyEditor.h"
#include "valueEditors/booleanEditor.h"
#include "valueEditors/enumEditor.h"

#include <QtWidgets/QApplication>

namespace TINKERUSD_NS
{

PropertyDelegate::PropertyDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
    , m_signalMapper(new QSignalMapper(this))
{
    connect(m_signalMapper, &QSignalMapper::mappedObject, this, &PropertyDelegate::commitAndCloseEditor);
}

QWidget* PropertyDelegate::createEditor(
    QWidget*                    parent,
    const QStyleOptionViewItem& option,
    const QModelIndex&          index) const
{
    auto abstractPropEditor = TINKERUSD_NS::Utils::getEditorFromIndex(index);

    // do not create an editor for group items
    if (!abstractPropEditor)
    {
        return nullptr;
    }

    QWidget* editor = abstractPropEditor->createEditor(parent);

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

void PropertyDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    auto abstractPropEditor = TINKERUSD_NS::Utils::getEditorFromIndex(index);

    if (abstractPropEditor)
    {
        abstractPropEditor->setEditorData(editor, index.model()->data(index, Qt::EditRole));
    }
    else
    {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

void PropertyDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index)
    const
{
    auto abstractPropEditor = TINKERUSD_NS::Utils::getEditorFromIndex(index);

    if (abstractPropEditor)
    {
        // keep model and editor synchronized here. this set the data in the model to the
        // value retrieved from the editor widget.
        model->setData(index, abstractPropEditor->editorData(editor), Qt::EditRole);
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

void PropertyDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index)
    const
{
    auto abstractPropEditor = TINKERUSD_NS::Utils::getEditorFromIndex(index);

    if (abstractPropEditor && index.column() == 1
        && abstractPropEditor->paint(painter, option, index.data(Qt::DisplayRole)))
    {
        return;
    }

    // remove the focus rectangle
    QStyleOptionViewItem optionCopy(option);
    optionCopy.state &= ~QStyle::State_HasFocus;

    QStyledItemDelegate::paint(painter, optionCopy, index);
}

QSize PropertyDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    // adjust the height of each row
    QSize size = QStyledItemDelegate::sizeHint(option, index);
    size.setHeight(size.height() + 12);

    return size;
}

void PropertyDelegate::commitAndCloseEditor(QObject* object)
{
    QWidget* editor = qobject_cast<QWidget*>(object);
    if (editor && editor->isVisible())
    {
        emit commitData(editor);
        emit closeEditor(editor);
    }
}

} // namespace TINKERUSD_NS