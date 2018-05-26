#pragma warning(disable : 4786)
#define GL_GLEXT_PROTOTYPES

#include "cloth.h"
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cmath>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;
using namespace Constants::Cloth;
/*
void Cloth::init() {
  vector<vec3> positions(NUM_NODES);
  for (int r = 0; r < HEIGHT; ++r) {
    for (int c = 0; c < WIDTH; ++c) {
      int i = r * WIDTH + c;
      positions[i] = vec3(position.x + c * SPRING_LENGTH_SIDE, position.y, position.z + r * SPRING_LENGTH_SIDE);
      nodes[i] = new ClothNode(this);
      previous_nodes[i] = new ClothNode(*nodes[i]);
      previous_nodes[i]->set_position(positions[i]);
      previous_nodes[i]->set_velocity(vec3(0, 0, 0));
    }
  }

  for (int r = 0; r < HEIGHT; ++r) {
    for (int c = 0; c < WIDTH; ++c) {
      int struct_index = 0;
      int shear_index = 0;
      int bend_index = 0;
      for (int i = -1; i < 2; ++i) {
        for (int j = -1; j < 2; ++j) {
          if (i == 0 && j == 0) { 
            continue; 
          }

          int adjacent_node = (r + i) * WIDTH + (c + j);
          ClothNode* cloth_node;
          if (r + i < 0 || r + i >= HEIGHT || c + j < 0 || c + j >= WIDTH) { 
            cloth_node = nullptr;
          } else { 
            cloth_node = previous_nodes[adjacent_node];
          }

          if ((abs(i) + abs(j)) == 2) {
            nodes[r * WIDTH + c]->get_shear_adjacent()[shear_index++] = cloth_node;
          } else {
            nodes[r * WIDTH + c]->get_struct_adjacent()[struct_index++] = cloth_node;

            int long_adjacent_node = (r + 2 * i) * WIDTH + (c + 2 * j);
            ClothNode* long_cloth_node;
            if (r + 2 * i < 0 || r + 2 * i >= HEIGHT || c + 2 * j < 0 || c + 2 * j >= WIDTH) { 
              long_cloth_node = nullptr;
            } else { 
              long_cloth_node = previous_nodes[long_adjacent_node];
            }

            nodes[r * WIDTH + c]->get_bend_adjacent()[bend_index++] = long_cloth_node;
          }
        }
      }
    }
  }

  int node_vertices_index = 0;
  for (int r = 0; r < HEIGHT - 1; ++r) {
    for (int c = 0; c < WIDTH - 1; ++c) {
      int index_array[6] = {
        r * WIDTH + c,            // top left
        (r + 1) * WIDTH + c,      // bottom left
        r * WIDTH + (c + 1),      // top right
        r * WIDTH + (c + 1),      // top right
        (r + 1) * WIDTH + c,      // bottom left
        (r + 1) * WIDTH + (c + 1) // bottom right
      };
      for (int i = 0; i < 6; ++i) {
        vec3 position = positions[index_array[i]];
        nodes[index_array[i]]->set_position(position);
        for (int j = 0; j < 3; ++j) {
          node_vertices[node_vertices_index] = position[j];
          nodes[index_array[i]]->get_node_vertices_ptrs().push_back(&node_vertices[node_vertices_index]);
          ++node_vertices_index;
        }
      }
    }
  }

  int node_adjacent_vertices_index = 0;
  for (int r = 0; r < HEIGHT - 1; ++r) {
    for (int c = 0; c < WIDTH - 1; ++c) {
      int index_array[6] = {
        r * WIDTH + c,            // top left
        (r + 1) * WIDTH + c,      // bottom left
        r * WIDTH + (c + 1),      // top right
        r * WIDTH + (c + 1),      // top right
        (r + 1) * WIDTH + c,      // bottom left
        (r + 1) * WIDTH + (c + 1) // bottom right
      };
      for (int i = 0; i < 6; ++i) {
        ClothNode** adjacents = nodes[index_array[i]]->get_struct_adjacent();
        for (int k = 0; k < 4; ++k){
          vec3 adj_position;
          if(adjacents[k]) {
            adj_position = adjacents[k]->get_position();
          } else {
            adj_position = nodes[index_array[i]]->get_position();
          }
          for (int j = 0; j < 3; ++j){
            node_adjacent_vertices[node_adjacent_vertices_index] = adj_position[j];
            nodes[index_array[i]]->get_adjacent_vertices_ptrs().push_back(&node_adjacent_vertices[node_adjacent_vertices_index]);

            ++node_adjacent_vertices_index;
          }
        }
      }
    }
  }

  //nodes[WIDTH*(WIDTH-1)]->set_mass(HUGE_VALF);
  //nodes[WIDTH*(WIDTH-1) + (HEIGHT-1)]->set_mass(HUGE_VALF);
  //nodes[0]->set_mass(HUGE_VALF);
  //nodes[HEIGHT-1]->set_mass(HUGE_VALF);

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, NUM_VERTEX_COMPONENTS * sizeof(GLfloat), node_vertices.data(), GL_STREAM_DRAW);

  glGenBuffers(1, &adjacent_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, adjacent_vbo);
  glBufferData(GL_ARRAY_BUFFER, NUM_VERTEX_COMPONENTS * 4 * sizeof(GLfloat), node_adjacent_vertices.data(), GL_STREAM_DRAW);

  create_shader_program(0);
  
  get_shader_locations();

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(position_location);

  glBindBuffer(GL_ARRAY_BUFFER, adjacent_vbo);
  GLint adjacent_location = glGetAttribLocation(shader_program, "adjacent_left");
  glVertexAttribPointer(adjacent_location, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(GLfloat), 0);
  glEnableVertexAttribArray(adjacent_location);

  adjacent_location = glGetAttribLocation(shader_program, "adjacent_down");
  glVertexAttribPointer(adjacent_location, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(GLfloat), (GLvoid*) (3 * sizeof(GLfloat)));
  glEnableVertexAttribArray(adjacent_location);

  adjacent_location = glGetAttribLocation(shader_program, "adjacent_up");
  glVertexAttribPointer(adjacent_location, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(GLfloat), (GLvoid*) (6 * sizeof(GLfloat)));
  glEnableVertexAttribArray(adjacent_location);

  adjacent_location = glGetAttribLocation(shader_program, "adjacent_right");
  glVertexAttribPointer(adjacent_location, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(GLfloat), (GLvoid*) (9 * sizeof(GLfloat)));
  glEnableVertexAttribArray(adjacent_location);

  back_color_location = glGetUniformLocation(shader_program, "back_color");
}
*/
void Cloth::addSpring(std::set<std::string> &springSet,std::string p1str, std::string p2str, ClothNode* p1, ClothNode* p2, int type) { 
  std::string p1p2 = p1str+"|"+p2str, p2p1 = p2str+"|"+p1str;
  if(springSet.find(p1p2) == springSet.end() && springSet.find(p2p1) == springSet.end()){
    springSet.insert(p2p1);
    p1->add_adjacent(p2, type);
    p2->add_adjacent(p1, type);
  }
}

