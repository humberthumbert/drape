#pragma once

#include "scene.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/gl.h>

class Sphere : public SceneObject {
private:
  /* DATA */
  int resolution;
  float radius;

  virtual void init() override;
  glm::vec3 parametric_sphere(float u, float v);

public:
  Sphere(glm::vec3 pos, float r) : SceneObject(pos) {
    resolution = 40;
    radius = r;
    init();
  }
  virtual ~Sphere() = default;
  virtual void draw() override;
  virtual void update(float t) override;
  virtual glm::vec3 impulse_response(glm::vec3& pos, glm::vec3& vel) override;
  virtual glm::vec3 penalty_response(glm::vec3& pos, glm::vec3& vel) override;
};
