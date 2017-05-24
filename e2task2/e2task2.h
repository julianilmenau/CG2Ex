
#include <glbinding/gl32core/gl.h>  // this is a OpenGL feature include; it declares all OpenGL 3.2 Core symbols


// For more information on how to write C++ please adhere to: 
// http://cginternals.github.io/guidelines/cpp/index.html

class e2task2
{
public:
    e2task2();

    void initializeGL();
    void releaseGL();
    bool loadShaders();
    void generateGeometry();

    void resize(int w, int h);
    void render();
    void execute();

protected:
    gl::GLuint m_vertices;
    gl::GLuint m_indices;
    gl::GLuint m_program;
    gl::GLuint m_vertexShader;
    gl::GLuint m_fragmentShader;
    gl::GLuint m_vao;
    gl::GLuint m_heightField;
    gl::GLuint m_groundTexture;

    int m_width;
    int m_height;
    int m_indexCount;

protected:
    void updateUniforms();
};
