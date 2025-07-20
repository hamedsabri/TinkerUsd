#include "undoManager.h"
#include <QDebug>
#include <QUndoStack>
#include <QUndoCommand>

namespace TINKERUSD_NS
{

QUndoStack* UndoManager::s_undoStack = new QUndoStack();

QUndoStack* UndoManager::undoStack() 
{
    return s_undoStack;
}

QAction* UndoManager::createUndoAction(QObject* parent, const QString& prefix)
{
    return s_undoStack->createUndoAction(parent, prefix);
}

QAction* UndoManager::createRedoAction(QObject* parent, const QString& prefix)
{
    return s_undoStack->createRedoAction(parent, prefix);
}

void UndoManager::displayUndoStackInfo()
{
    qDebug() << "---- Undo Stack ----";
    for (auto i = 0; i < UndoManager::undoStack()->count(); ++i) {
        const QUndoCommand* cmd = UndoManager::undoStack()->command(i);
        QString label = cmd ? cmd->text() : "null";
        QString marker = (i == UndoManager::undoStack()->index()) ? " <- current" : "";
        qDebug() << QString("#%1: %2%3").arg(i).arg(label, marker);
    }
    qDebug() << "--------------------";
}

UndoCommand::UndoCommand(const QString& commandName)
    : QUndoCommand(QString("'%1'").arg(commandName))
{
    UndoManager::undoStack()->push(this);
}

} // namespace TINKERUSD_NS