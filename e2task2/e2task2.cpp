
#include "e2task2.h"

#include <string>
#include <vector>

#pragma warning(push)
#pragma warning(disable : 4201)
#include <glm/gtc/random.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#pragma warning(push)

#include <glbinding/gl32core/gl.h>
#include <glbinding/gl32core/enum.h>

#include "common.h"
#include "Plane.h"


using namespace gl32core;


e2task2::e2task2()
: m_vertices(0)
, m_program(0)
, m_vertexShader(0)
, m_fragmentShader(0)
, m_vao(0)
, m_width(0)
, m_height(0)
, m_indexCount(0)
{
}

void e2task2::initializeGL()
{
    glEnable(GL_DEPTH_TEST);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClearDepth(1.0f);

    // create a rendering program, holding vertex and fragment shader
    m_program = glCreateProgram();

    // create a vertex shader
    m_vertexShader = glCreateShader(GL_VERTEX_SHADER);

    // create a fragment shader
    m_fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    // attach shaders to program
    glAttachShader(m_program, m_vertexShader);
    glAttachShader(m_program, m_fragmentShader);

    loadShaders();
    generateGeometry();

    // create vertex attribute configuration (the input to vertex shaders)
    glGenVertexArrays(1, &m_vao);

    // bind it since we want to operate on it
    glBindVertexArray(m_vao);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indices);

    // bind vertex buffer to bnding point "array buffer": needed for vertex attribute configuration
    glBindBuffer(GL_ARRAY_BUFFER, m_vertices);

    // configure the current buffer at GL_ARRAY_BUFFER to be input to the vertex shader, using the vertex interpretation defined here
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0); // in_vertex

    // enable previously configured vertex shader input
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenTextures(1, &m_heightField);
    glGenTextures(1, &m_groundTexture);

    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, m_heightField);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(GL_NEAREST));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(GL_NEAREST));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(GL_CLAMP_TO_EDGE));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(GL_CLAMP_TO_EDGE));

    auto raw = rawFromFile("e2task2/height.512.512.r.f.raw");
    // allocate memory on GPU for the texture (can also be used to upload actual texel data using the last parameter)
    glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(GL_RED), 512, 512, 0, GL_RED, GL_FLOAT, raw.data());

    glActiveTexture(GL_TEXTURE1);

    glBindTexture(GL_TEXTURE_2D_ARRAY, m_groundTexture);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(GL_NEAREST));
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(GL_NEAREST));
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, static_cast<GLint>(GL_MIRRORED_REPEAT));
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, static_cast<GLint>(GL_MIRRORED_REPEAT));

    auto raw2 = rawFromFile("e2task2/terrain.512.2048.rgba.ub.raw");
    // allocate memory on GPU for the texture (can also be used to upload actual texel data using the last parameter)
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, static_cast<GLint>(GL_RGBA8), 512, 512, 4, 0, GL_RGBA, GL_UNSIGNED_BYTE, raw2.data());
}

void e2task2::releaseGL()
{
    // Flag all aquired resources for deletion (hint: driver decides when to actually delete them; see: shared contexts)
    glDeleteBuffers(1, &m_vertices);
    glDeleteBuffers(1, &m_indices);
    glDeleteProgram(m_program);
    glDeleteShader(m_vertexShader);
    glDeleteShader(m_fragmentShader);
    glDeleteVertexArrays(1, &m_vao);
}

void e2task2::generateGeometry()
{
    auto vertices = Plane::vertices(128);
    auto indices = Plane::indices(128);

    m_indexCount = static_cast<int>(indices.size()) * 3;

    glGenBuffers(1, &m_vertices);
    glGenBuffers(1, &m_indices);

    glBindBuffer(GL_ARRAY_BUFFER, m_vertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size() * 3, vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_indices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLuint) * indices.size() * 3, indices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

bool e2task2::loadShaders()
{
    // attach 1 source to vertex shader
    const auto vertexShaderSource = textFromFile("e2task2/e2task2.vert");
    const auto vertexShaderSource_ptr = vertexShaderSource.c_str();
    if(vertexShaderSource_ptr)
        glShaderSource(m_vertexShader, 1, &vertexShaderSource_ptr, 0);

    // compile vertex shader
    glCompileShader(m_vertexShader);

    bool success = checkForCompilationError(m_vertexShader, "vertex shader");

    // attach 1 source to fragment shader
    const auto fragmentShaderSource = textFromFile("e2task2/e2task2.frag");
    const auto fragmentShaderSource_ptr = fragmentShaderSource.c_str();
    if(fragmentShaderSource_ptr)
        glShaderSource(m_fragmentShader, 1, &fragmentShaderSource_ptr, 0);

    // compile fragment shader
    glCompileShader(m_fragmentShader);

    success &= checkForCompilationError(m_fragmentShader, "fragment shader");

    if (!success)
    {
        return false;
    }

    // link program (confer to standard program compilation process: [tokenizer, lexer,] compiler, linker)
    glLinkProgram(m_program);

    success &= checkForLinkerError(m_program, "program");

    if (!success)
    {
        return false;
    }

    updateUniforms();

    // optional: bind the fragment shader output 0 to "out_color", which is 0 by default
    glBindFragDataLocation(m_program, 0, "out_color");

    return true;
}

void e2task2::resize(int w, int h)
{
    m_width = w;
    m_height = h;

    if (m_program)
    {
        updateUniforms();
    }
}

void e2task2::updateUniforms()
{
    glm::vec3 eye = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::mat4 view = glm::lookAt(eye, glm::vec3(0.0f, -0.35f, 0.0f), up);
    glm::mat3 normal = glm::inverseTranspose(glm::mat3(view));
    glm::mat4 viewProjection = glm::perspectiveFov(glm::radians(28.0f), float(m_width), float(m_height), 0.01f, 2.0f) * view;

    GLint viewProjectionLocation = glGetUniformLocation(m_program, "viewProjection");
    GLint heightFieldLocation = glGetUniformLocation(m_program, "heightField");
    GLint groundTextureLocation = glGetUniformLocation(m_program, "groundTexture");

    glUseProgram(m_program);
    glUniform1i(heightFieldLocation, 0);
    glUniform1i(groundTextureLocation, 1);
    glUniformMatrix4fv(viewProjectionLocation, 1, GL_FALSE, glm::value_ptr(viewProjection));
    glUseProgram(0);
}

void e2task2::render()
{
    glViewport(0, 0, m_width, m_height);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_heightField);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_groundTexture);

    glBindVertexArray(m_vao);
    glUseProgram(m_program);
    glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, nullptr);
    glUseProgram(0);
    glBindVertexArray(0);
}

void e2task2::execute()
{
    render();
}
