#include "hudOverlay.h"

#include <QPainter>
#include <QFont>
#include <QFontMetrics>
#include <QColor>
#include <QRect>

#ifndef GL_BGRA
#define GL_BGRA 0x80E1
#endif

namespace TINKERUSD_NS
{

HudOverlay::~HudOverlay()
{
    destroyGL();
}

void HudOverlay::destroyGL()
{
    if (!m_glFunctionCore) {
        return;
    }
    if (m_tex)  { m_glFunctionCore->glDeleteTextures(1, &m_tex);  m_tex  = 0; }
    if (m_vbo)  { m_glFunctionCore->glDeleteBuffers(1,  &m_vbo);  m_vbo  = 0; }
    if (m_vao)  { m_glFunctionCore->glDeleteVertexArrays(1, &m_vao); m_vao = 0; }
    if (m_prog) { m_glFunctionCore->glDeleteProgram(m_prog); m_prog = 0; }
    m_inited = false;
}

void HudOverlay::init(QOpenGLFunctions_4_5_Core* f)
{
    m_glFunctionCore = f;
    if (m_inited || !m_glFunctionCore) return;

    // quad (0,0)-(1,1)
    float verts[] = {
        // pos   // uv
        0.f, 0.f, 0.f, 0.f,
        1.f, 0.f, 1.f, 0.f,
        0.f, 1.f, 0.f, 1.f,
        1.f, 1.f, 1.f, 1.f
    };

    m_glFunctionCore->glGenVertexArrays(1, &m_vao);
    m_glFunctionCore->glGenBuffers(1, &m_vbo);
    m_glFunctionCore->glBindVertexArray(m_vao);
    m_glFunctionCore->glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    m_glFunctionCore->glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    m_glFunctionCore->glEnableVertexAttribArray(0);
    m_glFunctionCore->glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    m_glFunctionCore->glEnableVertexAttribArray(1);
    m_glFunctionCore->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    m_glFunctionCore->glBindBuffer(GL_ARRAY_BUFFER, 0);
    m_glFunctionCore->glBindVertexArray(0);

    const char* vs = R"(
        #version 140
        in vec2 aPos;
        in vec2 aUV;
        out vec2 vUV;
        uniform vec4 uRect; // x, y, w, h in NDC
        void main() {
            vec2 ndc = vec2(uRect.x + aPos.x * uRect.z,
                            uRect.y + aPos.y * uRect.w);
            gl_Position = vec4(ndc, 0.0, 1.0);
            vUV = vec2(aUV.x, 1.0 - aUV.y);
        }
    )";
    const char* fs = R"(
        #version 140
        in vec2 vUV;
        out vec4 fragColor;
        uniform sampler2D uTex;
        void main() {
            fragColor = texture(uTex, vUV);
        }
    )";
    m_prog = createProgram(vs, fs);
    m_uRect = m_glFunctionCore->glGetUniformLocation(m_prog, "uRect");
    m_uTex  = m_glFunctionCore->glGetUniformLocation(m_prog,  "uTex");

    m_glFunctionCore->glGenTextures(1, &m_tex);
    m_glFunctionCore->glBindTexture(GL_TEXTURE_2D, m_tex);
    m_glFunctionCore->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    m_glFunctionCore->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    m_glFunctionCore->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    m_glFunctionCore->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    m_glFunctionCore->glBindTexture(GL_TEXTURE_2D, 0);

    m_inited = true;
}

void HudOverlay::updateText(const QString& text, float dpr, int basePx)
{
    QFont font;
    font.setPixelSize(int(basePx * std::max(1.f, dpr)));

    QFontMetrics fm(font);
    const int margin = int(6 * dpr);

    // multi-line text
    const QStringList lines = text.split('\n');
    int maxW = 0;
    for (const QString& line : lines) {
        maxW = qMax(maxW, fm.horizontalAdvance(line));
    }
    const int lineH = fm.height();
    const int textW = maxW;
    const int textH = lineH * qMax(1, int(lines.size()));

    const int W = textW + 2 * margin;
    const int H = textH + 2 * margin;

    QImage img(W, H, QImage::Format_ARGB32_Premultiplied);
    img.fill(Qt::transparent);

    QPainter p(&img);
    p.setRenderHint(QPainter::TextAntialiasing, true);

    // background
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(0, 0, 0, 160));
    p.drawRoundedRect(QRect(0, 0, W, H), int(2 * dpr), int(2 * dpr));

    // text
    p.setPen(Qt::white);
    p.setFont(font);

    int y = margin + fm.ascent();
    for (const QString& line : lines) {
        p.drawText(margin, y, line);
        y += lineH;
    }

    m_img = std::move(img);
    m_needsUpload = true;
}

