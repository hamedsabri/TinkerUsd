#include "usdDrawTargetFBO.h"

#include <pxr/base/gf/vec2i.h>
#include <pxr/usd/usd/prim.h>

namespace
{
const char* vertexShaderSrc = R"(#version 450 core
    layout(location = 0) in vec2 aPos;
    layout(location = 1) in vec2 aTexCoord;
    out vec2 TexCoord;
    void main()
    {
        TexCoord = aTexCoord;
        gl_Position = vec4(aPos, 0.0, 1.0);
    }
    )";

const char* fragmentShaderSrc = R"(#version 450 core
        in vec2 TexCoord;
        out vec4 FragColor;
        uniform sampler2D screenTexture;

        // don't use a cannon to kill a mosquito.! let's do the linear to SRGB in the pixel shader.
        // good enough for now.
        vec3 linearToSRGB(vec3 color) {
            vec3 srgbLo = color * 12.92;
            vec3 srgbHi = (pow(abs(color), vec3(1.0/2.4)) * 1.055) - 0.055;
            return mix(srgbLo, srgbHi, vec3(greaterThan(color, vec3(0.0031308))));
        }

        void main()
        {
            vec4 linearColor = texture(screenTexture, TexCoord);
            vec3 srgbColor = linearToSRGB(linearColor.rgb);
            FragColor = vec4(srgbColor, linearColor.a);
        }
    )";
} // namespace

namespace TINKERUSD_NS
{

UsdDrawTargetFBO::UsdDrawTargetFBO()
    : m_vao(0)
    , m_vbo(0)
    , m_width(512)
    , m_height(512)
    , m_shaderProgram(0)
{
    initializeOpenGLFunctions();
}

UsdDrawTargetFBO::~UsdDrawTargetFBO()
{
    if (m_vbo)
    {
        glDeleteBuffers(1, &m_vbo);
    }
    if (m_vao)
    {
        glDeleteVertexArrays(1, &m_vao);
    }
}

void UsdDrawTargetFBO::resize(int width, int height)
{
    if (width == m_width && height == m_height)
    {
        return;
    }
    m_width = width;
    m_height = height;

    // create draw target
    m_drawTarget = GlfDrawTarget::New(GfVec2i(m_width, m_height));
    m_drawTarget->Bind();

    m_drawTarget->AddAttachment(TfToken("color"), GL_RGBA, GL_FLOAT, GL_RGBA);

    m_drawTarget->AddAttachment(TfToken("depth"), GL_DEPTH_COMPONENT, GL_FLOAT, GL_DEPTH_COMPONENT32F);

    m_drawTarget->Unbind();

    // compile shaders
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSrc, nullptr);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSrc, nullptr);
    glCompileShader(fragmentShader);

    m_shaderProgram = glCreateProgram();
    glAttachShader(m_shaderProgram, vertexShader);
    glAttachShader(m_shaderProgram, fragmentShader);
    glLinkProgram(m_shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    initializeQuad();
}

void UsdDrawTargetFBO::initializeQuad()
{
    static const float quadVertices[] = {
        // positions   // texCoords
        -1.0f, 1.0f,  0.0f,
        1.0f, // Top-left
        1.0f,  1.0f,  1.0f,
        1.0f, // Top-right
        1.0f,  -1.0f, 1.0f,
        0.0f, // Bottom-right
        -1.0f, -1.0f, 0.0f,
        0.0f // Bottom-left
    };

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0); // position
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1); // texCoords
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void UsdDrawTargetFBO::bind()
{
    if (m_drawTarget)
    {
        m_drawTarget->Bind();
    }
}

void UsdDrawTargetFBO::unbind()
{
    if (m_drawTarget)
    {
        m_drawTarget->Unbind();
    }
}

void UsdDrawTargetFBO::draw()
{
    if (!m_drawTarget)
    {
        return;
    }

    GLuint texId = m_drawTarget->GetAttachment("color")->GetGlTextureName();
    if (texId == 0)
    {
        return;
    }

    glDisable(GL_DEPTH_TEST);

    glUseProgram(m_shaderProgram);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texId);

    glUniform1i(glGetUniformLocation(m_shaderProgram, "screenTexture"), 0);

    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);

    glUseProgram(0);

    glBindTexture(GL_TEXTURE_2D, 0);
}

} // namespace TINKERUSD_NS
