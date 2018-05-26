#include "window.h"
#include <iostream>

using namespace std;

Window::Window(int w, int h) {
  width = w;
  height = h;

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  window = glfwCreateWindow(width, height, "ClothSim", nullptr, nullptr);
  glfwMakeContextCurrent(window);

  glewExperimental = GL_TRUE;
  glewInit();
  glEnable(GL_DEPTH_TEST);
}

Window::~Window() {
  glfwTerminate();
}

void Window::refresh() {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  scene->draw();
  glfwSwapBuffers(window);
}

void Window::getEvents() {
  glfwPollEvents();
}

bool Window::shouldClose() {
  return glfwWindowShouldClose(window);
}

void Window::close() {
  glfwSetWindowShouldClose(window, GL_TRUE);
}

GLFWwindow* Window::get_window() {
  return window;
}

const GLFWwindow* Window::get_window() const {
  return window;
}

void Window::set_scene(Scene* s) {
  scene = s;
}
