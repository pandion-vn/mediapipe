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

#include <opencv2/opencv.hpp>

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
void DrawFrame(const cv::Mat& frame, int32_t windowWidth, int32_t windowHeight);
GLuint MatToTexture(const cv::Mat &mat, GLenum minFilter, GLenum magFilter, GLenum wrapFilter);
bool InitializeWindowAndDevice(int32_t windowWidth, int32_t windowHeight);

int main(int argc, char** argv)
{
  // Camera capture
  cv::VideoCapture capture;
  capture.open(0);
  if (!capture.isOpened()) {
      std::cout << "Cannot open video: " << std::endl;
      exit(EXIT_FAILURE);
  }
  
  double fps = 0.0;
  fps = capture.get(CV_CAP_PROP_FPS);
  if (fps != fps) { // NaN
      fps = 25.0;
  }

  std::cout << "FPS: " << fps << std::endl;
  
  int32_t windowWidth = 1280;
  int32_t windowHeight = 720;
  InitializeWindowAndDevice(windowWidth, windowHeight);
  InitOpenGL(windowWidth, windowHeight);

  GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);
  static const GLfloat g_vertex_buffer_data[] = { 
		-1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 0.0f,  1.0f, 0.0f,
	};

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

  cv::Mat frame;
  while (DoEvent()) {
    if (!capture.read(frame)) {
      std::cout << "Cannot grab a frame." << std::endl;
      break;
    }

    DrawFrame(frame, windowWidth, windowHeight);

    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
      0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
      3,                  // size
      GL_FLOAT,           // type
      GL_FALSE,           // normalized?
      0,                  // stride
      (void*)0            // array buffer offset
    );
        
    // Draw the triangle !
    glDrawArrays(GL_TRIANGLES, 0, 3); // 3 indices starting at 0 -> 1 triangle

    glDisableVertexAttribArray(0);

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
  if (!glfwInit()) {
    throw "Failed to initialize glfw";
  }
  
  // 	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  // 	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  // 	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  // 	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  glfwWindow = glfwCreateWindow(windowWidth, windowHeight, "OpenGL", nullptr, nullptr);

  if (glfwWindow == nullptr) {
    glfwTerminate();
    throw "Failed to create an window.";
  }

  glfwMakeContextCurrent(glfwWindow);
  glfwSwapInterval(1);

  // Dark blue background
  glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    
  return true;
}

bool DoEvent() {
  if (glfwWindowShouldClose(glfwWindow) == GL_TRUE) {
    return false;
  }

  glfwPollEvents();

  return true;
}

void DrawFrame(const cv::Mat& frame, int32_t windowWidth, int32_t windowHeight) {
  // Clear color and depth buffers
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);     // Operate on model-view matrix

  glEnable(GL_TEXTURE_2D);
  GLuint image_tex = MatToTexture(frame, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP);

  /* Draw a quad */
  glBegin(GL_QUADS);
  glTexCoord2i(0, 0); glVertex2i(0,   0);
  glTexCoord2i(0, 1); glVertex2i(0,   windowHeight);
  glTexCoord2i(1, 1); glVertex2i(windowWidth, windowHeight);
  glTexCoord2i(1, 0); glVertex2i(windowWidth, 0);
  glEnd();

  glDeleteTextures(1, &image_tex);
  glDisable(GL_TEXTURE_2D);
}

GLuint MatToTexture(const cv::Mat &mat, GLenum minFilter, GLenum magFilter, GLenum wrapFilter) {
    // Generate a number for our textureID's unique handle
    GLuint textureID;
    glGenTextures(1, &textureID);

    // Bind to our texture handle
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Catch silly-mistake texture interpolation method for magnification
    if (magFilter == GL_LINEAR_MIPMAP_LINEAR  ||
        magFilter == GL_LINEAR_MIPMAP_NEAREST ||
        magFilter == GL_NEAREST_MIPMAP_LINEAR ||
        magFilter == GL_NEAREST_MIPMAP_NEAREST) {
        cout << "You can't use MIPMAPs for magnification - setting filter to GL_LINEAR" << endl;
        magFilter = GL_LINEAR;
    }

    // Set texture interpolation methods for minification and magnification
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

    // Set texture clamping method
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapFilter);

    // Set incoming texture format to:
    // GL_BGR       for CV_CAP_OPENNI_BGR_IMAGE,
    // GL_LUMINANCE for CV_CAP_OPENNI_DISPARITY_MAP,
    // Work out other mappings as required ( there's a list in comments in main() )
    GLenum inputColourFormat = GL_BGR;
    if (mat.channels() == 1) {
        inputColourFormat = GL_LUMINANCE;
    }

    // Create the texture
    glTexImage2D(GL_TEXTURE_2D,     // Type of texture
                 0,                 // Pyramid level (for mip-mapping) - 0 is the top level
                 GL_RGB,            // Internal colour format to convert to
                 mat.cols,          // Image width  i.e. 640 for Kinect in standard mode
                 mat.rows,          // Image height i.e. 480 for Kinect in standard mode
                 0,                 // Border width in pixels (can either be 1 or 0)
                 inputColourFormat, // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
                 GL_UNSIGNED_BYTE,  // Image data type
                 mat.ptr());        // The actual image data itself

    // If we're using mipmaps then generate them. Note: This requires OpenGL 3.0 or higher
    if (minFilter == GL_LINEAR_MIPMAP_LINEAR  ||
            minFilter == GL_LINEAR_MIPMAP_NEAREST ||
            minFilter == GL_NEAREST_MIPMAP_LINEAR ||
            minFilter == GL_NEAREST_MIPMAP_NEAREST) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    return textureID;
}

void TerminateWindowAndDevice()
{
  if (glfwWindow != nullptr) {
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