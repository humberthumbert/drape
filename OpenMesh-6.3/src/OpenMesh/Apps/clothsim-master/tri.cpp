#include "simulation.h" // must go first :(
#include "tri.h"
#include "constants.h"

#include <iostream>

using namespace std;
using namespace glm;
using namespace Constants::Simulation;

vec3 Tri::intersects(vec4& pos, vec4& vel) {
  float position_dot_normal = dot(vec3(pos.x, pos.y, pos.z), normal);
  float velocity_dot_normal = dot(vec3(vel.x, vel.y, vel.z), normal);

  // parallel, no collision
  if (abs(velocity_dot_normal) < EPSILON) {
    return vec3(0,0,0);
  }

  float t = -(position_dot_normal + offset) / velocity_dot_normal;

  // not colliding in the next frame
  if (t < 0 || t > Simulation::get_current_delta()) {
    return vec3(0,0,0); 
  }

  vec4 bary_pos = pos + (vel * t);

  // test barycentric coordinates
  vec3 v0 = b - a;
  vec3 v1 = c - a;
  vec3 v2 = vec3(bary_pos.x, bary_pos.y, bary_pos.z) - a;

  float dot00 = dot(v0, v0);
  float dot01 = dot(v0, v1);
  float dot02 = dot(v0, v2);
  float dot11 = dot(v1, v1);
  float dot12 = dot(v1, v2);
  
  float inv_denom = 1.0f / (dot00 * dot11 - dot01 * dot01);
  float u = (dot11 * dot02 - dot01 * dot12) * inv_denom;
  float v = (dot00 * dot12 - dot01 * dot02) * inv_denom;

  if(u >= 0 && v >= 0 && u + v <= 1){
    return normal;
  } else {
    return vec3(0,0,0);
  }
}

vec3& Tri::get_a() {
  return a;
}

vec3& Tri::get_b() {
  return b;
}

vec3& Tri::get_c() {
  return c;
}

const vec3& Tri::get_a() const {
  return a;
}

const vec3& Tri::get_b() const {
  return b;
}

const vec3& Tri::get_c() const {
  return c;
}
