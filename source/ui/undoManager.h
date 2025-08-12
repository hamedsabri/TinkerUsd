#pragma once

#include <QUndoStack>
#include <QUndoCommand>
#include <QAction>

namespace TINKERUSD_NS
{

// Singleton class to manage QUndoStack.
class UndoManager
{
public:
    static UndoManager& instance();

    QUndoStack* undoStack();

    QAction* createUndoAction(QObject* parent, const QString& prefix = "Undo");
    QAction* createRedoAction(QObject* parent, const QString& prefix = "Redo");
    void displayUndoStackInfo();

    // TODO: implement command composition

private:
    UndoManager() = default;
    QUndoStack m_undoStack;
};

class UndoCommand : public QUndoCommand
{
public:
    UndoCommand(const QString& commandName);
};

} // namespace TINKERUSD_NS
