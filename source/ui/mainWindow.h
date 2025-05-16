#pragma once

#include <QMainWindow>

namespace TINKERUSD_NS
{

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget* parent = nullptr);
    virtual ~MainWindow() = default;
};

} // namespace TINKERUSD_NS