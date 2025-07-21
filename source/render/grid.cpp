#include "grid.h"

#include "camera/usdCamera.h"

#include <QOpenGLShaderProgram>
#include <pxr/base/gf/matrix4f.h>

namespace
{
const char* vertexShaderSrc = R"(
        #version 450 core
        layout(location = 0) in vec3 aPos;
        uniform mat4 uViewMatrix;
        uniform mat4 uProjectionMatrix;
        void main() {
            gl_Position = uProjectionMatrix * uViewMatrix * vec4(aPos, 1.0);
        }
    )";

const char* fragmentShaderSrc = R"(
        #version 450 core
        uniform vec4 uColor;
        out vec4 FragColor;
        void main() {
            FragColor = uColor;
        }
    )";
} // namespace

namespace TINKERUSD_NS
{

Grid::Grid()
    : m_vao(0)
    , m_vbo(0)
    , m_shaderProgram(0)
    , m_size(30.0f)
    , m_cellCount(30)
    , m_baseLinesColor(QColor(124, 124, 124))
    , m_majorLinesColor(QColor(180, 210, 234))
{
}

Grid::~Grid()
{
    if (m_vao)
    {
        glDeleteVertexArrays(1, &m_vao);
    }
    if (m_vbo)
    {
        glDeleteBuffers(1, &m_vbo);
    }
    if (m_shaderProgram)
    {
        glDeleteProgram(m_shaderProgram);
    }
}

void Grid::initialize()
{
    initializeOpenGLFunctions();
    setupShader();

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
}

void Grid::setupShader()
{
    GLuint vert = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert, 1, &vertexShaderSrc, nullptr);
    glCompileShader(vert);

    GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag, 1, &fragmentShaderSrc, nullptr);
    glCompileShader(frag);

    m_shaderProgram = glCreateProgram();
    glAttachShader(m_shaderProgram, vert);
    glAttachShader(m_shaderProgram, frag);
    glLinkProgram(m_shaderProgram);

    glDeleteShader(vert);
    glDeleteShader(frag);
}

void Grid::draw(const UsdCamera* camera, const TfToken& upAxis)
{
    if (!camera)
    {
        return;
    }

    std::vector<GLfloat> baseLines;
    std::vector<GLfloat> majorLines;

    const float extent = m_size * m_cellCount;
    const bool  yUp = (upAxis == TfToken("Y"));
    const bool  zUp = (upAxis == TfToken("Z"));
    const bool  xUp = (upAxis == TfToken("X"));

    for (auto i = -m_cellCount; i <= m_cellCount; ++i)
    {
        float pos = i * m_size;
        // TODO: hight every 10th row for now
        bool isHighlight = (i % 10 == 0);

        auto& target = isHighlight ? majorLines : baseLines;

        if (yUp)
        {
            target.insert(
                target.end(),
                { pos, 0.0f, -extent, pos, 0.0f, extent, -extent, 0.0f, pos, extent, 0.0f, pos });
        }
        else if (zUp)
        {
            target.insert(
                target.end(),
                { pos, -extent, 0.0f, pos, extent, 0.0f, -extent, pos, 0.0f, extent, pos, 0.0f });
        }
        else if (xUp)
        {
            target.insert(
                target.end(),
                { 0.0f, pos, -extent, 0.0f, pos, extent, 0.0f, -extent, pos, 0.0f, extent, pos });
        }
    }

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    auto drawLines = [this, camera](const std::vector<GLfloat>& lines, const QColor& color) {
        glBufferData(GL_ARRAY_BUFFER, lines.size() * sizeof(float), lines.data(), GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(0);

        glUseProgram(m_shaderProgram);

        GfMatrix4f viewMatrix(camera->getViewMatrix());
        GfMatrix4f projMatrix(camera->getProjectionMatrix());

        glUniformMatrix4fv(
            glGetUniformLocation(m_shaderProgram, "uViewMatrix"), 1, GL_FALSE, viewMatrix.data());
        glUniformMatrix4fv(
            glGetUniformLocation(m_shaderProgram, "uProjectionMatrix"), 1, GL_FALSE, projMatrix.data());

        glUniform4f(
            glGetUniformLocation(m_shaderProgram, "uColor"),
            color.redF(),
            color.greenF(),
            color.blueF(),
            1.0f);

        glDrawArrays(GL_LINES, 0, lines.size() / 3);
    };

    drawLines(baseLines, m_baseLinesColor);
    drawLines(majorLines, m_majorLinesColor);

    glBindVertexArray(0);
    glUseProgram(0);
}

void Grid::setSize(float size) { m_size = size; }
void Grid::setCellCount(int count) { m_cellCount = count; }

void Grid::setBaseLinesColor(const QColor& color) { m_baseLinesColor = color; }

void Grid::setMajorLinesColor(const QColor& color) { m_majorLinesColor = color; }

} // namespace TINKERUSD_NS
