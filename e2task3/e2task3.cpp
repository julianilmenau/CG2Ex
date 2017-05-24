#include "e2task3.h"

#include <iostream>
#include <string>
#include <vector>
#include <random>

#include <glm/gtc/random.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include <glbinding/gl32core/gl.h>
#include <glbinding/gl32core/enum.h>

#include "common.h"
#include "Icosahedron.h"
#include "Cube.h"
#include "Cylinder.h"


using namespace gl32core;


namespace
{

	template <typename T, std::size_t Count>
	std::vector<T> toVector(const std::array<T, Count> & array)
	{
		return std::vector<T>(array.begin(), array.end());
	}

	template <typename Face>
	void instantiate(const std::vector<glm::vec3> & templateVertices, const std::vector<Face> & templateIndices, int geometryType, int id, std::vector<float> & vertices, std::vector<GLuint> & indices)
	{
		glm::vec3 color = glm::abs(glm::ballRand(1.0f));
		glm::vec3 position = glm::ballRand(10.0f);
		float scale = glm::linearRand(0.01f, 0.05f);
		glm::vec3 rotationVector = glm::sphericalRand(1.0f);
		float rotationAngle = glm::linearRand(0.0f, glm::pi<float>());

		glm::mat4 transform = glm::mat4(1.0f);
		transform = glm::scale(transform, glm::vec3(scale));
		transform = glm::rotate(transform, rotationAngle, rotationVector);
		transform = glm::translate(transform, position);

		const int indexOffset = vertices.size() / 11;

		for (const auto & vertex : templateVertices)
		{
			glm::vec4 projectedVertex = transform * glm::vec4(vertex, 1.0f);
			projectedVertex.x /= projectedVertex.w;
			projectedVertex.y /= projectedVertex.w;
			projectedVertex.z /= projectedVertex.w;

			vertices.emplace_back(projectedVertex.x);
			vertices.emplace_back(projectedVertex.y);
			vertices.emplace_back(projectedVertex.z);
			vertices.emplace_back(color.x);
			vertices.emplace_back(color.y);
			vertices.emplace_back(color.z);
			vertices.emplace_back(vertex.x);
			vertices.emplace_back(vertex.y);
			vertices.emplace_back(vertex.z);
			vertices.emplace_back(*reinterpret_cast<float*>(&id));
			vertices.emplace_back(*reinterpret_cast<float*>(&geometryType));
		}

		for (const auto & face : templateIndices)
		{
			indices.emplace_back(indexOffset + face[0]);
			indices.emplace_back(indexOffset + face[1]);
			indices.emplace_back(indexOffset + face[2]);
		}
	}

} // namespace


e2task3::e2task3()
	: m_vertices(0)
	, m_indices(0)
	, m_program(0)
	, m_vertexShader(0)
	, m_geometryShader(0)
	, m_fragmentShader(0)
	, m_vao(0)
	, m_fbo(0)
	, m_colorTexture(0)
	, m_depthTexture(0)
	, m_width(0)
	, m_height(0)
	, m_indexCount(0)
{
}

