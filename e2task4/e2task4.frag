// according GLSL version to OpenGL 3.2 core profile
#version 150 core

uniform sampler2D catun;
uniform sampler2D catco;

uniform float diffScale;
uniform int program = 0;

in vec2 v_uv;

// first color attachment output
out vec4 out_color;

void main()
{
    switch(program)
    {
    case 0:
        out_color = texture(catun, v_uv);
        break;
    case 1:
        out_color = texture(catco, v_uv);
        break;
    case 2:
    	vec3 d  = texture(catun, v_uv).rgb;
    	     d -= texture(catco, v_uv).rgb;
        out_color = vec4(abs(d * diffScale), 1.0);
        break;
    }
}
