// according GLSL version to OpenGL 3.2 core profile
#version 150 core

uniform sampler2D sprites;


in vec2 v_uvFrameStartTimeStart;
in vec2 v_uvFrameStartTimeEnd;
in vec2 v_uvFrameEndTimeStart;
in vec2 v_uvFrameEndTimeEnd;

in float dFrame;
in float dMove;

out vec4 out_color;

void main()
{
	vec4 color1 = mix(texture(sprites, v_uvFrameStartTimeStart), texture(sprites, v_uvFrameStartTimeEnd), dMove);
	vec4 color2 = mix(texture(sprites, v_uvFrameEndTimeStart), texture(sprites, v_uvFrameEndTimeEnd), dMove);

    out_color = mix(color1, color2, dFrame);
}
