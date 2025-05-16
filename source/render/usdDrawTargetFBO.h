#pragma once

#include <QOpenGLFunctions_4_5_Core>
#include <pxr/imaging/glf/drawTarget.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/usdImaging/usdImagingGL/engine.h>

PXR_NAMESPACE_USING_DIRECTIVE

namespace TINKERUSD_NS
{

class UsdDrawTargetFBO : protected QOpenGLFunctions_4_5_Core
{
public:
    UsdDrawTargetFBO();
    virtual ~UsdDrawTargetFBO();

    void resize(int width, int height);
    void bind();
    void unbind();
    void draw();

private:
    void initializeQuad();

private:
    GLuint              m_vao;
    GLuint              m_vbo;
    size_t              m_width;
    size_t              m_height;
    GLuint              m_shaderProgram;
    GlfDrawTargetRefPtr m_drawTarget;
};

} // namespace TINKERUSD_NS
