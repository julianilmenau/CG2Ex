#include "e2task4.h"

#include <iostream>
#include <fstream>
#include <ios>
#include <string>
#include <vector>
#include <algorithm>

#pragma warning(push)
#pragma warning(disable : 4201)
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/geometric.hpp>
#pragma warning(pop)

#include <glbinding/gl32ext/gl.h>
#include <glbinding/Meta.h>

#include <bitset>
#include "common.h"


using namespace gl32core;


e2task4::e2task4()
	: m_diffScale(2.f)
	, m_dxtMode(1) // 1: S3TC, 2: ASTC 4x4, 3: ETC2
	, m_zoom(0)
{
}

void e2task4::initializeGL()
{
	// set color used when clearing the frame buffer
	glClearColor(0.12f, 0.14f, 0.18f, 1.0f);

	// create 3 vertices with two components, building a triangle
	// 2D-restricted normalized device coordinates are used ([-1,1]^2)
	float vertices[] = { 1.f, -1.f, 1.f, 1.f, -1.f, -1.f, -1.f, 1.f };

	// register one buffer for vertices on the GPU
	glGenBuffers(1, &m_vbo);

	// bind buffer to binding point "array buffer"; also needed for vertex attribute configuration
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	// load geometry data to graphics memory
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * sizeof(vertices), vertices, GL_STATIC_DRAW);

	// optional cleanup: release "array buffer" binding point
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// create a rendering program, holding vertex and fragment shader
	m_program = glCreateProgram();

	// create a vertex shader
	m_vertexShader = glCreateShader(GL_VERTEX_SHADER);

	// create a fragment shader
	m_fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	// attach shaders to program
	glAttachShader(m_program, m_vertexShader);
	glAttachShader(m_program, m_fragmentShader);

	loadShaders();

	// create vertex attribute configuration (the input to vertex shaders)
	glGenVertexArrays(1, &m_vao);

	// bind it since we want to operate on it
	glBindVertexArray(m_vao);

	// bind vertex buffer to bnding point "array buffer": needed for vertex attribute configuration
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	// configure the current buffer at GL_ARRAY_BUFFER to be input to the vertex shader, using the vertex interpretation defined here
	// read two floats, use them for vertex shader input 0 and move 2 floats forward for the next vertex
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

	// enable previously configured vertex shader input
	glEnableVertexAttribArray(0);

	// create a texture 
	glGenTextures(static_cast<GLsizei>(m_textures.size()), m_textures.data());

	loadTextures();

	// optional: bind the fragment shader output 0 to "out_color", which is 0 by default
	glBindFragDataLocation(m_program, 0, "out_color");
}

void e2task4::releaseGL()
{
	// Flag all aquired resources for deletion (hint: driver decides when to actually delete them; see: shared contexts)
	glDeleteBuffers(1, &m_vbo);
	glDeleteProgram(m_program);
	glDeleteShader(m_vertexShader);
	glDeleteShader(m_fragmentShader);
	glDeleteVertexArrays(1, &m_vao);
	glDeleteTextures(static_cast<GLsizei>(m_textures.size()), m_textures.data());
}

bool e2task4::loadShaders()
{
	// attach 1 source to vertex shader
	const auto vertexShaderSource = textFromFile(/*"e2task4*/"e2task4.vert");
	const auto vertexShaderSource_ptr = vertexShaderSource.c_str();
	if (vertexShaderSource_ptr)
		glShaderSource(m_vertexShader, 1, &vertexShaderSource_ptr, 0);

	// compile vertex shader
	glCompileShader(m_vertexShader);

	bool success = checkForCompilationError(m_vertexShader, "vertex shader");

	// attach 1 source to fragment shader
	const auto fragmentShaderSource = textFromFile(/*"e2task4*/"e2task4.frag");
	const auto fragmentShaderSource_ptr = fragmentShaderSource.c_str();
	if (fragmentShaderSource_ptr)
		glShaderSource(m_fragmentShader, 1, &fragmentShaderSource_ptr, 0);

	// compile fragment shader
	glCompileShader(m_fragmentShader);

	success &= checkForCompilationError(m_fragmentShader, "fragment shader");

	if (!success)
	{
		return false;
	}

	// link program (confer to standard program compilation process: [tokenizer, lexer,] compiler, linker)
	glLinkProgram(m_program);

	success &= checkForLinkerError(m_program, "program");

	if (!success)
	{
		return false;
	}

	updateUniforms();

	return true;
}