void e2task3::initializeGL()
{
	glEnable(GL_DEPTH_TEST);

	// create a rendering program, holding vertex and fragment shader
	m_program = glCreateProgram();

	// create a vertex shader
	m_vertexShader = glCreateShader(GL_VERTEX_SHADER);

	// create a geometry shader
	m_geometryShader = glCreateShader(GL_GEOMETRY_SHADER);

	// create a fragment shader
	m_fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	// attach shaders to program
	glAttachShader(m_program, m_vertexShader);
	glAttachShader(m_program, m_geometryShader);
	glAttachShader(m_program, m_fragmentShader);

	loadShaders();
	generateGeometry();

	// create vertex attribute configuration (the input to vertex shaders)
	glGenVertexArrays(1, &m_vao);

	// bind it since we want to operate on it
	glBindVertexArray(m_vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indices);

	// bind vertex buffer to bnding point "array buffer": needed for vertex attribute configuration
	glBindBuffer(GL_ARRAY_BUFFER, m_vertices);

	// configure the current buffer at GL_ARRAY_BUFFER to be input to the vertex shader, using the vertex interpretation defined here
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 11, 0); // in_vertex
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 11, reinterpret_cast<void*>(3 * sizeof(float))); // in_color
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 11, reinterpret_cast<void*>(6 * sizeof(float))); // in_localCoord
	glVertexAttribIPointer(3, 1, GL_INT, sizeof(float) * 11, reinterpret_cast<void*>(9 * sizeof(float))); // in_id
	glVertexAttribIPointer(4, 1, GL_INT, sizeof(float) * 11, reinterpret_cast<void*>(10 * sizeof(float))); // in_type

																										   // enable previously configured vertex shader input
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// TODO 2.3: Add more framebuffer attachments
	glGenTextures(7, &m_colorTexture);
	//glGenTextures(1, &m_idTexture);
	//glGenTextures(1, &m_depthTexture);


	for (const auto texture : { m_colorTexture, m_depthTexture, m_idTexture,m_localnormalTexture,m_globalnormalTexture,m_localcoordTexture,m_globalcoordTexture })
	{
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(GL_NEAREST));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(GL_NEAREST));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(GL_CLAMP_TO_EDGE));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(GL_CLAMP_TO_EDGE));
	}

	glBindTexture(GL_TEXTURE_2D, m_depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(GL_DEPTH_COMPONENT), m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

	glBindTexture(GL_TEXTURE_2D, m_colorTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(GL_RGB8), m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

	glBindTexture(GL_TEXTURE_2D, m_idTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(GL_RGB8), m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

	glBindTexture(GL_TEXTURE_2D, m_localnormalTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(GL_RGB32F), m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glBindTexture(GL_TEXTURE_2D, m_globalnormalTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(GL_RGB32F), m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);


	glBindTexture(GL_TEXTURE_2D, m_localcoordTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(GL_RGB32F), m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glBindTexture(GL_TEXTURE_2D, m_globalcoordTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(GL_RGB32F), m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

	// TODO 2.3: Initialize added framebuffer attachments

	glBindTexture(GL_TEXTURE_2D, 0);

	// configure framebuffer

	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthTexture, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_colorTexture, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, m_idTexture, 0);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, m_localnormalTexture, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, m_globalnormalTexture, 0);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, m_localcoordTexture, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, m_globalcoordTexture, 0);

	static const GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5 };
	glDrawBuffers(6, drawBuffers);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void e2task3::releaseGL()
{
	// Flag all aquired resources for deletion (hint: driver decides when to actually delete them; see: shared contexts)
	glDeleteBuffers(1, &m_vertices);
	glDeleteBuffers(1, &m_indices);
	glDeleteProgram(m_program);
	glDeleteShader(m_vertexShader);
	glDeleteShader(m_geometryShader);
	glDeleteShader(m_fragmentShader);
	glDeleteVertexArrays(1, &m_vao);

	glDeleteFramebuffers(1, &m_fbo);

	glDeleteTextures(7, &m_colorTexture);

	// TODO 2.3: Free added framebuffer attachments
}

void e2task3::generateGeometry()
{
	std::random_device r;
	std::default_random_engine e1(r());
	std::uniform_int_distribution<int> uniform_dist(20, 30);

	const int numCubes = uniform_dist(e1);
	const int numSpheres = uniform_dist(e1);
	const int numCylinders = uniform_dist(e1);

	auto sphereVertices = toVector(Icosahedron::vertices());
	auto sphereIndices = toVector(Icosahedron::indices());
	Icosahedron::refine(sphereVertices, sphereIndices, 4);

	auto cubeVertices = toVector(Cube::vertices());
	auto cubeIndices = toVector(Cube::indices());

	auto cylinderVertices = Cylinder::vertices(8);
	auto cylinderIndices = Cylinder::indices(8);

	std::vector<float> vertices;
	std::vector<GLuint> indices;

	vertices.reserve(11 * (sphereVertices.size() * numSpheres + cubeVertices.size() * numCubes + cylinderVertices.size() * numCylinders));
	indices.reserve(3 * (sphereIndices.size() * numSpheres + cubeIndices.size() * numCubes + cylinderIndices.size() * numCylinders));

	int id = 1;

	for (auto i = 0; i < numSpheres; ++i)
	{
		instantiate(sphereVertices, sphereIndices, 0, id++, vertices, indices);
	}

	for (auto i = 0; i < numCubes; ++i)
	{
		instantiate(cubeVertices, cubeIndices, 1, id++, vertices, indices);
	}

	for (auto i = 0; i < numCylinders; ++i)
	{
		instantiate(cylinderVertices, cylinderIndices, 2, id++, vertices, indices);
	}

	m_indexCount = indices.size();

	glGenBuffers(1, &m_vertices);
	glGenBuffers(1, &m_indices);

	glBindBuffer(GL_ARRAY_BUFFER, m_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, m_indices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

bool e2task3::loadShaders()
{
	// attach 1 source to vertex shader
	const auto vertexShaderSource = textFromFile("e2task3/e2task3.vert");
	const auto vertexShaderSource_ptr = vertexShaderSource.c_str();
	if (vertexShaderSource_ptr)
		glShaderSource(m_vertexShader, 1, &vertexShaderSource_ptr, 0);

	// compile vertex shader
	glCompileShader(m_vertexShader);

	bool success = checkForCompilationError(m_vertexShader, "vertex shader");

	// attach 1 source to fragment shader
	const auto geometryShaderSource = textFromFile("e2task3/e2task3.geom");
	const auto geometryShaderSource_ptr = geometryShaderSource.c_str();
	if (geometryShaderSource_ptr)
		glShaderSource(m_geometryShader, 1, &geometryShaderSource_ptr, 0);

	// compile fragment shader
	glCompileShader(m_geometryShader);

	success &= checkForCompilationError(m_geometryShader, "geometry shader");

	// attach 1 source to fragment shader
	const auto fragmentShaderSource = textFromFile("e2task3/e2task3.frag");
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

void e2task3::resize(int w, int h)
{
	m_width = w;
	m_height = h;

	if (m_depthTexture)
	{
		glBindTexture(GL_TEXTURE_2D, m_depthTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(GL_DEPTH_COMPONENT), m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		glBindTexture(GL_TEXTURE_2D, m_colorTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(GL_RGB8), m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glBindTexture(GL_TEXTURE_2D, m_idTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(GL_R8), m_width, m_height, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);

		glBindTexture(GL_TEXTURE_2D, m_localnormalTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(GL_RGB32F), m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glBindTexture(GL_TEXTURE_2D, m_globalnormalTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(GL_RGB32F), m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);


		glBindTexture(GL_TEXTURE_2D, m_localcoordTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(GL_RGB32F), m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glBindTexture(GL_TEXTURE_2D, m_globalcoordTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(GL_RGB32F), m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		// TODO 2.3: Resize added framebuffer attachments

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	if (m_program)
	{
		updateUniforms();
	}
}

void e2task3::updateUniforms()
{
	glm::vec3 eye = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::mat4 view = glm::lookAt(eye, glm::vec3(0.0f, 0.0f, 0.0f), up);
	glm::mat3 normal = glm::inverseTranspose(glm::mat3(view));
	glm::mat4 viewProjection = glm::perspectiveFov(glm::radians(30.0f), float(m_width), float(m_height), 0.01f, 2.0f) * view;

	GLint viewProjectionLocation = glGetUniformLocation(m_program, "viewProjection");
	GLint eyeLocation = glGetUniformLocation(m_program, "eye");
	GLint normalMatrixLocation = glGetUniformLocation(m_program, "normalMatrix");

	glUseProgram(m_program);
	glUniform3f(eyeLocation, eye.x, eye.y, eye.z);
	glUniformMatrix4fv(viewProjectionLocation, 1, GL_FALSE, glm::value_ptr(viewProjection));
	glUniformMatrix3fv(normalMatrixLocation, 1, GL_FALSE, glm::value_ptr(normal));
	glUseProgram(0);
}

void e2task3::render()
{
	glViewport(0, 0, m_width, m_height);

	static const GLfloat clearDepth = 1.0f;
	static const GLfloat clearColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	static const GLfloat clearId[] = { 0.0f };
	static const GLfloat clearVec[] = { 0.0f,0.0f,0.0f };

	// TODO 2.3: Clear added framebuffer attachments

	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	// clear

	glClearBufferfi(GL_DEPTH_STENCIL, 0, clearDepth, 0);
	glClearBufferfv(GL_COLOR, 0, clearColor);
	glClearBufferfv(GL_COLOR, 1, clearVec);
	glClearBufferfv(GL_COLOR, 2, clearVec);
	glClearBufferfv(GL_COLOR, 3, clearVec);
	glClearBufferfv(GL_COLOR, 4, clearVec);
	glClearBufferfv(GL_COLOR, 5, clearVec);
	// rasterization

	glBindVertexArray(m_vao);
	glUseProgram(m_program);
	glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, nullptr);
	glUseProgram(0);
	glBindVertexArray(0);

	// blitting

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glDrawBuffer(GL_BACK_LEFT);

	glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void e2task3::printInfos(double x_, double y_)
{
	GLint xNew = static_cast<GLint>(x_), yNew = m_height - static_cast<GLint>(y_);
	GLfloat *  colors = new GLfloat[3];


	std::cout << "Viewportposition: (" << static_cast<int>(xNew) << ", " << static_cast<int>(yNew) << ")" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glReadBuffer(GL_COLOR_ATTACHMENT1);
	glReadPixels(xNew, yNew, 1, 1, GL_RGB, GL_FLOAT, colors);

	std::cout << "Objektnummer: " << (int)(colors[0] * 255) << std::endl;
	std::cout << "Geometrietyp: ";
	switch ((int)(colors[1] * 255)) {
	case 0: {
		std::cout << "Kugel";
		break;
		}
	case 1:	{
		std::cout << "Quader";
		break;
		}
	case 2:	{std::cout << "Zylinder";
		break;
		}
	};
	std::cout << std::endl;

	float * depth = new float[1];
	glReadPixels(xNew, yNew, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, depth);
	std::cout<<"Tiefenwert: "<< depth[0] <<std::endl;
	
	glReadBuffer(GL_COLOR_ATTACHMENT5);
	glReadPixels(xNew, yNew, 1, 1, GL_RGB, GL_FLOAT, colors);
	for (int i = 0; i < 3; i++) {
		colors[i] = (colors[i] * 2) - 1;
	}
	std::cout<<"Position im Weltkoordinatensystem: ("<< colors[0]<<", "<<colors[1]<<", "<<colors[2]<<")"<<std::endl;
	
	glReadBuffer(GL_COLOR_ATTACHMENT3);
	glReadPixels(xNew, yNew, 1, 1, GL_RGB, GL_FLOAT, colors);
	for (int i = 0; i < 3; i++) {
		colors[i] = (colors[i] * 2) - 1;
	}
	std::cout<<"Normale im Weltkoordinatensystem: (" << colors[0] << ", " << colors[1] << ", " << colors[2] << ")"<<std::endl;

	glReadBuffer(GL_COLOR_ATTACHMENT2);
	glReadPixels(xNew, yNew, 1, 1, GL_RGB, GL_FLOAT, colors);
	for (int i = 0; i < 3; i++) {
		colors[i] = (colors[i] * 2) - 1;
	}
	std::cout<<"Position im Objektkoordinatensystem: (" << colors[0] << ", " << colors[1] << ", " << colors[2] << ")"<<std::endl;


	glReadBuffer(GL_COLOR_ATTACHMENT4);
	glReadPixels(xNew, yNew, 1, 1, GL_RGB, GL_FLOAT, colors);
	for (int i = 0; i < 3; i++) {
		colors[i] = (colors[i] * 2.0) - 1.0;
	}
	std::cout<<"Normale im Objektkoordinatensystem: (" << colors[0] << ", " << colors[1] << ", " << colors[2] << ")"<<std::endl;
	
	
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glReadPixels(xNew, yNew, 1, 1, GL_RGB, GL_FLOAT, colors);
	std::cout << "Fragmentfarbe: (" << (int)(colors[0] * 255) << ", " << (int)(colors[1] * 255) << ", " << (int)(colors[2] * 255) << ")" << std::endl;

	std::cout << std::endl;
	// TODO 2.3: Convert x and y to framebuffer lookup coordinates
	// TODO 2.3: Load all required information from framebuffer attachments
	// TODO 2.3: Output to std::cout
}

void e2task3::execute()
{
	render();
}