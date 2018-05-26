#pragma once

#include "scene.h"
#include "tri.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/gl.h>

class Cube : public SceneObject {
 private:
  /* DATA */
  virtual void init() override;
  Tri tris[12];

 public:
  Cube() : SceneObject(glm::vec3(0, 0, 0)) {
    init();
  }
  virtual void draw() override;
  virtual void update(float t) override;
  virtual glm::vec3 impulse_response(glm::vec3& pos, glm::vec3& vel) override;
  virtual glm::vec3 penalty_response(glm::vec3& pos, glm::vec3& vel) override;
};
