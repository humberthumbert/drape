#pragma once

#include <glm/glm.hpp>
#include <GL/gl.h>
#include <string>
#include <vector>

class SceneObject;

class Scene {
 private:
  /* DATA */
  glm::mat4 view;
  glm::mat4 projection;
  glm::vec3 gravity;
  std::vector<SceneObject*> objects;
  bool draw_fill;
  bool draw_nodes;
  bool draw_wire;

 public:
  Scene();
  ~Scene();

  void draw();
  void update(float t);
  void add_object(SceneObject* so);

  /* Getters/Setters */
  glm::mat4& get_view();
  glm::mat4& get_projection();
  std::vector<SceneObject*>& get_objects();

  bool get_draw_fill() const;
  bool get_draw_nodes() const;
  bool get_draw_wire() const;
  const glm::mat4& get_view() const;
  const glm::mat4& get_projection() const;
  const std::vector<SceneObject*>& get_objects() const;

  void set_draw_fill(bool b);
  void set_draw_nodes(bool b);
  void set_draw_wire(bool b);
  void set_view(glm::vec3 eye, glm::vec3 look, glm::vec3 up);
  void set_projection(float fovy, float fovx, float near, float far);
  void set_gravity(glm::vec3 dir);
};

class SceneObject {
 protected:
  Scene* scene;
  glm::mat4 model;
  glm::mat4 inverse_model;
  glm::vec3 model_gravity;
  glm::vec3 position;
  GLuint vao;
  GLuint vbo;
  GLuint shader_program;
  GLuint vertex_shader;
  GLuint fragment_shader;
  GLint model_location;
  GLint view_location;
  GLint projection_location;
  GLint color_location;
  GLint position_location;

  virtual void init() = 0;
  void create_shader_program(int type);
  void create_shader_program(std::ifstream& vertex_input, std::ifstream& fragment_input);
  void get_shader_locations();

 public:
  SceneObject(glm::vec3 pos);
  virtual ~SceneObject() = default;
  virtual void draw() = 0;
  virtual void update(float t) = 0;
  virtual glm::vec3 impulse_response(glm::vec3& pos, glm::vec3& vel);
  virtual glm::vec3 penalty_response(glm::vec3& pos, glm::vec3& vel);
  void translate(float x, float y, float z);
  void rotate(glm::vec3 axis, float theta);
  void scale(float x, float y, float z);
  void clear_model();

  /* Getters/Setters */
  glm::mat4& get_model();
  glm::mat4& get_inverse_model();
  glm::vec3& get_model_gravity();
  Scene* get_scene();

  const glm::mat4& get_model() const;
  const glm::mat4& get_inverse_model() const;
  const glm::vec3& get_model_gravity() const;
  const Scene* get_scene() const;

  void set_model_gravity(glm::vec3 scene_gravity);
  void set_scene(Scene* s);
};