// configure required min/mag filter and wrap modes
void e2task4::setupTexture(const GLenum filter, const GLenum wrap)
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(filter));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(filter));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(wrap));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(wrap));
}

void e2task4::analyzeTexture(const std::string & identifier, const size_t size)
{
	std::cout << std::endl << identifier << ":" << std::endl;
	std::cout << "  Size: " << humanFileSize(size) << std::endl;
}

void e2task4::analyzeTexture(const std::string & identifier
	, const std::vector<glm::vec4> & I, const nano & duration)
{
	auto sizeCompressed = static_cast<GLint>(0);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &sizeCompressed);

	auto w = static_cast<GLint>(0);
	auto h = static_cast<GLint>(0);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);

	auto K = std::vector<glm::vec4>(w * h);
	//glGetCompressedTexImage(GL_TEXTURE_2D, 0, K.data());
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, K.data());

	std::cout << std::endl << identifier << ":" << std::endl;
	std::cout << "    Size: " << humanFileSize(sizeCompressed) << std::endl;
	std::cout << "     MSE: " << std::fixed << std::setprecision(2) << mse(I, K, 255.f) << std::endl;
	std::cout << "    PSNR: " << std::fixed << std::setprecision(2) << psnr(I, K, 255.f) << "dB" << std::endl;
	std::cout << " EncTime: " << std::fixed << std::setprecision(2)
		<< std::chrono::duration<double, std::milli>(duration).count() << "ms" << std::endl;
}

void e2task4::setupCompressed(
	const GLint texture
	, const std::vector<char> & raw
	, const int width
	, const int height
	, const GLenum format
	, const std::vector<GLenum> & formats
	, const std::vector<glm::vec4> & If
	, const bool bypassGraphicsDriverEncoding
	, const std::string & fileName
	, const bool writeToFile)
{
	const auto formatSupported = std::find(formats.cbegin(), formats.cend(), format) != formats.cend();
	const auto formatString = glbinding::Meta::getString(format);

	glBindTexture(GL_TEXTURE_2D, texture);

	if (formatSupported && !bypassGraphicsDriverEncoding)
	{
		auto start = std::chrono::steady_clock::now();

		glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(format), width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, raw.data());
		glFinish();

		auto end = std::chrono::steady_clock::now();
		auto duration = end - start;

		analyzeTexture(formatString, If, duration);
	}
	else if (!bypassGraphicsDriverEncoding)
		std::cerr << std::endl << formatString << std::endl << "  NOT SUPPORTED" << std::endl;


	if (bypassGraphicsDriverEncoding)
	{
		auto R = rawFromFile(fileName.c_str());
		glCompressedTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, static_cast<GLsizei>(R.size()), R.data());

		analyzeTexture(formatString + " BYPASSED", If, nano());
	}
	else if (writeToFile)
	{
		auto sizeCompressed = static_cast<GLint>(0);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &sizeCompressed);

		auto R = std::vector<char>(sizeCompressed);
		glGetCompressedTexImage(GL_TEXTURE_2D, 0, R.data());

		auto file = std::ofstream(fileName.c_str(), std::ios::out | std::ios::binary);
		file.write(reinterpret_cast<char*>(R.data()), R.size());
		file.close();
	}

	setupTexture();
}

