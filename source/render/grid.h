#pragma once

#include <QColor>
#include <QOpenGLFunctions_4_5_Core>

#include <pxr/base/gf/matrix4f.h>
#include <pxr/base/tf/token.h>

PXR_NAMESPACE_USING_DIRECTIVE

namespace TINKERUSD_NS 
{

class UsdCamera;
class Grid : public QOpenGLFunctions_4_5_Core
{
public:
    Grid();
    virtual ~Grid();

    void initialize();
    void draw(const UsdCamera* camera, const TfToken& upAxis);

    void setSize(float size);
    void setCellCount(int count);
    void setBaseLinesColor(const QColor& color);
    void setMajorLinesColor(const QColor& color);

private:
    void setupShader();

private:
    GLuint m_vao;
    GLuint m_vbo;
    GLuint m_shaderProgram;
    float  m_size;
    int m_cellCount;
    QColor m_baseLinesColor;
    QColor m_majorLinesColor;
};

} // namespace TINKERUSD_NS