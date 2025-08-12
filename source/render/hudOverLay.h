#pragma once

#include <QOpenGLFunctions_4_5_Core>
#include <QImage>
#include <QString>

namespace TINKERUSD_NS
{

// HUD text renderer for the USD viewport. Mimicking most of the HUD renderer logic from Usdview here. 
// The text is first rendered into a QImage using QPainter, then uploaded as an
// OpenGL texture and drawn as a screen-space quad in normalized device coordinates.
// This approach bypasses the drawing of text directly with QPainter on top of the GL widget.
// https://forum.aousd.org/t/usdrenderenginegl-leaves-gl-state-that-breaks-qpainter-overlays-in-qopenglwidget/2639

class HudOverlay final
{
public:
    HudOverlay() = default;
    ~HudOverlay();

    // initialize GL resources (call after a valid GL context is current).
    void init(QOpenGLFunctions_4_5_Core* f);

    // update the text image; call when the HUD string changes.
    // dpr = devicePixelRatioF(); basePx = base pixel size (logical).
    void updateText(const QString& text, float dpr, int basePx = 14);

    // draw the HUD at top-left. widgetW/H are widget logical size (width(), height()).
    void draw(int widgetW, int widgetH, float dpr);

    //change position in device pixels
    void setPositionPx(int x, int y) { m_posX = x; m_posY = y; }

private:
    GLuint createProgram(const char* vs, const char* fs);
    void   destroyGL();

private:
    QImage m_img;

    QOpenGLFunctions_4_5_Core* m_glFunctionCore{nullptr};

    bool   m_inited{false};
    bool   m_needsUpload{false};
    GLuint m_vao{ 0 };
    GLuint m_vbo{ 0 };
    GLuint m_prog{ 0 };
    GLuint m_tex{ 0 };
    GLint  m_uRect{ -1 };
    GLint  m_uTex{ 0 };
    int    m_posX{ 8 };
    int    m_posY{ 8 };
};

} // namespace TINKERUSD_NS