void Cloth::addFaces(ClothNode* c1, ClothNode* c2, ClothNode* c3){
  for(int i = 0; i < 3; i++) node_vertices.push_back(c1->get_position()[i]);
  for(int i = 0; i < 3; i++) node_vertices.push_back(c2->get_position()[i]);
  for(int i = 0; i < 3; i++) node_vertices.push_back(c3->get_position()[i]);
  printf("c1:[%f, %f, %f], \tc2:[%f, %f, %f]\t, c3:[%f, %f, %f]\n\n",
    c1->get_position()[0], c1->get_position()[1], c1->get_position()[2],
    c2->get_position()[0], c2->get_position()[1], c2->get_position()[2],
    c3->get_position()[0], c3->get_position()[1], c3->get_position()[2]
  );
  num_faces++;
}

Cloth::Cloth(glm::vec3 pos, QuadMesh mesh): SceneObject(pos){
  clothMesh = mesh;
  init();
}

void Cloth::init(){
  // creating ClothNodes
  for(QuadMesh::VertexIter v_it=clothMesh.vertices_begin(); v_it!=clothMesh.vertices_end(); ++v_it) {
    ClothNode* cn = new ClothNode(this);
    cn->set_position(vec3(clothMesh.point(*v_it)[0], clothMesh.point(*v_it)[1], clothMesh.point(*v_it)[2]));
    cn->set_velocity(vec3(0, 0, 0));
    nodes.push_back(cn);
    num_nodes++;
  }
  // creating face and structural and shear spring
  std::set<std::string> springSet;
  for(QuadMesh::FaceIter f_it = clothMesh.faces_begin(); f_it != clothMesh.faces_end(); ++f_it) {
    // store the four vertices in a quad
    std::vector<ClothNode*> vertices;
    std::vector<std::string> verticesIdx;
    for (QuadMesh::FaceVertexIter fv_it=clothMesh.fv_iter(*f_it); fv_it.is_valid(); ++fv_it)
    {
      vertices.push_back(nodes[fv_it->idx()]);
      verticesIdx.push_back(fv_it->idx()+"");
    }

    ClothNode* topLeft = vertices[0];
    ClothNode* topRight = vertices[1];
    ClothNode* botRight = vertices[2];
    ClothNode* botLeft = vertices[3];
    // structural spring | use verticesIdx to eliminate duplicated spring
    addSpring(springSet, verticesIdx[0], verticesIdx[1], topLeft, topRight, STRUCT);
    addSpring(springSet, verticesIdx[0], verticesIdx[3], topLeft, botLeft, STRUCT);
    addSpring(springSet, verticesIdx[2], verticesIdx[1], botRight, topRight, STRUCT);
    addSpring(springSet, verticesIdx[2], verticesIdx[3], botRight, botLeft, STRUCT);
    // shear spring
    addSpring(springSet, verticesIdx[0], verticesIdx[2], topLeft, botRight, SHEAR);
    addSpring(springSet, verticesIdx[1], verticesIdx[3], topRight, botLeft, SHEAR);
    
    addFaces(topRight, botLeft, topLeft);
    addFaces(botRight, botLeft, topRight);
  }
    
  // create bending spring
  for (QuadMesh::VertexIter v_it=clothMesh.vertices_sbegin(); v_it!=clothMesh.vertices_end(); ++v_it)
  {
    for (QuadMesh::VertexVertexIter vv_it=clothMesh.vv_iter(*v_it); vv_it.is_valid(); ++vv_it) {
      // for each vertex iterate the secondary neighbor, since one vertex's four cross secondary neighbor has been
      // add as shear spring, only secondary neighbors in the same column or row will be add as bending spring.
      for (QuadMesh::VertexVertexIter v3_it=clothMesh.vv_iter(*vv_it); v3_it.is_valid(); ++v3_it) {
        if(v_it->idx() != v3_it->idx())
          addSpring(springSet, v_it->idx() + "", v3_it->idx() + "", nodes[v_it->idx()], nodes[v3_it->idx()], BEND);
      }
    }
  }
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, num_faces * 3 * sizeof(GLfloat), node_vertices.data(), GL_STREAM_DRAW);

  create_shader_program(0);
  get_shader_locations();

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(position_location);
  
  back_color_location = glGetUniformLocation(shader_program, "back_color");
}

