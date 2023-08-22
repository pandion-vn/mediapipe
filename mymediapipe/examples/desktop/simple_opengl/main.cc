#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
using namespace std;

#ifdef __APPLE__

#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#include <OpenGL/gl3ext.h>
#define GLFW_INCLUDE_GLCOREARB

#define glGenVertexArrays glGenVertexArraysAPPLE
#define glBindVertexArray glBindVertexArrayAPPLE
#define glDeleteVertexArrays glDeleteVertexArraysAPPLE

#else

// #include <GL/glew.h>
// #include <GLES3/gl32.h>

#endif  // defined(__APPLE__)

// #define GLFW_INCLUDE_ES2

// #include <opencv2/opencv.hpp>

// I use glfw to easy to write
// 簡単に書くために、glfwを使用します。
// https://www.glfw.org/
#include <GLFW/glfw3.h>

static GLFWwindow* glfwWindow = nullptr;
void InitOpenGL(int w, int h);
bool DoEvent();
void TerminateWindowAndDevice();
void ClearScreen();
void PresentDevice();
bool InitializeWindowAndDevice(int32_t windowWidth, int32_t windowHeight);

int main(int argc, char** argv)
{
	int32_t windowWidth = 1280;
	int32_t windowHeight = 720;
	InitializeWindowAndDevice(windowWidth, windowHeight);
  InitOpenGL(windowWidth, windowHeight);

	while (DoEvent())
	{
		// Ececute functions about DirectX
		// DirectXの処理
		PresentDevice();
		glfwPollEvents();
	}

	// glDeleteBuffers(1, &vertexbuffer);
	// glDeleteVertexArrays(1, &VertexArrayID);
	// glDeleteProgram(programID);

	TerminateWindowAndDevice();

  return 0;
}

void InitOpenGL(int w, int h) {
  glViewport(0, 0, w, h); // use a screen size of WIDTH x HEIGHT

  glMatrixMode(GL_PROJECTION);     // Make a simple 2D projection on the entire window
  glLoadIdentity();
  glOrtho(0.0, w, h, 0.0, 0.0, 100.0);

  glMatrixMode(GL_MODELVIEW);    // Set the matrix mode to object modeling

  glClearColor(100.0f, 0.0f, 0.0f, 0.0f);
  glClearDepth(0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the window
}

bool InitializeWindowAndDevice(int32_t windowWidth, int32_t windowHeight)
{
	// Initialize Window
	// ウインドウの初期化
	if (!glfwInit())
	{
		throw "Failed to initialize glfw";
	}
	
  // 	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  // 	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  // 	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  // 	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindow = glfwCreateWindow(windowWidth, windowHeight, "OpenGL", nullptr, nullptr);

	if (glfwWindow == nullptr)
	{
		glfwTerminate();
		throw "Failed to create an window.";
	}

	glfwMakeContextCurrent(glfwWindow);
	glfwSwapInterval(1);

	// Dark blue background
  glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    
	return true;
}

bool DoEvent()
{
	if (glfwWindowShouldClose(glfwWindow) == GL_TRUE)
	{
		return false;
	}

	glfwPollEvents();

	return true;
}

void TerminateWindowAndDevice()
{
	if (glfwWindow != nullptr)
	{
		glfwDestroyWindow(glfwWindow);
		glfwTerminate();
		glfwWindow = nullptr;
	}
}

void ClearScreen()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void PresentDevice() { glfwSwapBuffers(glfwWindow); }