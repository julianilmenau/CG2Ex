
#include <chrono>

#include <glm/vec2.hpp>

#include <glbinding/gl32core/gl.h>  // this is a OpenGL feature include; it declares all OpenGL 3.2 Core symbols


// For more information on how to write C++ please adhere to: 
// http://cginternals.github.io/guidelines/cpp/index.html

class e2task1
{
public:
    e2task1();

    void initializeGL();
    void releaseGL();
    bool loadShaders();
    bool loadTextures();

    void setRunning(bool on);
    void setLookTo(double x, double y);

    void resize(int w, int h);
    void render();
    void execute();

protected:
    void loadUniformLocations();

protected:
    gl::GLuint m_vertices;
    gl::GLuint m_program;
    gl::GLuint m_vertexShader;
    gl::GLuint m_fragmentShader;
    gl::GLuint m_vao;
    gl::GLuint m_texture;

    gl::GLint m_offsetLocation;
    gl::GLint m_scaleLocation;
    gl::GLint m_spritesLocation;
    gl::GLint m_currentDurationLocation;
    gl::GLint m_fullDurationLocation;
    gl::GLint m_directionLocation;
    gl::GLint m_runningLocation;

    int m_width;
    int m_height;

    bool m_running;
    glm::vec2 m_direction;

    std::chrono::high_resolution_clock::time_point m_start;
};