Cloth::~Cloth() {
  for (auto it = nodes.begin(); it != nodes.end(); ++it) {
    delete (*it);
  }
}

int round_up(int num_to_round, int multiple) {
  if (multiple == 0) {
    return num_to_round;
  }

  int remainder = abs(num_to_round) % multiple;
  if (remainder == 0) {
    return num_to_round;
  }

  if (num_to_round < 0) {
    return -(abs(num_to_round) - remainder);
  } else {
    return num_to_round + multiple - remainder;
  }
}

unsigned long long Cloth::shash(vec3& pos) {
  return round_up(pos.x, SPRING_LENGTH_SIDE * 2) + round_up(pos.y, SPRING_LENGTH_SIDE * 2) * 2000 + round_up(pos.z, SPRING_LENGTH_SIDE * 2) * 300000;
}

unordered_map<unsigned long long, vector<ClothNode*> >& Cloth::get_spatial_hash() {
  return spatial_hash;
}

const unordered_map<unsigned long long, vector<ClothNode*> >& Cloth::get_spatial_hash() const {
  return spatial_hash;
}

void Cloth::update(float delta) {
  for (auto i = nodes.begin(); i != nodes.end(); ++i) {
    unsigned long long key = shash((*i)->get_position());
    if (spatial_hash.find(key) == spatial_hash.end()) {
      spatial_hash[key] = vector<ClothNode*>();
    }
    spatial_hash[key].push_back((*i));
  }

  for (auto i = nodes.begin(); i != nodes.end(); ++i) {
    if(i != nodes.begin()) (*i)->update(delta);
  }

  spatial_hash.clear();
  for(int k = 0; k < 3; ++k) {
   for(int i = 0; i < nodes.size(); ++i){
    std::vector<ClothNode*> neighbors = nodes[i]->get_struct_adjacent();
    for(unsigned j = 0; j < neighbors.size(); j++){
      ClothNode* current_cloth_node = nodes[i];
      ClothNode* adjacent_cloth_node = neighbors[j];
      if (isinf(current_cloth_node->get_mass()) && isinf(adjacent_cloth_node->get_mass())) {
        continue; 
      }
      vec3 delta_position = adjacent_cloth_node->get_position() - current_cloth_node->get_position();
      vec3 delta_velocity = adjacent_cloth_node->get_velocity() - current_cloth_node->get_velocity();
      float distance = length(delta_position);
      float max_length;

      max_length = SPRING_LENGTH_SIDE * (1 + CRITICAL_DEFORMATION_RATE);
      if (distance > max_length) {
      //move nodes together
      vec3 max_pos = max_length * normalize(delta_position);
      if (isinf(current_cloth_node->get_mass())) {
        adjacent_cloth_node->add_position(-(delta_position - max_pos));
        adjacent_cloth_node->add_velocity(-(delta_position - max_pos) / delta);
      } else if (isinf(adjacent_cloth_node->get_mass())) {
        current_cloth_node->add_position(delta_position - max_pos);
        current_cloth_node->add_velocity((delta_position - max_pos) / delta);
      } else {
        current_cloth_node->add_position(0.5f * (delta_position - max_pos));
        adjacent_cloth_node->add_position(-0.5f * (delta_position - max_pos));
        current_cloth_node->add_velocity(0.5f * (delta_position - max_pos) / delta);
        adjacent_cloth_node->add_velocity(-0.5f * (delta_position - max_pos) / delta);
        // current_cloth_node->add_velocity(-0.5f * delta_velocity);
        // adjacent_cloth_node->add_velocity(0.5f * delta_velocity);
      }
      // current_cloth_node->attenuate_velocity(DEFORMATION_VELOCITY_ATTENUATION);
      // adjacent_cloth_node->attenuate_velocity(DEFORMATION_VELOCITY_ATTENUATION);
      }
    }
   } 
  }
  for (int k = 0; k < 3; ++k) {
    for (int r = 0; r < HEIGHT; ++r) {
      for (int c = 0; c < WIDTH; ++c) {
        int index = r * WIDTH + c;
        for (int i = -1; i < 2; ++i) {
          for (int j = -1; j < 2; ++j) {
            if (i == 0 && j == 0) {
              continue; 
            }

            int adjacent_node = (r + i) * WIDTH + (c + j);
            if (r + i < 0 || r + i >= HEIGHT || c + j < 0 || c + j >= WIDTH) {
              continue;
            }

            ClothNode* adjacent_cloth_node = nodes[adjacent_node];
            ClothNode* current_cloth_node = nodes[index];

            if (isinf(current_cloth_node->get_mass()) && isinf(adjacent_cloth_node->get_mass())) {
              continue; 
            }

            
          }
        }
      }
    }
  }
  for (int i = 0; i < NUM_NODES; ++i) {
    previous_nodes[i]->set_position(nodes[i]->get_position());
    previous_nodes[i]->set_velocity(nodes[i]->get_velocity());
  }
}

