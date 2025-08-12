#include "undoManager.h"
#include <QDebug>

namespace TINKERUSD_NS
{

UndoManager& UndoManager::instance()
{
    static UndoManager instance;
    return instance;
}

QUndoStack* UndoManager::undoStack()
{
    return &m_undoStack;
}

QAction* UndoManager::createUndoAction(QObject* parent, const QString& prefix)
{
    return m_undoStack.createUndoAction(parent, prefix);
}

QAction* UndoManager::createRedoAction(QObject* parent, const QString& prefix)
{
    return m_undoStack.createRedoAction(parent, prefix);
}

void UndoManager::displayUndoStackInfo()
{
    qDebug() << "---- Undo Stack ----";
    for (int i = 0; i < m_undoStack.count(); ++i)
    {
        const QUndoCommand* cmd = m_undoStack.command(i);
        QString label = cmd ? cmd->text() : "null";
        QString marker = (i == m_undoStack.index()) ? " <- current" : "";
        qDebug() << QString("#%1: %2%3").arg(i).arg(label, marker);
    }
    qDebug() << "--------------------";
}

UndoCommand::UndoCommand(const QString& commandName)
    : QUndoCommand(QString("'%1'").arg(commandName))
{
    UndoManager::instance().undoStack()->push(this);
}

} // namespace TINKERUSD_NS
