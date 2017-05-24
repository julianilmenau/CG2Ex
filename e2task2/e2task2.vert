// according GLSL version to OpenGL 3.2 core profile
#version 150 core

uniform mat4 viewProjection;
uniform sampler2D heightField;

in vec3 in_vertex;

out vec2 v_texCoords;
// TODO 2.2: Pass interpolated vertex height to fragment shader
out float height;

void main()
{
    // TODO 2.2: load height from height field
    float in_height = texture(heightField, in_vertex.xz * 2).x;
    
    // displace vertex position dependend on the height
    gl_Position = viewProjection * vec4(in_vertex.x, in_height * 0.4, in_vertex.z, 1.0);
    v_texCoords = in_vertex.xz * 6.0;
    height = in_height;
}
