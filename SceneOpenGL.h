#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <memory>

class SceneOpenGL
{
public:
	SceneOpenGL() = delete;
	SceneOpenGL(std::string title, GLint w, GLint h);
	~SceneOpenGL();

	GLboolean Initialize();
	GLint Run();

private:
	GLFWwindow* m_window;
	std::string m_title;
	GLint m_width;
	GLint m_height;
};