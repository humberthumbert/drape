 #pragma once

#include "scene.h"
#include "integrator.h"
#include "constants.h"

#include <cstdlib>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <set>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

enum direction{LEFT, UP, DOWN, RIGHT};
class ClothNode;

class Cloth : public SceneObject {
 private:
  /* DATA */
  int num_nodes = 0;
  int num_faces = 0;
  QuadMesh clothMesh;
  std::vector<ClothNode*> nodes;
  std::vector<ClothNode*> previous_nodes;
  std::vector<GLfloat> node_vertices;
  std::vector<GLfloat> node_adjacent_vertices;
  GLuint adjacent_vbo;
  GLint back_color_location;
  std::unordered_map<unsigned long long, std::vector<ClothNode*> > spatial_hash;

  virtual void init() override;

 public:
  Cloth(glm::vec3 pos) : SceneObject(pos) {
    nodes.resize(Constants::Cloth::NUM_NODES);
    previous_nodes.resize(Constants::Cloth::NUM_NODES);
    node_vertices.resize(Constants::Cloth::NUM_VERTEX_COMPONENTS);
    node_adjacent_vertices.resize(Constants::Cloth::NUM_VERTEX_COMPONENTS * 4);
    init();
  }
  Cloth(glm::vec3 pos, QuadMesh mesh);// : SceneObject(pos){}

  ~Cloth();
  virtual void draw() override;
  virtual void update(float delta) override;
  void draw_fill();
  void draw_nodes();
  void draw_wired();

  static unsigned long long shash(glm::vec3& pos);

  /* GETTERS/SETTERS */
  std::vector<ClothNode*>& get_nodes();
  std::vector<ClothNode*>& get_previous_nodes();
  std::vector<GLfloat>& get_node_vertices();
  std::unordered_map<unsigned long long, std::vector<ClothNode*> >& get_spatial_hash();

  const std::unordered_map<unsigned long long, std::vector<ClothNode*> >& get_spatial_hash() const;
  const std::vector<ClothNode*>& get_nodes() const;
  const std::vector<ClothNode*>& get_previous_nodes() const;
  const std::vector<GLfloat>& get_node_vertices() const;

  void addFaces(ClothNode* c1, ClothNode* c2, ClothNode* c3);
  void addSpring(std::set<std::string> &springSet,std::string p1str, std::string p2str, ClothNode* p1, ClothNode* p2, int type, direction d);
};

class ClothNode {
 private:
  friend std::ostream& operator << (std::ostream& o, const ClothNode& cn) {
    return o << cn.position.x << " " << cn.position.y << " " << cn.position.z << std::endl;
  }

  /* DATA */
  glm::vec3 position;
  glm::vec3 velocity;
  glm::vec4 color;
  float mass;
  Cloth* cloth;
  Integrator* integrator;
  //ClothNode* struct_adjacent[4]; // struct springs
  //ClothNode* shear_adjacent[4]; // shear springs
  //ClothNode* bend_adjacent[4]; // bend springs
  std::vector<ClothNode*> struct_adjacent;
  std::vector<ClothNode*> shear_adjacent;
  std::vector<ClothNode*> bend_adjacent;
  std::vector<GLfloat*> node_vertices_ptrs;
  std::vector<GLfloat*> adjacent_vertices_ptrs;

  void update_node_vertices_ptrs();
  glm::vec3 get_forces();

 public:
  void update(float delta);
  float distance(ClothNode& c);
  glm::vec3 spring_force(ClothNode* c, float rest_length);
  ClothNode() = default;
  ~ClothNode();
  ClothNode(Cloth* c);
  ClothNode(ClothNode const& cn);

  /* GETTERS/SETTERS */
  glm::vec3& get_position();
  glm::vec3& get_velocity();
  glm::vec4& get_color();
  float get_mass() const;
  Cloth* get_cloth();
  Integrator* get_integrator();
  /*ClothNode** get_struct_adjacent();
  ClothNode** get_shear_adjacent();
  ClothNode** get_bend_adjacent();
  */
  std::vector<ClothNode*> get_struct_adjacent();
  std::vector<ClothNode*> get_shear_adjacent();
  std::vector<ClothNode*> get_bend_adjacent();
  std::vector<GLfloat*>& get_node_vertices_ptrs();
  std::vector<GLfloat*>& get_adjacent_vertices_ptrs();

  const glm::vec3& get_position() const;
  const glm::vec3& get_velocity() const;
  const glm::vec4& get_color() const;
  const Cloth* get_cloth() const;
  const Integrator* get_integrator() const;
  const std::vector<GLfloat*>& get_node_vertices_ptrs() const;
  const std::vector<GLfloat*>& get_adjacent_vertices_ptrs() const;

  void set_position(glm::vec3 p);
  void set_velocity(glm::vec3 v);
  void set_color(glm::vec4 c);
  void set_mass(float m);
  void set_cloth(Cloth* c);
  void set_integrator(Integrator* i);

  void add_position(glm::vec3 p);
  void add_velocity(glm::vec3 v);
  void add_adjacent(ClothNode* cn, int type, direction d);
  void attenuate_velocity(float factor);
};
