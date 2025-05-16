#include "mainMenuBar.h"

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
    QMenu* fileMenu = addMenu("File");

    QAction* newStageAction = new QAction("New Stage", this);
    QAction* openStageAction = new QAction("Open Stage", this);
    QAction* quitAction = new QAction("Quit", this);

    m_panelsMenu = addMenu("Panels");

    fileMenu->addAction(newStageAction);
    fileMenu->addAction(openStageAction);
    fileMenu->addSeparator();
    fileMenu->addAction(quitAction);

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
}

} // namespace TINKERUSD_NS