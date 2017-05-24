// according GLSL version to OpenGL 3.3 core profile
#version 150 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec3 v_color[];
in vec3 v_localCoord[];
in int v_id[];
in int v_type[];

uniform mat4 viewProjection;

out vec3 g_color;
out vec3 g_worldCoord;
out vec3 g_localCoord;
flat out vec3 g_worldNormal;
flat out vec3 g_localNormal;
flat out int g_id;
flat out int g_type;

void main()
{
    for (int i=0; i < 3; ++i)
    {
        gl_Position = viewProjection * gl_in[i].gl_Position;
        
        g_color = v_color[i];
        g_worldCoord = gl_in[i].gl_Position.xyz;
        g_localCoord = v_localCoord[i].xyz;
        g_worldNormal = cross(gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz, gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz);
        g_localNormal = cross(v_localCoord[1].xyz - v_localCoord[0].xyz, v_localCoord[2].xyz - v_localCoord[0].xyz);
        g_id = v_id[i];
        g_type = v_type[i];
        
        EmitVertex();
    }
    
    EndPrimitive();
}
