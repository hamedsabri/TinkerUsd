#include "mainWindow.h"

#include "DockManager.h"

#include "composition/compositionInspectorWidget.h"
#include "core/globalSelection.h"
#include "core/usdDocument.h"
#include "mainMenuBar.h"
#include "outliner/outlinerWidget.h"
#include "propertyEditor/propertyWidget.h"
#include "scriptEditor/scriptEditor.h"
#include "viewportOpenGLWidget.h"

#include <QComboBox>
#include <QDir>
#include <QFileInfo>
#include <QLabel>
#include <QStatusBar>
#include <QToolBar>

    
namespace TINKERUSD_NS
{

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("TinkerUsd: untitled");

    auto usdDocument = new UsdDocument(this);
    auto mainMenuBar = new MainMenuBar(this);
    auto statusBar = new QStatusBar(this);
    auto dockManager = new ads::CDockManager(this);
    auto scriptEditorWidget = new ScriptEditor(this);
    auto viewportGLWidget = new ViewportOpenGLWidget(usdDocument);
    auto compInspectorWidget = new CompositionInspectorWidget(usdDocument);
    auto outlinerWidget = new OutlinerWidget(usdDocument);
    auto propertyWidget = new PropertyWidget(usdDocument);
    auto aovComboBox = new QComboBox();
    auto shadingComboBox = new QComboBox();

    // ads styles heet
    QFile StyleSheetFile(":/drak_ads.css");
    if (StyleSheetFile.open(QIODevice::ReadOnly))
    {
        QTextStream StyleSheetStream(&StyleSheetFile);
        auto        Stylesheet = StyleSheetStream.readAll();
        StyleSheetFile.close();
        dockManager->setStyleSheet(Stylesheet);
    }

    // menubar
    setMenuBar(mainMenuBar);

    // statusbar
    setStatusBar(statusBar);

    // openGlViewport dockWidget
    ads::CDockWidget* openGlViewportDockWidget = new ads::CDockWidget("Viewport");
    openGlViewportDockWidget->setWidget(viewportGLWidget);

    openGlViewportDockWidget->setMinimumSizeHintMode(
        ads::CDockWidget::MinimumSizeHintFromDockWidget);
    openGlViewportDockWidget->setMinimumSize(600, 150);

    openGlViewportDockWidget->setFeature(ads::CDockWidget::DockWidgetClosable, false);
    openGlViewportDockWidget->setFeature(ads::CDockWidget::DockWidgetMovable, false);
    openGlViewportDockWidget->setFeature(ads::CDockWidget::DockWidgetFloatable, false);

    dockManager->addDockWidget(ads::CenterDockWidgetArea, openGlViewportDockWidget);
    mainMenuBar->getPanelsMenu()->addAction(openGlViewportDockWidget->toggleViewAction());

    auto toolBar = openGlViewportDockWidget->createDefaultToolBar();
    aovComboBox->setToolTip("Select AOV");
    toolBar->addWidget(aovComboBox);
    QObject::connect(
        aovComboBox,
        &QComboBox::currentTextChanged,
        this,
        [viewportGLWidget](const QString& aov) {
            viewportGLWidget->setRendererAov(aov.toStdString());
        });

    toolBar->addSeparator();

    shadingComboBox->setToolTip("Select Shading");
    shadingComboBox->addItem(
        "Smooth", static_cast<int>(ViewportOpenGLWidget::ShadingMode::SHADEDSMOOTH));
    shadingComboBox->addItem(
        "Points", static_cast<int>(ViewportOpenGLWidget::ShadingMode::POINTS));
    shadingComboBox->addItem(
        "Wireframe", static_cast<int>(ViewportOpenGLWidget::ShadingMode::WIREFRAME));
    shadingComboBox->addItem(
        "Wireframe On Surface",
        static_cast<int>(ViewportOpenGLWidget::ShadingMode::WIREFRAME_ON_SURFACE));
    shadingComboBox->addItem(
        "Flat", static_cast<int>(ViewportOpenGLWidget::ShadingMode::SHADED_FLAT));

    shadingComboBox->setCurrentIndex(static_cast<int>(viewportGLWidget->shadingMode()));

    toolBar->addWidget(shadingComboBox);
    // Connect the currentIndexChanged signal to update the shading mode
    QObject::connect(
        shadingComboBox,
        QOverload<int>::of(&QComboBox::currentIndexChanged),
        this,
        [viewportGLWidget](int index) {
            auto mode = static_cast<ViewportOpenGLWidget::ShadingMode>(index);
            viewportGLWidget->setShadingMode(mode);
        });

