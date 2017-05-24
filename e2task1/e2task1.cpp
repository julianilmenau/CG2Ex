
#include "e2task1.h"

#include <iostream>
#include <string>
#include <vector>

#pragma warning(push)
#pragma warning(disable : 4201)
#include <glm/geometric.hpp>
#pragma warning(pop)

#include <glbinding/gl32core/gl.h>

#include "common.h"


using namespace gl32core;


e2task1::e2task1()
: m_running(false)
{
}

void e2task1::initializeGL()
{
    // set color used when clearing the frame buffer
    glClearColor(0.12f, 0.14f, 0.18f, 1.0f);

    float vertices[] = { 1.f, -1.f, 1.f, 1.f, -1.f, -1.f, -1.f, 1.f };

    glGenBuffers(1, &m_vertices);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

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

    // create vertex attribute configuration (the input to vertex shaders)
    glGenVertexArrays(1, &m_vao);

    // bind it since we want to operate on it
    glBindVertexArray(m_vao);

    // bind vertex buffer to bnding point "array buffer": needed for vertex attribute configuration
    glBindBuffer(GL_ARRAY_BUFFER, m_vertices);

    // configure the current buffer at GL_ARRAY_BUFFER to be input to the vertex shader, using the vertex interpretation defined here
    // read two floats, use them for vertex shader input 0 and move 2 floats forward for the next vertex
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

    // enable previously configured vertex shader input
    glEnableVertexAttribArray(0);

    // create a texture 
    glGenTextures(1, &m_texture);

    loadTextures();

    // optional: bind the fragment shader output 0 to "out_color", which is 0 by default
    glBindFragDataLocation(m_program, 0, "out_color");

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_start = std::chrono::high_resolution_clock::now();
}

void e2task1::releaseGL()
{
    // Flag all aquired resources for deletion (hint: driver decides when to actually delete them; see: shared contexts)
    glDeleteBuffers(1, &m_vertices);
    glDeleteProgram(m_program);
    glDeleteShader(m_vertexShader);
    glDeleteShader(m_fragmentShader);
    glDeleteVertexArrays(1, &m_vao);
    glDeleteTextures(1, &m_texture);
}

void e2task1::setRunning(bool on)
{
    m_running = on;

    glUseProgram(m_program);
    glUniform1i(m_runningLocation, m_running);
    glUniform1i(m_fullDurationLocation, m_running ? 2000 : 1000);
    glUseProgram(0);

    m_start = std::chrono::high_resolution_clock::now();
}

void e2task1::setLookTo(double x, double y)
{
    m_direction = glm::normalize(glm::vec2(x, m_height - y) - glm::vec2(m_width, m_height) / glm::vec2(2.0));

    glUseProgram(m_program);
    glUniform2f(m_directionLocation, m_direction.x, m_direction.y);
    glUseProgram(0);
}

bool e2task1::loadShaders()
{
    // attach 1 source to vertex shader
    const auto vertexShaderSource = textFromFile("e2task1/e2task1.vert");
    const auto vertexShaderSource_ptr = vertexShaderSource.c_str();
    if(vertexShaderSource_ptr)
        glShaderSource(m_vertexShader, 1, &vertexShaderSource_ptr, 0);

    // compile vertex shader
    glCompileShader(m_vertexShader);

    bool success = checkForCompilationError(m_vertexShader, "vertex shader");

    // attach 1 source to fragment shader
    const auto fragmentShaderSource = textFromFile("e2task1/e2task1.frag");
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

    loadUniformLocations();

    return true;
}

void e2task1::loadUniformLocations()
{
    glUseProgram(m_program);

    m_offsetLocation  = glGetUniformLocation(m_program, "offset");
    m_scaleLocation  = glGetUniformLocation(m_program, "scale");
    m_spritesLocation = glGetUniformLocation(m_program, "sprites");
    m_currentDurationLocation = glGetUniformLocation(m_program, "currentDuration");
    m_fullDurationLocation = glGetUniformLocation(m_program, "fullDuration");
    m_directionLocation = glGetUniformLocation(m_program, "direction");
    m_runningLocation = glGetUniformLocation(m_program, "running");

    const auto squarifying = static_cast<float>(m_width) / m_height;
    const auto scale = std::min(1.0f, static_cast<float>(m_height) / m_width * 2.0f);

    glUseProgram(m_program);
    glUniform2f(m_offsetLocation, 0.0f, 0.0f);
    glUniform1i(m_spritesLocation, 0);
    glUniform2f(m_scaleLocation, 0.7f * scale, 0.7f * squarifying * scale);
    glUniform2f(m_directionLocation, m_direction.x, m_direction.y);
    glUniform1i(m_runningLocation, m_running);
    glUniform1i(m_fullDurationLocation, m_running ? 2000 : 1000);

    glUseProgram(0);
}

bool e2task1::loadTextures()
{
    glBindTexture(GL_TEXTURE_2D, m_texture);

    auto raw = rawFromFile("e2task1/sprites.7200.2780.rgba.ub.raw");
    // allocate memory on GPU for the texture (can also be used to upload actual texel data using the last parameter)
    glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(GL_RGBA8), 7200, 2780, 0, GL_RGBA, GL_UNSIGNED_BYTE, raw.data());

    // configure required min/mag filter and wrap modes
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(GL_LINEAR));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(GL_LINEAR));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(GL_CLAMP_TO_EDGE));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(GL_CLAMP_TO_EDGE));

    return true;
}

void e2task1::resize(int w, int h)
{
    m_width = w;
    m_height = h;

    const auto squarifying = static_cast<float>(m_width) / m_height;
    const auto scale = std::min(1.0f, static_cast<float>(m_height) / m_width * 2.0f);

    glUseProgram(m_program);
    glUniform2f(m_scaleLocation, 0.45f * scale, 0.45f * squarifying * scale);
    glUseProgram(0);
}

void e2task1::render()
{
    const auto current = std::chrono::high_resolution_clock::now();

    // Define the area for the rasterizer that is used for the NDC mapping ([-1, 1]^2 x [0, 1])
    glViewport(0, 0, m_width, m_height);

    // clear offscreen-framebuffer color attachment (no depth attachment configured and thus omitting GL_DEPTH_BUFFER_BIT)
    glClear(GL_COLOR_BUFFER_BIT);

    // bind program and textures for use in next draw calls
    glUseProgram(m_program);

    glUniform1i(m_currentDurationLocation, std::chrono::duration_cast<std::chrono::milliseconds>(current - m_start).count());

    // bind vertex array object to restore vertex shader input configuration
    glBindVertexArray(m_vao);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // cleanup: release currently used vertex array
    glBindVertexArray(0);
}

void e2task1::execute()
{
    render();
}