void Cloth::draw_fill() {
  glUniform3f(color_location, 1.0f, 0.2f, 1.0f);
  glUniform3f(back_color_location, 245 / 255.0f, 222 / 256.0f, 179 / 256.0f);
  glDrawArrays(GL_TRIANGLES, 0, NUM_VERTEX_COMPONENTS / 3);
}

void Cloth::draw_nodes() {
  glDisable(GL_DEPTH_TEST);
  glPointSize(3);
  glUniform3f(color_location, 1.0f, 0.0f, 0.0f);
  glUniform3f(back_color_location, 1.0f, 0.0f, 0.0f);
  glDrawArrays(GL_POINTS, 0, NUM_VERTEX_COMPONENTS / 3);
  glEnable(GL_DEPTH_TEST);
}

void Cloth::draw_wired() {
  glUniform3f(color_location, 0.0f, 0.0f, 1.0f);
  glUniform3f(back_color_location, 0.0f, 0.0f, 1.0f);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glDrawArrays(GL_TRIANGLES, 0, NUM_VERTEX_COMPONENTS / 3);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Cloth::draw() {
  // for(int i = 0; i < NUM_VERTEX_COMPONENTS; ++i){
  //   float num = node_vertices.data()[i];
  //   if(num != num){ cout << "nan appeared at " << i << endl; }
  // }

  glBindVertexArray(vao);
  glUseProgram(shader_program);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, NUM_VERTEX_COMPONENTS * sizeof(GLfloat), node_vertices.data(), GL_STREAM_DRAW);
/*
  glBindBuffer(GL_ARRAY_BUFFER, adjacent_vbo);
  glBufferData(GL_ARRAY_BUFFER, NUM_VERTEX_COMPONENTS * 4 * sizeof(GLfloat), node_adjacent_vertices.data(), GL_STREAM_DRAW);
*/
  glUniformMatrix4fv(view_location, 1, GL_FALSE, value_ptr(scene->get_view()));
  glUniformMatrix4fv(projection_location, 1, GL_FALSE, value_ptr(scene->get_projection()));

  if (scene->get_draw_fill()) {
    draw_fill();
  }
  if (scene->get_draw_nodes()) {
    draw_nodes();
  }
  if (scene->get_draw_wire()) {
    draw_wired();
  }
}

