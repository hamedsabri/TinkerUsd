#include "ui/mainWindow.h"
#include "ui/logger/loggerWidget.h"

#include <QApplication>
#include <QFile>
#include <QGuiApplication>

using namespace TINKERUSD_NS;

int main(int argc, char** argv)
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);

    QFile styleFile(":/darktheme.css");
    if (styleFile.open(QFile::ReadOnly | QFile::Text))
    {
        QString darkStyle = styleFile.readAll();
        app.setStyleSheet(darkStyle);
    }

    LogWidget::instance().installMessageHandler();
    LogWidget::instance().setToStdout(true);

    app.setWindowIcon(QIcon(":/app_icon.svg"));

    MainWindow mainWindow;
    mainWindow.resize(1600, 800);
    mainWindow.show();

    return app.exec();
}