#pragma once

#include <QUndoStack>

namespace TINKERUSD_NS
{

// UndoManager
class UndoManager 
{
public:
    static QUndoStack* undoStack();

    static QAction* createUndoAction(QObject* parent, const QString& prefix = "Undo");
    static QAction* createRedoAction(QObject* parent, const QString& prefix = "Redo");
    static void displayUndoStackInfo();

private:
    static QUndoStack* s_undoStack;
};

// UndoCommand
class UndoCommand : public QUndoCommand 
{
public:
    UndoCommand(const QString& commandName);
};


} // namespace TINKERUSD_NS