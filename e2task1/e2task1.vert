// according GLSL version to OpenGL 3.2 core profile
#version 150 core
#define PI 3.1415926538

// input into vertex shader (configured by vertex array configuration and filled from vertex buffer)
in vec2 in_vertex;

uniform vec2 offset;
uniform vec2 scale;

uniform sampler2D sprites;
uniform int currentDuration;
uniform int fullDuration;
uniform vec2 direction;
uniform int running;

// additional color output for this shader; will be interpolated among the 3 vertices building the trimouseDirAngle

out vec2 v_uvFrameStartTimeStart;
out vec2 v_uvFrameStartTimeEnd;
out vec2 v_uvFrameEndTimeStart;
out vec2 v_uvFrameEndTimeEnd;
out float dFrame;
out float dMove;

// TODO 2.1: add metadata of spriteset

const int framesLineNormal = 58;
const int framesLineRunning = 45;
const int framesCycleNormal = 16;
const int framesCycleRunning = 20;
const int directionsNormal = 16;
const int directionsRunning = 32;




vec2 startNormal;
vec2 startRunning;
ivec2 spriteFrameStartTimeStart;
ivec2 spriteFrameStartTimeEnd;
ivec2 spriteFrameEndTimeStart;
ivec2 spriteFrameEndTimeEnd;

void main()
{

	
    float mouseDirAngle = atan(direction.y/direction.x);
    float revmouseDirAngle = atan(direction.y/-direction.x);
    mouseDirAngle = mix(mouseDirAngle, mouseDirAngle + PI, float(direction.x < 0));
    mouseDirAngle = mix(mouseDirAngle, mouseDirAngle + (2*PI), float(mouseDirAngle < 0));

    int directions = int(mix(directionsNormal, directionsRunning, float(running > 0)));

    float rotationID = (mouseDirAngle/(PI)) * directions / 2;
    int rotationID1 = int(floor(rotationID));
    int rotationID2 = int(floor(rotationID + 1)) % directions;

    dMove = fract(rotationID);

	vec2 animationScale = mix(vec2(0.75, 0.8), vec2(0.95, 0.9), float(running > 0));

    gl_Position = vec4(in_vertex * scale * animationScale  + offset, 0.0, 1.0);

    vec2 norm_vertex = (in_vertex + 1) / 2;			//Coords between 0..1
	ivec2 size = textureSize(sprites,0);
	ivec2 frameSizeNormal = (size - ivec2(0, 2115)) / ivec2(framesLineNormal, 5); //size - untererPart.y
	ivec2 frameSizerunning = (size - ivec2(0, 665)) / ivec2(framesLineRunning, 15); //size - obererPart.y

	startNormal = vec2(0, size.y - frameSizeNormal.y);
	startRunning = vec2(0, 14*frameSizerunning.y);

	ivec2 frameSize= ivec2(mix(frameSizeNormal, frameSizerunning, float(running > 0)));
	vec2 start = mix(startNormal, startRunning, float(running > 0));

	int framesLine = int(mix(framesLineNormal, framesLineRunning, float(running > 0)));
	int framesCycle = int(mix(framesCycleNormal, framesCycleRunning, float(running > 0)));

	float cycleFrame = fract(float(currentDuration) / fullDuration) * framesCycle;
	int cycleFrame1 = int(floor(cycleFrame)) % framesCycle;
	int cycleFrame2 = (cycleFrame1 + 1) % framesCycle;
	
	dFrame = fract(cycleFrame);

	int frameFrameStartTimeStart = cycleFrame1 + (rotationID1 * framesCycle);
	int frameFrameStartTimeEnd = cycleFrame1 + (rotationID2 * framesCycle);
	int frameFrameEndTimeStart = cycleFrame2 + (rotationID1 * framesCycle);
	int frameFrameEndTimeEnd = cycleFrame2 + (rotationID2 * framesCycle);

	spriteFrameStartTimeStart = ivec2(frameFrameStartTimeStart % framesLine, frameFrameStartTimeStart / framesLine);
	spriteFrameStartTimeEnd = ivec2(frameFrameStartTimeEnd % framesLine, frameFrameStartTimeEnd / framesLine);
	spriteFrameEndTimeStart = ivec2(frameFrameEndTimeStart % framesLine, frameFrameEndTimeStart / framesLine);
	spriteFrameEndTimeEnd = ivec2(frameFrameEndTimeEnd % framesLine, frameFrameEndTimeEnd / framesLine);

	v_uvFrameStartTimeStart = (start + ((norm_vertex + vec2(spriteFrameStartTimeStart.x, -spriteFrameStartTimeStart.y)) * frameSize)) / size;
	v_uvFrameStartTimeEnd = (start + ((norm_vertex + vec2(spriteFrameStartTimeEnd.x, -spriteFrameStartTimeEnd.y)) * frameSize)) / size;
	v_uvFrameEndTimeStart = (start + ((norm_vertex + vec2(spriteFrameEndTimeStart.x, -spriteFrameEndTimeStart.y)) * frameSize)) / size;
	v_uvFrameEndTimeEnd = (start + ((norm_vertex + vec2(spriteFrameEndTimeEnd.x, -spriteFrameEndTimeEnd.y)) * frameSize)) / size;

}
