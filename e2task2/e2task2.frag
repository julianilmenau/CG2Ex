// according GLSL version to OpenGL 3.2 core profile
#version 150 core

uniform sampler2DArray groundTexture;

in vec2 v_texCoords;
in float height;

out vec4 out_color;

uniform float h0 = 0.1;
uniform float h1 = 0.3;
uniform float h2 = 0.6;
uniform float he = 0.1;

void main()
{
    // TODO 2.2: select & sample appropriate tile(s) from texture array
    float texnr = 1;
    float h = h0;

    vec2 texHeight = vec2 (texnr, h);

    texHeight = mix(vec2(2, h1),texHeight, float(height < h0 + he / 2));
    texHeight = mix(vec2(3, h2), texHeight, float(height < h1 + he / 2));
    
    texnr = texHeight.x;
	h = texHeight.y;
    float deltah = (height - h + (he / 2)) / he;
    
    vec4 color1 = texture(groundTexture, vec3(v_texCoords, int(texnr - 1)));
    vec4 color2 = texture(groundTexture, vec3(v_texCoords, int(texnr)));

    out_color = mix(color1, color2, clamp(deltah, 0, 1));
}
