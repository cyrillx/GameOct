#include "stdafx.h"
#include "NanoWindow.h"
#include "NanoLog.h"
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
//=============================================================================
bool window::Init(uint16_t width, uint16_t height, std::string_view title)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	windowHandle = glfwCreateWindow(width, height, title.data(), NULL, NULL);
	if (!windowHandle)
	{
		Fatal("Failed to create GLFW window");
		return false;
	}
	glfwMakeContextCurrent(windowHandle);
	glfwSetFramebufferSizeCallback(windowHandle, framebuffer_size_callback);
	glfwSetCursorPosCallback(windowHandle, mouse_callback);
	glfwSetScrollCallback(windowHandle, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(windowHandle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	const int openGLVersion = gladLoadGL(glfwGetProcAddress);
	if (openGLVersion < GLAD_MAKE_VERSION(3, 3))
	{
		Fatal("Failed to initialize OpenGL context!");
		return false;
	}

	return true;
}
//=============================================================================
void window::Close() noexcept
{
	if (windowHandle) glfwDestroyWindow(windowHandle);
	windowHandle = nullptr;
	glfwTerminate();
}
//=============================================================================
bool window::WindowShouldClose() noexcept
{
	return glfwWindowShouldClose(windowHandle);
}
//=============================================================================
void window::Swap()
{
	glfwSwapBuffers(windowHandle);
	glfwPollEvents();
}
//=============================================================================