bool e2task4::loadTextures()
{
	auto numFormats = 0;
	glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS, &numFormats);

	auto formats = std::vector<GLenum>(numFormats);
	glGetIntegerv(GL_COMPRESSED_TEXTURE_FORMATS, reinterpret_cast<gl::GLint*>(formats.data()));

	glBindTexture(GL_TEXTURE_2D, m_textures[0]);

	// this is used for all file names (below)
	auto texture = std::string(/*e2task4*/"cat");            // 128x128 and 1024x1024
															 //auto texture = std::string("e2task4/test");           // 16x16

															 // from the texture pack
															 //auto texture = std::string("e2task4/totoro");         // 512x512
															 //auto texture = std::string("e2task4/colorfulnight");  // 512x512
															 //auto texture = std::string("e2task4/insidethegreen"); // 2048x2048
															 //auto texture = std::string("e2task4/wahyugiraffe");   // 2048x2048
															 //auto texture = std::string("e2task4/heic1501a");      // 4096x4096
															 //auto texture = std::string("e2task4/unreal4");        // 4096x4096
															 //auto texture = std::string("e2task4/gta5map");        // 8192x8192
															 //auto texture = std::string("e2task4/nomansky");       // 8192x8192

	static const auto w = 128;
	static const auto h = 128;

	// SUGGESTED WORKFLOW/USAGE:
	// (1) try without bypass (false, false): in this mode the time is measured for comparison with your encoder
	// (2) for those that are not supported or crash enable bypass (true, false)

	static const auto bypass = true;
	static const auto toFile = false;


	texture.append("." + std::to_string(w) + "." + std::to_string(h));


	// Gather comparison data for analyze (pnsr and mse etc).

	auto raw = rawFromFile((texture + ".rgba.ub.raw").c_str());

	// allocate memory on GPU for the texture (can also be used to upload actual texel data using the last parameter)
	glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(GL_RGBA8), w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, raw.data());
	setupTexture();
	analyzeTexture("GL_RGBA8", raw.size());

	auto If = std::vector<glm::vec4>(w * h);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, If.data());
	auto Iu = std::vector<std::uint8_t>(w * h * 4);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, Iu.data());


	// Measure and setup preset encodings.

	// the first boolean parameter bypasses driver conversion and loads image from file with the given fileName
	// the second boolean causes the compressed image to be written into a binary file with the given fileName

	setupCompressed(m_textures[1], raw, w, h
		, gl32ext::GL_COMPRESSED_RGB_S3TC_DXT1_EXT, formats, If
		, bypass, texture + ".dxt1.raw", toFile);

	setupCompressed(m_textures[2], raw, w, h
		, gl32ext::GL_COMPRESSED_RGBA_ASTC_4x4_KHR, formats, If
		, bypass, texture + ".astc4x4.raw", toFile);

	setupCompressed(m_textures[3], raw, w, h
		, gl32ext::GL_COMPRESSED_RGBA8_ETC2_EAC, formats, If
		, bypass, texture + ".etc2.raw", toFile);


	// Measure and setup custom encoding.

	glBindTexture(GL_TEXTURE_2D, m_textures[4]);

	auto start = std::chrono::steady_clock::now();

	const auto Eu = encode(Iu, w, h);
	glCompressedTexImage2D(GL_TEXTURE_2D, 0, gl32ext::GL_COMPRESSED_RGB_S3TC_DXT1_EXT, w, h, 0, static_cast<GLsizei>(Eu.size()), Eu.data());
	glFinish();

	auto end = std::chrono::steady_clock::now();
	auto duration = end - start;

	setupTexture();
	analyzeTexture("Exercise 2 Task 4: Custom S3TC (DXT1)", If, duration);

	glBindTexture(GL_TEXTURE_2D, 0);
	return true;
}

void e2task4::updateUniforms()
{
	glUseProgram(m_program);

	m_offsetLocation = glGetUniformLocation(m_program, "offset");
	m_scaleLocation = glGetUniformLocation(m_program, "scale");

	m_catunLocation = glGetUniformLocation(m_program, "catun");
	m_catcoLocation = glGetUniformLocation(m_program, "catco");
	m_diffScaleLocation = glGetUniformLocation(m_program, "diffScale");
	m_zoomLocation = glGetUniformLocation(m_program, "zoom");

	m_programLocation = glGetUniformLocation(m_program, "program");
	glUniform1f(m_diffScaleLocation, m_diffScale);

	glUseProgram(0);
}

