#include "stdafx.h"
#include "NanoWindow.h"
#include "NanoLog.h"
#include "NanoOpenGLExt.h"
//=============================================================================
namespace
{
	uint16_t width, height;
	uint16_t bufferWidth, bufferHeight;

	bool keys[1024];

	float lastX;
	float lastY;
	float xChange;
	float yChange;
	bool mouseFirstMoved;
}
//=============================================================================
void handleKeysCallback(GLFWwindow* window, int key, int code, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			keys[key] = false;
		}
	}
}
//=============================================================================
void handleMouseCallback(GLFWwindow* window, double xPos, double yPos)
{
	if (mouseFirstMoved)
	{
		lastX = xPos;
		lastY = yPos;
		mouseFirstMoved = false;
	}

	xChange = xPos - lastX;
	yChange = lastY - yPos;

	lastX = xPos;
	lastY = yPos;
}
//=============================================================================
void windowSizeCallback(GLFWwindow* window, int windowWidth, int windowHeight)
{
	width = windowWidth;
	height = windowHeight;
}
//=============================================================================
bool window::Init(uint16_t width, uint16_t height, std::string_view title)
{
	if (!glfwInit())
	{
		Fatal("Error Initialising GLFW");
		return false;
	}
	// Setup GLFW Windows Properties
	// OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Core Profile
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// Allow forward compatiblity
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create the window
	windowHandle = glfwCreateWindow(width, height, title.data(), NULL, NULL);
	if (!windowHandle)
	{
		Fatal("Failed to create GLFW window");
		return false;
	}
	// Get buffer size information
	int bufW, bufH;
	glfwGetFramebufferSize(windowHandle, &bufW, &bufH);
	bufferWidth = bufW;
	bufferHeight = bufH;
	width = bufferWidth;
	height = bufferHeight;

	// Set the current context
	glfwMakeContextCurrent(windowHandle);

	// createCallbacks
	glfwSetKeyCallback(windowHandle, handleKeysCallback);
	glfwSetCursorPosCallback(windowHandle, handleMouseCallback);
	//glfwSetScrollCallback(windowHandle, scrollCallback);
	glfwSetWindowSizeCallback(windowHandle, windowSizeCallback);

	// tell GLFW to capture our mouse
	//glfwSetInputMode(windowHandle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	const int openGLVersion = gladLoadGL(glfwGetProcAddress);
	if (openGLVersion < GLAD_MAKE_VERSION(3, 3))
	{
		Fatal("Failed to initialize OpenGL context!");
		return false;
	}

	ext::Init();

	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
	mouseFirstMoved = true;

	glViewport(0, 0, bufferWidth, bufferHeight);

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
	//glfwPollEvents();
}
//=============================================================================
uint16_t window::GetBufferWidth() { return bufferWidth; }
uint16_t window::GetBufferHeight() { return bufferHeight; }
uint16_t window::GetWidth() { return width; }
uint16_t window::GetHeight() { return height; }
bool* window::GetsKeys() { return keys; }
//=============================================================================
float window::GetXChange()
{
	float theChange = xChange;
	xChange = 0.0f;
	return theChange;
}
//=============================================================================
float window::GetYChange()
{
	float theChange = yChange;
	yChange = 0.0f;
	return theChange;
}
//=============================================================================