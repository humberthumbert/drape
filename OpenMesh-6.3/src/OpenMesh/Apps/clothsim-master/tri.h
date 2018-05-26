#pragma once

#include "scene.h"

#include <glm/glm.hpp>

class Tri {
 private:
   glm::vec3 a; // vertices defined counter-clockwise
   glm::vec3 b;
   glm::vec3 c;
   glm::vec3 normal;
   float offset;

 public:
  Tri() = default;
  Tri(glm::vec3 a, glm::vec3 b, glm::vec3 c) : a(a), b(b), c(c) {
    normal = normalize(cross(c - a, b - a));
    offset = dot(a, normal);
  }
  ~Tri() = default;
  glm::vec3 intersects(glm::vec4& pos, glm::vec4& vel);

  /* GETTERS/SETTERS */
  glm::vec3& get_a();
  glm::vec3& get_b();
  glm::vec3& get_c();

  const glm::vec3& get_a() const;
  const glm::vec3& get_b() const;
  const glm::vec3& get_c() const;

};
