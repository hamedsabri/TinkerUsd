#include "cameraSettingsDialog.h"
#include "viewportOpenGLWidget.h"

#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QVBoxLayout>

using namespace TINKERUSD_NS;

CameraSettingsDialog::CameraSettingsDialog(ViewportOpenGLWidget* viewport, QWidget* parent)
    : QDialog(parent)
    , m_viewport(viewport)
{
    setWindowTitle("Camera Settings");
    auto* form = new QFormLayout;

    m_nearSpin = new QDoubleSpinBox(this);
    m_nearSpin->setDecimals(5);
    m_nearSpin->setRange(0.00001, 1e6);
    m_nearSpin->setSingleStep(0.001);
    m_nearSpin->setValue(m_viewport ? m_viewport->nearClip() : 0.1);

    m_farSpin = new QDoubleSpinBox(this);
    m_farSpin->setDecimals(3);
    m_farSpin->setRange(0.01, 1e9);
    m_farSpin->setSingleStep(1.0);
    m_farSpin->setValue(m_viewport ? m_viewport->farClip() : 1000000.0);

    form->addRow("Near Clip", m_nearSpin);
    form->addRow("Far Clip",  m_farSpin);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &CameraSettingsDialog::onAccept);
    connect(buttons, &QDialogButtonBox::rejected, this, &CameraSettingsDialog::reject);

    auto* layout = new QVBoxLayout(this);
    layout->addLayout(form);
    layout->addWidget(buttons);
    setLayout(layout);
}

void CameraSettingsDialog::onAccept()
{
    double n = m_nearSpin->value();
    double f = m_farSpin->value();

    // sanity check: keep far > near
    if (f <= n) f = n * 10.0;

    m_viewport->setClipPlanes(n, f);
    accept();
}
