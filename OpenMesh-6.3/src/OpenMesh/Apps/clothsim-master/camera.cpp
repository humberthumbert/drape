#include "camera.h"

#include <glm/gtx/rotate_vector.hpp>

using namespace glm;
using namespace std;

void Camera::strafe_horizontal(float delta) {
  vec3 left = cross(up, look_at - position);
  left = normalize(left);

  position += left * delta;
  look_at += left * delta;
}

void Camera::strafe_vertical(float delta) {
  vec3 forward = look_at - position;
  vec3 left = cross(up, forward);
  left = normalize(left);

  vec3 relative_up = cross(forward, left);
  relative_up = normalize(relative_up);

  position += relative_up * delta;
  look_at += relative_up * delta;
}

void Camera::strafe_zoom(float delta) {
  vec3 forward = look_at - position;
  forward = normalize(forward);
  
  position += forward * delta;
  look_at += forward * delta;
}

void Camera::rotate_horizontal(float delta) {
  vec3 forward = look_at - position;
  vec3 left = cross(up, forward);
  vec3 relative_up = cross(forward, left);
  relative_up = normalize(relative_up);
  
  vec3 relative_pos = position - look_at;
  relative_pos = rotate(relative_pos, delta, relative_up);

  position = relative_pos + look_at;
}

void Camera::rotate_vertical(float delta) {
  vec3 forward = look_at - position;
  vec3 left = cross(up, forward);
  left = normalize(left);

  vec3 relative_pos = position - look_at;
  relative_pos = rotate(relative_pos, delta, left);

  position = relative_pos + look_at;
}

void Camera::set_projection(float distance, float angle) {
  view_distance = distance;
  view_angle = angle;
}

vec3& Camera::get_position() {
  return position;
}

vec3& Camera::get_look_at() {
  return look_at;
}

vec3& Camera::get_up() {
  return up;
}

const vec3& Camera::get_position() const {
  return position;
}

const vec3& Camera::get_look_at() const {
  return look_at;
}

const vec3& Camera::get_up() const {
  return up;
}

float Camera::get_view_distance() const {
  return view_distance;
}

float Camera::get_view_angle() const {
  return view_angle;
}
