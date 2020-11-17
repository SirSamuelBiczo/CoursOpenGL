#include "SceneOpenGL.h"
#include "common/Shader.hpp"
#include <array>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
 

SceneOpenGL::SceneOpenGL(std::string title, GLint w, GLint h) :
	m_title{ std::move(title) },
	m_width{ std::move(w) },
	m_height{std::move(h)}
{
	glfwSetErrorCallback([](int code, const char* msg) -> void
		{
			std::cerr << code << " : " << msg << std::endl;
		});
}

SceneOpenGL::~SceneOpenGL()
{
	glfwTerminate();
}

GLboolean SceneOpenGL::Initialize()
{
	if (!glfwInit())
		return GLFW_FALSE;

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	m_window = glfwCreateWindow(m_width, m_height, m_title.data(), nullptr, nullptr);

	if (!m_window)
		return GLFW_FALSE;

	glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int w, int h) -> void
		{
			glViewport(0, 0, w, h);
		});
 
 
	glfwMakeContextCurrent(m_window);
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
		return GL_FALSE;

	return GLFW_TRUE;
}

GLint SceneOpenGL::Run()
{
	GLint code = EXIT_SUCCESS;

	std::array<GLfloat, 108> vertices
	{
		-1.0f,-1.0f,-1.0f, // triangle 1 : begin
		-1.0f,-1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f, // triangle 1 : end
		1.0f, 1.0f,-1.0f, // triangle 2 : begin
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f, // triangle 2 : end
		1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		1.0f,-1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f,-1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f,-1.0f, 1.0f
	};


	Shader shader("shaders/SimpleVertexShader.glsl", "shaders/SimpleFragmentShader.glsl");

	if (!shader.Compile())
		std::cerr << "SHADER::COMPILATION::FAILURE" << std::endl;

	GLuint VAO;
	GLuint VBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), nullptr, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), &vertices[0]);
 
	
	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	glm::mat4 Projection = glm::perspective(glm::radians(70.0f), (float)m_width / (float)m_height, 0.1f, 100.0f);

	// Or, for an ortho camera :
	//glm::mat4 Projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates

	// Camera matrix
	glm::mat4 View = glm::lookAt(
		glm::vec3(4, 3, 1), // Camera is at (4,3,3), in World Space
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);

	// Model matrix : an identity matrix (model will be at the origin)
	glm::mat4 Model = glm::mat4(1.0f);
	// Our ModelViewProjection : multiplication of our 3 matrices
 
	GLint matrixID = glGetUniformLocation(shader.GetProgramID(), "MVP");

	do 
	{
	
		glClearColor(0.3, 0.3, 0.3, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
	
		shader.Use();
		
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(sizeof(vertices)));
		glEnableVertexAttribArray(1);


		glm::mat4 mvp = Projection * View * Model; // Remember, matrix multiplication is the other way around

		glUniformMatrix4fv(matrixID, 1, GL_FALSE, &mvp[0][0]);
 
		glDrawArrays(GL_TRIANGLES, 0, 36);
 
		glBindBuffer(GL_ARRAY_BUFFER, 0);

 
 
		glfwSwapBuffers(m_window);
		glfwPollEvents();

	} while (!glfwWindowShouldClose(m_window) && glfwGetKey(m_window, GLFW_KEY_ESCAPE) != GLFW_PRESS);


	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	return code;
}