    toolBar->addSeparator();

    // outliner
    ads::CDockWidget* outlinerDockWidget = new ads::CDockWidget("Outliner");
    outlinerDockWidget->setWidget(outlinerWidget);
    outlinerDockWidget->setMinimumSizeHintMode(ads::CDockWidget::MinimumSizeHintFromDockWidget);
    outlinerDockWidget->setMinimumSize(340, 150);

    auto dockAreaWidgetOutliner
        = dockManager->addDockWidget(ads::LeftDockWidgetArea, outlinerDockWidget);
    mainMenuBar->getPanelsMenu()->addAction(outlinerDockWidget->toggleViewAction());

    // composition
    ads::CDockWidget* compositionDockWidget = new ads::CDockWidget("Inspectors");
    compositionDockWidget->setWidget(compInspectorWidget);
    compositionDockWidget->setMinimumSizeHintMode(ads::CDockWidget::MinimumSizeHintFromDockWidget);
    compositionDockWidget->setMinimumSize(340, 150);
    dockManager->addDockWidget(
        ads::BottomDockWidgetArea, compositionDockWidget, dockAreaWidgetOutliner);
    mainMenuBar->getPanelsMenu()->addAction(compositionDockWidget->toggleViewAction());

    // property editor
    ads::CDockWidget* dockWidgetProperty = new ads::CDockWidget("Properties");
    dockWidgetProperty->setWidget(propertyWidget);
    dockWidgetProperty->setMinimumSize(250, 280);
    auto dockAreaPropertyEditor
        = dockManager->addDockWidget(ads::RightDockWidgetArea, dockWidgetProperty);
    mainMenuBar->getPanelsMenu()->addAction(dockWidgetProperty->toggleViewAction());

    // script editor
    ads::CDockWidget* scriptEditorDockWidget = new ads::CDockWidget("Script Editor");
    scriptEditorDockWidget->setWidget(scriptEditorWidget);
    dockManager->addDockWidget(
        ads::BottomDockWidgetArea, scriptEditorDockWidget, dockAreaPropertyEditor);
    mainMenuBar->getPanelsMenu()->addAction(scriptEditorDockWidget->toggleViewAction());

    // connection signal/slots
    connect(
        mainMenuBar,
        &MainMenuBar::requestNewStage,
        usdDocument,
        &UsdDocument::createNewStageInMemory);
    connect(mainMenuBar, &MainMenuBar::requestOpenStage, usdDocument, &UsdDocument::openStage);

    connect(usdDocument, &UsdDocument::stageOpened, [this](const QString& filePath) {
        QString baseName = QFileInfo(filePath).fileName();
        QString title
            = QString("%1 - TinkerUsd: %2").arg(baseName, QDir::toNativeSeparators(filePath));
        setWindowTitle(title);

        GlobalSelection::instance().clearSelection();
    });

    connect(mainMenuBar,&MainMenuBar::camFrameSelectSignal,viewportGLWidget,&ViewportOpenGLWidget::frameSelected);

    connect(mainMenuBar,&MainMenuBar::camResetSignal,viewportGLWidget,&ViewportOpenGLWidget::reset);

    auto rendererTypeLabel = new QLabel();
    auto stageUpAxisLabel = new QLabel(QString("Up Axis: %1 ").arg(viewportGLWidget->upAxisDisplayName()));

    statusBar->addWidget(rendererTypeLabel);
    statusBar->addWidget(stageUpAxisLabel);

    connect(
        viewportGLWidget,
        &ViewportOpenGLWidget::rendererAvailable,
        this,
        [this, statusBar, viewportGLWidget, rendererTypeLabel, aovComboBox]() {
            
            rendererTypeLabel->setText(
                QString("Render: %1 ").arg(viewportGLWidget->rendererDisplayName()));

            // aov
            aovComboBox->clear();
            const auto  aovs = viewportGLWidget->getRendererAovs();
            QStringList qAovs;
            for (const auto& aov : aovs) {
                qAovs.append(QString::fromStdString(aov));
            }
            aovComboBox->addItems(qAovs);
            if (!aovs.empty()) {
                aovComboBox->setCurrentText(QString::fromStdString(aovs[0]));
            }
        });

        connect(usdDocument, &UsdDocument::stageOpened, this, [this, viewportGLWidget, stageUpAxisLabel]() {
            stageUpAxisLabel->setText(QString("Up Axis: %1 ").arg(viewportGLWidget->upAxisDisplayName()));
        });
}

} // namespace TINKERUSD_NS