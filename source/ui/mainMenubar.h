#pragma once

#include <QMenu>
#include <QMenuBar>

namespace TINKERUSD_NS
{

class MainMenuBar : public QMenuBar
{
    Q_OBJECT
public:
    MainMenuBar(QWidget* parent = nullptr);
    virtual ~MainMenuBar() = default;

    QMenu* getPanelsMenu() const { return m_panelsMenu; }

signals:
    void requestNewStage();
    void requestOpenStage(const QString& path);
    void camFrameSelectSignal();
    void camResetSignal();

private:
    void setupMenus();

private:
    QMenu* m_panelsMenu;
};

} // namespace TINKERUSD_NS