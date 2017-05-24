// according GLSL version to OpenGL 3.3 core profile
#version 150 core

in vec3 g_color;
in vec3 g_worldCoord;
in vec3 g_localCoord;
flat in vec3 g_worldNormal;
flat in vec3 g_localNormal;
flat in int g_id;
flat in int g_type;

uniform vec3 eye;
uniform mat3 normalMatrix;

layout (location=0) out vec3 out_color;
layout (location=1) out vec3 out_id;

layout (location=2) out vec3 out_locnorm;
layout (location=3) out vec3 out_globnorm;

layout (location=4) out vec3 out_loccoord;
layout (location=5) out vec3 out_globcoord;
 

// TODO 2.3: Add more render target outputs

void main()
{
    out_color = g_color * mix(0.4, 1.0, dot(normalMatrix * normalize(g_worldNormal), normalize(eye)));
	out_id = vec3(g_id/255.0,g_type/255.0,0);
	out_locnorm=(g_localNormal+vec3(1,1,1))/2.0;
	out_globnorm=(g_worldNormal+vec3(1,1,1))/2.0;
	out_loccoord=(g_localCoord+vec3(1,1,1))/2.0;
	out_globcoord=(g_worldCoord+vec3(1,1,1))/2.0;
    // TODO 2.3: Write to additional render target outputs
}
