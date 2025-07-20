#include "mainMenuBar.h"
#include "undomanager.h"

#include <QAction>
#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>

namespace TINKERUSD_NS
{

MainMenuBar::MainMenuBar(QWidget* parent)
    : QMenuBar(parent)
{
    setupMenus();
}

void MainMenuBar::setupMenus()
{
    // file
    QMenu* fileMenu = addMenu("File");

    QAction* newStageAction = new QAction("New Stage", this);
    QAction* openStageAction = new QAction("Open Stage", this);
    QAction* quitAction = new QAction("Quit", this);

    fileMenu->addAction(newStageAction);
    fileMenu->addAction(openStageAction);
    fileMenu->addSeparator();
    fileMenu->addAction(quitAction);

    // edit
    QMenu* editMenu = addMenu("Edit");
    QAction* undoAction = UndoManager::createUndoAction(this);
    QAction* redoAction = UndoManager::createRedoAction(this);
    QAction* clearUndoAction = new QAction("Clear Undo History", this);
    QAction* debugUndoStackAction = new QAction("Debug UndoStack", this);
    undoAction->setShortcut(QKeySequence::Undo);
    redoAction->setShortcut(QKeySequence::Redo);
    editMenu->addAction(undoAction);
    editMenu->addAction(redoAction);
    editMenu->addSeparator();
    editMenu->addAction(clearUndoAction);
    editMenu->addAction(debugUndoStackAction);

    // panels
    m_panelsMenu = addMenu("Panels");

    // camera
    QMenu* cameraMenu = addMenu("Camera");
    QAction* frameSelectedAction = new QAction("Frame Selected", this);
    frameSelectedAction->setShortcut(QKeySequence(Qt::Key_F));
    QAction* resetAction = new QAction("Reset", this);
    cameraMenu->addAction(frameSelectedAction);
    cameraMenu->addAction(resetAction);

    // help
    QMenu* helpMenu = addMenu("Help");
    QAction* about = new QAction("About", this);
    helpMenu->addAction(about);

    connect(newStageAction, &QAction::triggered, this, &MainMenuBar::requestNewStage);
    connect(openStageAction, &QAction::triggered, [this]() {
        QString file = QFileDialog::getOpenFileName(
            this, "Open USD Stage", "", "USD Files (*.usd *.usda *.usdc *.usdz)");
        if (!file.isEmpty())
            emit requestOpenStage(file);
    });
    connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);

    connect(about, &QAction::triggered, this, [this]() {
        QMessageBox::about(
            this,
            "About",
            "TinkerUsd\n"
            "Version 0.1.0\n\n"
            "Author: Hamed Sabri\n"
        );
    });

    connect(frameSelectedAction, &QAction::triggered,this, &MainMenuBar::camFrameSelectSignal);
    connect(resetAction, &QAction::triggered,this, &MainMenuBar::camResetSignal);

    connect(clearUndoAction, &QAction::triggered, this, []() {
        UndoManager::undoStack()->clear();
    });

    connect(debugUndoStackAction, &QAction::triggered, this, []() {
        UndoManager::displayUndoStackInfo();
    });
}

} // namespace TINKERUSD_NS