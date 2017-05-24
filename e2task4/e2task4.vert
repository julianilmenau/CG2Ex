// according GLSL version to OpenGL 3.2 core profile
#version 150 core

// input into vertex shader (configured by vertex array configuration and filled from vertex buffer)
in vec2 in_vertex;

uniform vec2 offset;
uniform vec2 scale;
uniform int zoom;

// additional color output for this shader; will be interpolated among the 3 vertices building the triangle
out vec2 v_uv;

void main()
{
    gl_Position = vec4(in_vertex * scale + offset, 0.0, 1.0);
    v_uv = (in_vertex.xy * 0.5 / pow(2.0, zoom) + 0.5);
}
