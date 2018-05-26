#pragma warning(disable : 4786)
#define GL_GLEXT_PROTOTYPES

#include "triangle.h"
#include "constants.h"

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cmath>
#include <iostream>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;
using namespace Constants::Simulation;
using namespace Constants::Triangle;

Triangle::~Triangle() {

}

void Triangle::init() {
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glGenBuffers(1, &vbo);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

  ifstream input_vertex(VERTEX_SHADER_FILE);
  ifstream input_fragment(FRAGMENT_SHADER_FILE);

  //create_shader_program(input_vertex, input_fragment);
  create_shader_program(2);
  
  get_shader_locations();

  glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(position_location);

  integrator = new VerletIntegrator(position, velocity); 
}

void Triangle::draw() {
  glBindVertexArray(vao);
  glUseProgram(shader_program);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

  GLint M = glGetUniformLocation(shader_program, "M");
  glUniformMatrix4fv(M, 1, GL_FALSE, value_ptr(get_model()));

  GLint V = glGetUniformLocation(shader_program, "V");
  glUniformMatrix4fv(V, 1, GL_FALSE, value_ptr(scene->get_view()));

  GLint P = glGetUniformLocation(shader_program, "P");
  glUniformMatrix4fv(P, 1, GL_FALSE, value_ptr(scene->get_projection()));

  GLint uniColor = glGetUniformLocation(shader_program, "color");
  glUniform3f(uniColor, 1.0f, 1.0f, 1.0f);

  glDrawArrays(GL_TRIANGLES, 0, 3);
}

void Triangle::update(float delta) {
  // vec3 old_position = position;
  // integrator->update(model_gravity, delta);
  // vec3 delta_position = position - old_position;
  // translate(delta_position[0], delta_position[1], delta_position[2]);
}

glm::vec3 Triangle::impulse_response(glm::vec3& pos, glm::vec3& vel) {  
  //TRIANGLE HAS A NORMAL OF( 0,0,1 ) 
  //Normal Calculation: normalize( cross(B-A , C-A) );

  //convert ray to triangle space
  vec4 in_space_position = inverse_model * vec4(pos, 1);
  vec4 in_space_velocity = inverse_model * vec4(vel, 0);
  
  //parallel, no collision
  if(abs(in_space_velocity[2]) < EPSILON){
    return vec3(0,0,0);
  }

  float t = - ( in_space_position[2] / in_space_velocity[2] );

  //not colliding in the next frame
  if( t < 0 || t > MIN_DELTA ){ return vec3(0,0,0); }

  //second test barycentric coordinates
  vec3 v0 = vec3( vertices[3], vertices[4], vertices[5] ) - vec3( vertices[0], vertices[1], vertices[2] );//B-A
  vec3 v1 = vec3( vertices[6], vertices[7], vertices[8] ) - vec3( vertices[0], vertices[1], vertices[2] );//C-A
  vec3 v2 = vec3(in_space_position.x, in_space_position.y, in_space_position.z) - vec3( vertices[0], vertices[1], vertices[2] );//P-A

  float dot00 = dot(v0, v0);
  float dot01 = dot(v0, v1);
  float dot02 = dot(v0, v2);
  float dot11 = dot(v1, v1);
  float dot12 = dot(v1, v2);
  
  float invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
  float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
  float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

  if( u < 0 || v < 0 || (u + v) > 1) { return vec3(0,0,0); }

  //we have collided, now suggest reponse
  vec3 response = vec3 ( 0, -20*in_space_velocity[1], -2*in_space_velocity[2] );
  vec4 global_response = model * vec4( response, 0 );
  return vec3(global_response.x, global_response.y, global_response.z);
}

vec3 Triangle::penalty_response(vec3& pos, vec3& vel) {
  vec3 pv0 = vec3(vertices[0], vertices[1], vertices[2]) - pos;
  vec3 v0 = vec3( vertices[3], vertices[4], vertices[5] ) - vec3( vertices[0], vertices[1], vertices[2] );//B-A
  vec3 v1 = vec3( vertices[6], vertices[7], vertices[8] ) - vec3( vertices[0], vertices[1], vertices[2] );//C-A
  vec3 normal = cross(v0, v1);
  float distance = dot(pv0, normal);//length(deltaPos);
  if (distance != distance || distance > 0.01f) {
    return vec3(0,0,0);
  }

  vec3 direction = normalize(normal);
  vec3 response = /*(0.01f - distance)*/dot(vel, normal) * direction;
  return response * 20.0f;
}