vector<ClothNode*>& Cloth::get_nodes() {
  return nodes;
} 

vector<ClothNode*>& Cloth::get_previous_nodes() {
  return previous_nodes;
}

vector<GLfloat>& Cloth::get_node_vertices() {
  return node_vertices;
}

const vector<ClothNode*>& Cloth::get_nodes() const {
  return nodes;
} 

const vector<ClothNode*>& Cloth::get_previous_nodes() const {
  return previous_nodes;
}

const vector<GLfloat>& Cloth::get_node_vertices() const {
  return node_vertices;
}

ClothNode::~ClothNode() {
  delete integrator;
}

ClothNode::ClothNode(Cloth* c) {
  cloth = c;
  mass = 1.0;
  integrator = new VerletIntegrator(position, velocity);
}

ClothNode::ClothNode(ClothNode const& cn) {
  cloth = cn.cloth;
  integrator = cn.integrator;
  position = cn.position;
  velocity = cn.velocity;
  color = cn.color;
  copy(begin(cn.struct_adjacent), end(cn.struct_adjacent), begin(struct_adjacent));
  copy(begin(cn.shear_adjacent), end(cn.shear_adjacent), begin(shear_adjacent));
  copy(begin(cn.bend_adjacent), end(cn.bend_adjacent), begin(bend_adjacent));
  mass = cn.mass;
  node_vertices_ptrs = cn.node_vertices_ptrs;
}

void ClothNode::update(float delta) {
  vector<SceneObject*> const& objects = cloth->get_scene()->get_objects();
  for (auto i = objects.begin(); i != objects.end(); ++i) {
    vec3 response = (*i)->impulse_response(position, velocity);
    velocity += response;
    if (response.x != 0 || response.y != 0 || response.z != 0) {
      velocity *= 0.1f;
      //velocity = vec3(0,0,0);
    }
    // velocity += (*i)->impulse_response(position, velocity);
    // position += (*i)->penalty_response(position, velocity) / 2000.0f;
  }

  vec3 force = get_forces();

  // repulsion for self-intersection
  unsigned long long key = Cloth::shash(position);
  unordered_map<unsigned long long, vector<ClothNode*> >& spatial_hash = cloth->get_spatial_hash();
  vector<ClothNode*>& collision_candidates = spatial_hash[key];
  int my_index = -1;
  int index = 0;
  // check close nodes
  for(auto i = collision_candidates.begin(); i != collision_candidates.end(); ++i){
    if((*i) == this) { 
      my_index = index;
      continue;
    }
    vec3 delta_pos = position - (*i)->position;
    if(length(delta_pos) < SPRING_LENGTH_SIDE * 0.1f) {
      force = delta_pos * 4000.0f;
      //velocity -= mass * ((velocity - (*i)->velocity)) / 2.0f;
      velocity = vec3(0,0,0);
      position -= delta_pos / 2.0f;
    }
    ++index;
  }
  if (my_index != -1) {
    collision_candidates.erase(collision_candidates.begin() + my_index);
    unsigned long long new_key = Cloth::shash(position);
    if (spatial_hash.find(new_key) == spatial_hash.end()) {
      spatial_hash[new_key] = vector<ClothNode*>();
    }
    spatial_hash[new_key].push_back(this);
  }

  integrator->update(force, delta);

  update_node_vertices_ptrs();
}

void ClothNode::update_node_vertices_ptrs() {
  for (int i = 0; i < node_vertices_ptrs.size() / 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      *node_vertices_ptrs[i * 3  + j] = position[j];
    }
  }
  for (int i = 0; i < adjacent_vertices_ptrs.size(); i += 12) {
    for (int k = 0; k < 4; ++k) {
      for (int j = 0; j < 3; ++j) {
        if(struct_adjacent[k]){
          *adjacent_vertices_ptrs[i + k*3 + j] = struct_adjacent[k]->position[j];
        } else {
          *adjacent_vertices_ptrs[i + k*3 + j] = position[j];
        }
      }
    }
  }
}

