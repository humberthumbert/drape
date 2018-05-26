#include "integrator.h"
#include "scene.h"

using namespace glm;
using namespace std;

void VerletIntegrator::update(vec3 const& force, float delta) {
  vec3 old_velocity = velocity;
  velocity = velocity + force * delta;
  position = position + (old_velocity + velocity) * 0.5f * delta;
}

void SemiImplicitEulerIntegrator::update(vec3 const& force, float delta ) {
  velocity = velocity + force * delta;
  position = position + delta * velocity;
}
