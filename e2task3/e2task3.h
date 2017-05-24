
#include <glbinding/gl32core/gl.h>  // this is a OpenGL feature include; it declares all OpenGL 3.2 Core symbols


// For more information on how to write C++ please adhere to: 
// http://cginternals.github.io/guidelines/cpp/index.html

class e2task3
{
public:
    e2task3();

    void initializeGL();
    void releaseGL();
    bool loadShaders();
    void generateGeometry();

    void resize(int w, int h);
    void render();
    void execute();

    void printInfos(double x, double y);

protected:
    gl::GLuint m_vertices;
    gl::GLuint m_indices;
    gl::GLuint m_program;
    gl::GLuint m_vertexShader;
    gl::GLuint m_geometryShader;
    gl::GLuint m_fragmentShader;
    gl::GLuint m_vao;

    gl::GLuint m_fbo;

    gl::GLuint m_colorTexture;
    gl::GLuint m_depthTexture;
	gl::GLuint m_idTexture;
	gl::GLuint m_localnormalTexture;
	gl::GLuint m_globalnormalTexture;
	gl::GLuint m_localcoordTexture;
	gl::GLuint m_globalcoordTexture;
    int m_width;
    int m_height;
    int m_indexCount;

protected:
    void updateUniforms();
};