void e2task4::resize(const int w, const int h)
{
	m_width = w;
	m_height = h;
}

void e2task4::diffScale(const float scale)
{
	m_diffScale = std::max(0.f, scale);

	glUseProgram(m_program);
	glUniform1f(m_diffScaleLocation, m_diffScale);
	glUseProgram(0);
}

void e2task4::dxtMode(const int mode)
{
	m_dxtMode = mode;
}

void e2task4::zoom(const int level)
{
	m_zoom = std::max(0, level);

	glUseProgram(m_program);
	glUniform1i(m_zoomLocation, m_zoom);
	glUseProgram(0);
}

float e2task4::diffScale() const
{
	return static_cast<float>(m_diffScale);
}


void e2task4::render()
{
	// Define the area for the rasterizer that is used for the NDC mapping ([-1, 1]^2 x [0, 1])
	glViewport(0, 0, m_width, m_height);

	// clear offscreen-framebuffer color attachment (no depth attachment configured and thus omitting GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT);

	// bind vertex array object to restore vertex shader input configuration
	glBindVertexArray(m_vao);

	// bind program and textures for use in next draw calls
	glUseProgram(m_program);

	glUniform1i(m_catunLocation, 0);
	glUniform1i(m_catcoLocation, 1);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	static const auto midpoints = cellMidpoints(3, 2, glm::vec4(-1.f, -1.f, 1.f, 1.f));
	const auto scale = cellScale(3, 2, glm::vec4(-1.f, -1.f, 1.f, 1.f), m_width, m_height, 0.1f) * 0.5f;
	glUniform2f(m_scaleLocation, scale.x, scale.y);

	// OPENGL ENCODER

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_textures[0]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_textures[m_dxtMode]);

	for (auto i = 0; i < 3; ++i)
	{
		glUniform2f(m_offsetLocation, midpoints[i + 3].x, midpoints[i + 3].y);
		glUniform1i(m_programLocation, i);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

	// CUSTOM ENCODER

	glBindTexture(GL_TEXTURE_2D, m_textures[4]);
	for (auto i = 0; i < 3; ++i)
	{
		glUniform2f(m_offsetLocation, midpoints[i + 0].x, midpoints[i + 0].y);
		glUniform1i(m_programLocation, i);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

	glDisable(GL_BLEND);

	// cleanup: release currently used program and textures
	glUseProgram(0);

	glBindTexture(GL_TEXTURE_2D, 0);

	// cleanup: release currently used vertex array
	glBindVertexArray(0);
}

void e2task4::execute()
{
	render();
}


// ToDo (e2task4): write S3TC DXT1 encoder here...
// Refer to line 256ff for changing the texture (download imagepack via moodle...)

std::vector<std::uint8_t> e2task4::encode(const std::vector<std::uint8_t> & I, int width, int height)
{
	auto E = std::vector<std::uint8_t>(width / 4 * height / 4 * 8);
	// encode I into E ... format should be GL_COMPRESSED_RGB_S3TC_DXT1_EXT
	// https://www.opengl.org/wiki/S3_Texture_Compression ...
	union bit64 {
		uint64_t big;
		uint8_t small[8];
	};
	std::vector<bit64> blocks;
	for (int x = 0; x < width / 4; x++) {
		for (int y = 0; y < height / 4; y++) {
			uint64_t block = 0;
			
			int dmax = 0;
			int maxbxy1 = 0;
			int maxbxy2 = 0;
			for (int bxy1 = 0; bxy1 < 16; bxy1++) {
			std::vector<std::uint8_t> cur1;
			for (int i = 0; i < 4; i++) {
			std::uint8_t n = I[4*(bxy1%4+4*y + (bxy1/4+4*x)*width) + i];
			cur1.push_back(n);
			}
			for (int bxy2 = 0; bxy2 < 16; bxy2++) {
			std::vector<std::uint8_t> cur2;
			for (int i = 0; i < 4; i++) {
			std::uint8_t n = I[4*(bxy2%4+4*y + (bxy2/4+4*x)*width) + i];
			cur2.push_back(n);
			}
			int d = 0;
			for (int i = 0; i < 4; i++) {
			d += abs(cur1[i] - cur2[i]);
			}
			if (d > dmax) {
			dmax = d;
			maxbxy1 = bxy1;
			maxbxy2 = bxy2;
			}
			}
			}
			int sum1 = 0;
			int sum2 = 0;
			for (int i = 0; i < 4; i++) {
			sum1 += I[4*(maxbxy1%4+4*y + (maxbxy1/4+4*x)*width) + i];
			sum2 += I[4*(maxbxy2%4+4*y + (maxbxy2/4+4*x)*width) + i];
			}
			if (sum1 > sum2) {
			int tmp = maxbxy1;
			maxbxy1 = maxbxy2;
			maxbxy2 = tmp;
			}
			std::vector<std::uint8_t> max;
			for (int i = 0; i < 4; i++) {
			std::uint8_t n = I[4*(maxbxy1%4+4*y + (maxbxy1/4+4*x)*width) + i];
			max.push_back(n);
			}
			std::vector<std::uint8_t> min;
			for (int i = 0; i < 4; i++) {
			std::uint8_t n = I[4*(maxbxy2%4+4*y + (maxbxy2/4+4*x)*width) + i];
			min.push_back(n);
			}
			std::vector<std::uint8_t> c0 = max;
			std::vector<std::uint8_t> c1 = min;
			std::vector<std::uint8_t> c2(4);
			std::vector<std::uint8_t> c3(4);

			for (int i = 0; i < 4; i++) {
				c2[i] = (2 * c0[i] + c1[i]) / 3;
				c3[i] = ((c0[i] + 2 * c1[i]) / 3);
			}
			for (int blocky = 3; blocky >= 0; blocky--) {
				for (int blockx = 3; blockx >= 0; blockx--) {
					std::vector<unsigned int> d = { 0, 0, 0, 0 };

					for (int i = 0; i < 4; i++) {


						std::uint8_t n = I[4 * (blockx + 4 * y + (blocky + 4 * x)*width) + i];

						d[0] += abs(n - c0[i]);
						d[1] += abs(n - c1[i]);
						d[2] += abs(n - c2[i]);
						d[3] += abs(n - c3[i]);
					}
					unsigned int imin = 0;
					unsigned int min = 255;
					for (int i = 0; i < 4; i++) {
						if (d[i] < min) {
							min = d[i];
							imin = i;
						}
					}
					block += imin;
					if (!(blockx == 0 && blocky == 0)) {
						block = block << 2;
					}
				}
			}
			unsigned int maxr = floor(max[0] / 8);
			unsigned int maxg = floor(max[1] / 4);
			unsigned int maxb = floor(max[2] / 8);
			unsigned int minr = floor(min[0] / 8);
			unsigned int ming = floor(min[1] / 4);
			unsigned int minb = floor(min[2] / 8);
			uint8_t c0_low(0), c0_hi(0), c1_low(0), c1_hi(0);

			c0_hi = maxr;
			c0_hi = c0_hi << 3;
			c0_hi += maxg / 8;
			c0_low = maxg % 8;
			c0_low = c0_low << 5;
			c0_low += maxb;

			c1_hi = minr;
			c1_hi = c1_hi << 3;
			c1_hi += ming / 8;
			c1_low = ming % 8;
			c1_low = c1_low << 5;
			c1_low += minb;

			block = block << 8;
			block += c1_hi;
			block = block << 8;
			block += c1_low;
			block = block << 8;
			block += c0_hi;
			block = block << 8;
			block += c0_low;

			bit64 tmp;
			tmp.big = block;
			blocks.push_back(tmp);
			
		}
	}
	for (int o = 0; o<blocks.size(); o++) {
		for (int u = 0; u<8; u++) {
			E[o * 8 + u] = blocks[o].small[u]; 
		}
	}
	
	return E;
}
