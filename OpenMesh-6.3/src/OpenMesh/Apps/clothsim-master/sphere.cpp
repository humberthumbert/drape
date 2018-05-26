#pragma warning(disable : 4786)
#define GL_GLEXT_PROTOTYPES

#include "sphere.h"
#include "constants.h"

#include <iostream>
#include <fstream>

using namespace glm;
using namespace std;
using namespace Constants::Sphere;
using namespace Constants::Simulation;

vec3 Sphere::parametric_sphere(float u, float v) {
  return vec3(cos(u) * sin(v) * radius,
      cos(v) * radius,
      sin(u) * sin(v) * radius);
}

inline void push_point(vector<GLfloat>& list, vec3 point) {
  for (int i = 0; i < 3; ++i) {
    list.push_back(point[i]);
  }
}

void Sphere::init() {
  vector<GLfloat> points;
  float two_pi = 2.0 * pi<float>();
  for (int i = 0; i < resolution; ++i) {
    for (int j = 0; j < resolution; ++j) {
      float u = (float) i * two_pi / resolution;
      float v = (float) j * two_pi / resolution;

      float un = (float) (i + 1) * two_pi / resolution;
      float vn = (float) (j + 1) * two_pi / resolution;

      vec3 p0 = parametric_sphere(u , v);
      vec3 p1 = parametric_sphere(u , vn);
      vec3 p2 = parametric_sphere(un, v);
      vec3 p3 = parametric_sphere(un, vn);

      // first triangle
      push_point(points, p0);
      push_point(points, p1);
      push_point(points, p2);

      // second triangle
      push_point(points, p2);
      push_point(points, p1);
      push_point(points, p3);
    }
  }

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glGenBuffers(1, &vbo);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(GLfloat), points.data(), GL_DYNAMIC_DRAW);

  ifstream input_vertex(VERTEX_SHADER_FILE);
  ifstream input_fragment(FRAGMENT_SHADER_FILE);
  
  //create_shader_program(input_vertex, input_fragment);
  create_shader_program(1);

  get_shader_locations();

  glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(position_location);
}

void Sphere::draw() {
  glBindVertexArray(vao);
  glUseProgram(shader_program);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  glUniformMatrix4fv(model_location, 1, GL_FALSE, value_ptr(model));
  glUniformMatrix4fv(view_location, 1, GL_FALSE, value_ptr(scene->get_view()));
  glUniformMatrix4fv(projection_location, 1, GL_FALSE, value_ptr(scene->get_projection()));

  if(scene->get_draw_fill()) {
    glUniform3f(color_location, 1.0f, 1.0f, 1.0f);
    glPolygonMode(GL_FRONT, GL_FILL);
    glDrawArrays(GL_TRIANGLES, 0, resolution * resolution * 3);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }

  if(scene->get_draw_wire()){
    glUniform3f(color_location, 0.0f, 1.0f, 0.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawArrays(GL_TRIANGLES, 0, resolution * resolution * 3);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }

  if(scene->get_draw_nodes()){
    glPointSize(3);
    glUniform3f(color_location, 1.0f, 1.0f, 0.0f);
    glDrawArrays(GL_POINTS, 0, resolution * resolution * 3);
  }
}

void Sphere::update(float delta) {

}

vec3 Sphere::impulse_response(vec3& position, vec3& velocity) {
  ///*
  vec3 next_pos = position + velocity * MIN_DELTA;
  vec3 next_delta = next_pos - this->position;
  float distance = length(next_delta);
  if (distance > radius || distance != distance || distance < EPSILON) {
    return vec3(0.0,0.0,0.0);
  }

  vec3 direction = normalize(next_delta);
  vec3 response = (radius - distance) * direction;
  return response * 1.5f;
  
  //return vec3(0,0,0);
}

vec3 Sphere::penalty_response(vec3& pos, vec3& vel) {
  vec3 deltaPos = pos - this->position;

  float distance = length(deltaPos);
  if (distance != distance || distance > radius * 1.1f) {
    return vec3(0,0,0);
  }

  vec3 direction = normalize(deltaPos);
  vec3 response = (radius * 1.1f - distance) * direction;
  return response * 2000.0f;
}
