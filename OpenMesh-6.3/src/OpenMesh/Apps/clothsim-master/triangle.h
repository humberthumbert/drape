#pragma once

#include "scene.h"
#include "integrator.h"

#include <glm/glm.hpp>
#include <cstdlib>

class Triangle : public SceneObject {
 private:
  Integrator* integrator;
  glm::vec3 velocity;
  float vertices[9] = {
    0.0f,  0.5f, 0.0f, // Vertex 1 (X, Y)
    0.5f, -0.5f, 0.0f, // Vertex 2 (X, Y)
    -0.5f, -0.5f, 0.0f // Vertex 3 (X, Y)
  };
  virtual void init() override;

 public:
  Triangle() : SceneObject(glm::vec3(vertices[0], vertices[1], vertices[2])) {
    init();
  }
  ~Triangle();
  virtual void draw(void) override;
  virtual void update(float t) override;
  virtual glm::vec3 impulse_response(glm::vec3& position, glm::vec3& velocity) override;
  virtual glm::vec3 penalty_response(glm::vec3& position, glm::vec3& velocity) override;
};
