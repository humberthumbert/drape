#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Integrator {
 protected:
  /* DATA */
  glm::vec3& position;
  glm::vec3& velocity;

 public:
  Integrator(glm::vec3& pos, glm::vec3& vel) : position(pos), velocity(vel) {}
  virtual ~Integrator() = default;
  virtual void update(glm::vec3 const& force, float delta) = 0;
};

class VerletIntegrator : public Integrator {
 private:
  /* DATA */
  glm::vec3 prev_pos;

 public:
  using Integrator::Integrator;
  virtual ~VerletIntegrator() = default;
  virtual void update(glm::vec3 const& force, float delta) override;
};

class SemiImplicitEulerIntegrator : public Integrator { 
 public:
  using Integrator::Integrator;
  virtual ~SemiImplicitEulerIntegrator() = default;
  virtual void update(glm::vec3 const& force, float delta) override;
};
