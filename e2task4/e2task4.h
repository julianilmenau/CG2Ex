
#include <vector>
#include <chrono>

#pragma warning(push)
#pragma warning(disable : 4201)
#include <glm/vec4.hpp>
#pragma warning(pop)

#include <glbinding/gl32core/gl.h>  // this is a OpenGL feature include; it declares all OpenGL 3.2 Core symbols


// For more information on how to write C++ please adhere to: 
// http://cginternals.github.io/guidelines/cpp/index.html

class e2task4
{
public:
    e2task4();

    void initializeGL();
    void releaseGL();
    void resize(int w, int h);
    void render();
    void execute();

    void diffScale(float scale);
    float diffScale() const;

    void dxtMode(int mode);
    void zoom(int level); // 0 for no zoom, 9 for 10^9 zoom

public:
    bool loadShaders();
    bool loadTextures();
    void updateUniforms();
        
protected:

    static std::vector<std::uint8_t> encode(const std::vector<std::uint8_t> & I, int width, int height);
    
    static void setupTexture(
        gl::GLenum filter = gl::GL_NEAREST, 
        gl::GLenum wrap   = gl::GL_CLAMP_TO_EDGE);

    using nano = std::chrono::duration<long long, std::nano>;

    static void analyzeTexture(const std::string & identifier, const size_t size);
    static void analyzeTexture(
        const std::string & identifier, 
        const std::vector<glm::vec4> & I,
        const nano & duration);

    void setupCompressed(
        const gl::GLint texture
    ,   const std::vector<char> & raw
    ,   const int width
    ,   const int height
    ,   const gl::GLenum format
    ,   const std::vector<gl::GLenum> & formats
    ,   const std::vector<glm::vec4> & If
    ,   const bool bypassGraphicsDriverEncoding
    ,   const std::string & fileName
    ,   const bool writeToFile);

protected:
    gl::GLuint m_vbo;
    gl::GLuint m_colors;
    gl::GLuint m_program;
    gl::GLuint m_vertexShader;
    gl::GLuint m_fragmentShader;
    gl::GLuint m_vao;
    std::array<gl::GLuint, 5> m_textures;

    gl::GLint m_offsetLocation;
    gl::GLint m_scaleLocation;
    gl::GLint m_catunLocation;
    gl::GLint m_catcoLocation;
    gl::GLint m_diffScaleLocation;
    gl::GLint m_zoomLocation;
    gl::GLint m_programLocation;

    int m_width;
    int m_height;

    gl::GLfloat m_diffScale;
    gl::GLint m_dxtMode;
    gl::GLint m_zoom;
};