vec3 ClothNode::get_forces() {
  if (isinf(mass)) {
    return vec3(0,0,0);
  }
  vec3 sum_force;
  for (int i = 0; i < 4; ++i) {
    if (struct_adjacent[i]) {
      sum_force += spring_force(struct_adjacent[i], SPRING_LENGTH_SIDE);
    }
    if (shear_adjacent[i]) {
      sum_force += spring_force(shear_adjacent[i], SPRING_LENGTH_DIAG);
    }
    if (bend_adjacent[i]) {
      sum_force += spring_force(bend_adjacent[i], SPRING_LENGTH_SIDE * 2);
    }
  }
  sum_force += cloth->get_model_gravity();

  vector<SceneObject*> const& objects = cloth->get_scene()->get_objects();
  for (auto i = objects.begin(); i != objects.end(); ++i) {
    vec3 response = (*i)->penalty_response(position, sum_force);
    sum_force += response;
  }

  sum_force -= 0.5f * velocity;

  sum_force /= mass;

  
  return sum_force;
}

vec3 ClothNode::spring_force(ClothNode* cn, float rest_length) {
  vec3 distance = cn->get_position() - position;
  vec3 spring_direction = normalize(distance);

  float delta_x = length(distance) - rest_length;
  float force = delta_x * SPRING_CONSTANT;

  vec3 delta_v = cn->get_velocity() - velocity;
  // float damping = dot(spring_direction, delta_v) * SPRING_DAMPING;

  return (force) * spring_direction;
}

vec3& ClothNode::get_position() {
  return position;
}

vec3& ClothNode::get_velocity() {
  return velocity;
}

vec4& ClothNode::get_color() {
  return color;
}

const vec3& ClothNode::get_position() const {
  return position;
}

const vec3& ClothNode::get_velocity() const {
  return velocity;
}

const vec4& ClothNode::get_color() const {
  return color;
}

float ClothNode::get_mass() const {
  return mass;
}

Cloth* ClothNode::get_cloth() {
  return cloth;
}
/*
ClothNode** ClothNode::get_struct_adjacent() {
  return &struct_adjacent[0];
}

ClothNode** ClothNode::get_shear_adjacent() {
  return &shear_adjacent[0];
}

ClothNode** ClothNode::get_bend_adjacent() {
  return &bend_adjacent[0];
}*/

std::vector<ClothNode*> ClothNode::get_struct_adjacent(){
  return struct_adjacent;
}

std::vector<ClothNode*> ClothNode::get_shear_adjacent(){
  return shear_adjacent;
}

std::vector<ClothNode*> ClothNode::get_bend_adjacent(){
  return bend_adjacent;
}

Integrator* ClothNode::get_integrator() {
  return integrator;
}

vector<GLfloat*>& ClothNode::get_node_vertices_ptrs() {
  return node_vertices_ptrs;
}

vector<GLfloat*>& ClothNode::get_adjacent_vertices_ptrs() {
  return adjacent_vertices_ptrs;
}

const Cloth* ClothNode::get_cloth() const {
  return cloth;
}

const Integrator* ClothNode::get_integrator() const {
  return integrator;
}

const vector<GLfloat*>& ClothNode::get_node_vertices_ptrs() const {
  return node_vertices_ptrs;
}

const vector<GLfloat*>& ClothNode::get_adjacent_vertices_ptrs() const {
  return adjacent_vertices_ptrs;
}

void ClothNode::set_position(vec3 p) {
  position = p;
}

void ClothNode::set_velocity(vec3 v) {
  velocity = v;
}

void ClothNode::set_color(vec4 c) {
  color = c;
}

void ClothNode::set_mass(float m) {
  mass = m;
}

void ClothNode::set_cloth(Cloth* c) {
  cloth = c;
}

void ClothNode::set_integrator(Integrator* i) {
  integrator = i;
}

void ClothNode::add_position(vec3 p) {
  position += p;
}

void ClothNode::add_velocity(vec3 v) {
  velocity += v;
}

void ClothNode::add_adjacent(ClothNode* cn, int type){
  if(type == STRUCT){
    struct_adjacent.push_back(cn);
  } else if(type == SHEAR){
    shear_adjacent.push_back(cn);
  } else if(type == BEND){
    bend_adjacent.push_back(cn);
  }
}
void ClothNode::attenuate_velocity(float factor) {
  velocity *= factor;
}
