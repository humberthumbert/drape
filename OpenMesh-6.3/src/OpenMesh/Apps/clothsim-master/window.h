#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "scene.h"

class Window {
 private:
  /* DATA */
  Scene* scene;
  GLFWwindow* window;
  int width;
  int height;

 public:
  Window(int w, int h);
  ~Window();
  void refresh();
  bool shouldClose();
  void getEvents();
  void close();

  /* GETTERS/SETTERS */
  GLFWwindow* get_window();

  const GLFWwindow* get_window() const;

  void set_scene(Scene* s);
};