void HudOverlay::draw(int widgetW, int widgetH, float dpr)
{
    if (!m_inited || widgetW <= 0 || widgetH <= 0) {
        return;  
    } 

    // upload only when text changed
    if (m_needsUpload && !m_img.isNull()) {
        // ensure row length is default (usdview does this too)
        m_glFunctionCore->glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
        m_glFunctionCore->glBindTexture(GL_TEXTURE_2D, m_tex);
        m_glFunctionCore->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                          m_img.width(), m_img.height(),
                          0, GL_BGRA, GL_UNSIGNED_BYTE, m_img.constBits());
        m_glFunctionCore->glBindTexture(GL_TEXTURE_2D, 0);
        m_needsUpload = false;
    }
    if (m_img.isNull()) {
        return;
    }

    // setup state for 2D overlay
    m_glFunctionCore->glDisable(GL_DEPTH_TEST);
    m_glFunctionCore->glDisable(GL_STENCIL_TEST);
    m_glFunctionCore->glDisable(GL_SCISSOR_TEST);
    m_glFunctionCore->glEnable(GL_BLEND);
    m_glFunctionCore->glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
                             GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    // compute NDC rect
    const float W = float(widgetW);
    const float H = float(widgetH);

    // position in device pixels (UI in window coords)
    const float xPx = float(m_posX);
    const float yPx = float(m_posY);
    const float rectW = float(m_img.width())  / dpr;
    const float rectH = float(m_img.height()) / dpr;

    // convert to NDC ([-1,1], origin at bottom-left for Y math)
    const float xNdc =  2.0f * (xPx / W) - 1.0f;
    const float yNdc =  1.0f - 2.0f * ((yPx + rectH) / H);
    const float wNdc =  2.0f * (rectW / W);
    const float hNdc =  2.0f * (rectH / H);

    m_glFunctionCore->glUseProgram(m_prog);
    m_glFunctionCore->glUniform4f(m_uRect, xNdc, yNdc, wNdc, hNdc);
    m_glFunctionCore->glUniform1i(m_uTex, 0);

    m_glFunctionCore->glActiveTexture(GL_TEXTURE0);
    m_glFunctionCore->glBindTexture(GL_TEXTURE_2D, m_tex);

    m_glFunctionCore->glBindVertexArray(m_vao);
    m_glFunctionCore->glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    m_glFunctionCore->glBindVertexArray(0);

    m_glFunctionCore->glBindTexture(GL_TEXTURE_2D, 0);
    m_glFunctionCore->glUseProgram(0);
}

GLuint HudOverlay::createProgram(const char* vs, const char* fs)
{
    GLuint v = m_glFunctionCore->glCreateShader(GL_VERTEX_SHADER);
    GLuint f = m_glFunctionCore->glCreateShader(GL_FRAGMENT_SHADER);
    m_glFunctionCore->glShaderSource(v, 1, &vs, nullptr);
    m_glFunctionCore->glCompileShader(v);
    m_glFunctionCore->glShaderSource(f, 1, &fs, nullptr);
    m_glFunctionCore->glCompileShader(f);
    GLuint p = m_glFunctionCore->glCreateProgram();
    m_glFunctionCore->glAttachShader(p, v);
    m_glFunctionCore->glAttachShader(p, f);
    m_glFunctionCore->glBindAttribLocation(p, 0, "aPos");
    m_glFunctionCore->glBindAttribLocation(p, 1, "aUV");
    m_glFunctionCore->glLinkProgram(p);
    m_glFunctionCore->glDeleteShader(v);
    m_glFunctionCore->glDeleteShader(f);
    return p;
}

} // namespace TINKERUSD_NS
