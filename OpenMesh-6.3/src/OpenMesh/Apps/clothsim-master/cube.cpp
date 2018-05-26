#pragma warning(disable : 4786)
#define GL_GLEXT_PROTOTYPES

#include "cube.h"
#include "constants.h"

#include <iostream>
#include <fstream>

using namespace std;
using namespace glm;
using namespace Constants::Cube;

void Cube::init() {
  // looking down on cube
  vec3 t0 = vec3(-1, 1, 1); // top left
  vec3 t1 = vec3(1, 1, 1); // clockwise
  vec3 t2 = vec3(1, -1, 1);
  vec3 t3 = vec3(-1, -1, 1);

  vec3 b0 = vec3(-1, 1, -1);
  vec3 b1 = vec3(1, 1, -1);
  vec3 b2 = vec3(1, -1, -1);
  vec3 b3 = vec3(-1, -1, -1);

  tris[0] = Tri(t0, t3, t1); // top
  tris[1] = Tri(t2, t1, t3);
  tris[2] = Tri(b0, b1, b3); // bottom
  tris[3] = Tri(b2, b3, b1);

  tris[4] = Tri(t3, b3, t2); // front
  tris[5] = Tri(b2, t2, b3);
  tris[6] = Tri(t0, t1, b0); // back
  tris[7] = Tri(b1, b0, t1);

  tris[8] = Tri(t2, b2, t1); // right
  tris[9] = Tri(b1, t1, b2);
  tris[10] = Tri(b0, b3, t0); // left
  tris[11] = Tri(t3, t0, b3);

  vector<GLfloat> points;
  for (int i = 0; i < 12; ++i) {
    vec3 vertex = tris[i].get_a();
    for (int j = 0; j < 3; ++j) {
      points.push_back(vertex[j]);
    }
    vertex = tris[i].get_b();
    for (int j = 0; j < 3; ++j) {
      points.push_back(vertex[j]);
    }
    vertex = tris[i].get_c();
    for (int j = 0; j < 3; ++j) {
      points.push_back(vertex[j]);
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
  create_shader_program(3);
  
  get_shader_locations();

  glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(position_location);
}

void Cube::draw() {
  glBindVertexArray(vao);
  glUseProgram(shader_program);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  glUniformMatrix4fv(model_location, 1, GL_FALSE, value_ptr(model));
  glUniformMatrix4fv(view_location, 1, GL_FALSE, value_ptr(scene->get_view()));
  glUniformMatrix4fv(projection_location, 1, GL_FALSE, value_ptr(scene->get_projection()));

  if(scene->get_draw_fill()) {
    glUniform3f(color_location, 1.0f, 1.0f, 1.0f);
    glPolygonMode(GL_FRONT, GL_FILL);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }

  if(scene->get_draw_wire()) {
    glUniform3f(color_location, 0.0f, 1.0f, 0.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }

  if(scene->get_draw_nodes()) {
    glPointSize(3);
    glUniform3f(color_location, 1.0f, 1.0f, 0.0f);
    glDrawArrays(GL_POINTS, 0, 36);
  }
}

void Cube::update(float t) {

}

vec3 Cube::impulse_response(vec3& pos, vec3& vel) {
  vec4 in_space_position = inverse_model * vec4(pos, 1);
  vec4 in_space_velocity = inverse_model * vec4(vel, 0);

  vec3 resp_dir;
  for (int i = 0; i < 12; ++i) {
    resp_dir = tris[i].intersects(in_space_position, in_space_velocity);
    if(!(resp_dir[0] == 0 && resp_dir[1] == 0 && resp_dir[2] == 0)) {
      break;
    }
  }
  if (resp_dir.x == 0 && resp_dir.y == 0 && resp_dir.z == 0) {
    return vec3(0,0,0);
  }

  // we have collided, now suggest reponse
  vec3 in_space_vel_3 = vec3(in_space_velocity.x, in_space_velocity.y, in_space_velocity.z);
  vec3 response = dot(resp_dir, in_space_vel_3) * (-1.1f) * resp_dir;
  // vec4 global_response = (dot(resp_dir, in_space_vel_3) < 0) ? model * vec4( response, 0) : vec4(0,0,0,0);
  vec4 global_response = model * vec4(response, 0);
  position += response;
  return vec3(global_response.x, global_response.y, global_response.z);
}

vec3 Cube::penalty_response(vec3& pos, vec3& vel) {
  vec4 in_space_position = inverse_model * vec4(pos, 1);
  vec3 response = vec3(0,0,0);
  if (in_space_position[0] >= -1.2 && in_space_position[0] <= 1.2) {
    if (in_space_position[1] >= -1.2 && in_space_position[1] <= 1.2) {
      if (in_space_position[2] >= -1.2 && in_space_position[2] <= 1.2) {
        // inside cube
        if (in_space_position[0] >= in_space_position[1]) {
          if(in_space_position[0] >= in_space_position[2]) {
            response = vec3(1.2-in_space_position[0],0,0);
          } else {
            response = vec3(0,0,1.2-in_space_position[2]);
          }
        } else if (in_space_position[1] > in_space_position[2]) {
          response = vec3(0,1.2-in_space_position[1],0);
        } else {
          response = vec3(0,0,1.2-in_space_position[2]);
        }
      }
    }
  }
  return (response * 120.0f) - ((5.0f * sqrt(120.0f) * dot(response, vel)) * response);
  //return response * 200.0f;

  /*
  vec3 direction = vec3(in_space_position.x, in_space_position.y, in_space_position.z);
  float distance = length(direction);
  if (distance > 1.1f || distance != distance) {
    return vec3(0,0,0);
  }
  direction = normalize(direction);
  return 2000.0f * (1.1f - distance) * direction;
  */
}
