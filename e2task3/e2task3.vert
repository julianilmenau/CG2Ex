// according GLSL version to OpenGL 3.3 core profile
#version 150 core
#extension GL_ARB_explicit_attrib_location : require

layout(location = 0) in vec3 in_vertex;
layout(location = 1) in vec3 in_color;
layout(location = 2) in vec3 in_localCoord;
layout(location = 3) in int in_id;
layout(location = 4) in int in_type;

// additional color output for this shader; will be interpolated among the 3 vertices building the triangle
out vec3 v_color;
out vec3 v_localCoord;
out int v_id;
out int v_type;

void main()
{
    gl_Position = vec4(in_vertex, 1.0);
    v_color = in_color;
    v_localCoord = in_localCoord;
    v_id = in_id;
    v_type = in_type;
}
