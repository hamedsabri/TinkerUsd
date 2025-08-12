#pragma once
#include <QDialog>

class QDoubleSpinBox;

namespace TINKERUSD_NS 
{

class ViewportOpenGLWidget;

class CameraSettingsDialog : public QDialog
{
    Q_OBJECT
public:
    CameraSettingsDialog(ViewportOpenGLWidget* viewport, QWidget* parent = nullptr);

private slots:
    void onAccept();

private:
    ViewportOpenGLWidget* m_viewport;
    QDoubleSpinBox* m_nearSpin;
    QDoubleSpinBox* m_farSpin;
};

} // namespace TINKERUSD_NS
