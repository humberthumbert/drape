#pragma once

#include <glm/glm.hpp>
#include <GL/gl.h>

class Camera {
private:
  /* DATA */
  glm::vec3 position;
  glm::vec3 look_at;
  glm::vec3 up;
  float view_distance;
  float view_angle;

 public:
  Camera(glm::vec3 p, glm::vec3 l, glm::vec3 u) : position(p), look_at(l), up(u) {}
  void set_projection(float distance, float angle);
  void strafe_horizontal(float delta);
  void strafe_vertical(float delta);
  void strafe_zoom(float delta);
  void rotate_horizontal(float delta);
  void rotate_vertical(float delta);

  /* GETTERS/SETTERS */
  glm::vec3& get_position();
  glm::vec3& get_look_at();
  glm::vec3& get_up();
  float get_view_distance() const;
  float get_view_angle() const;

  const glm::vec3& get_position() const;
  const glm::vec3& get_look_at() const;
  const glm::vec3& get_up() const;
};
