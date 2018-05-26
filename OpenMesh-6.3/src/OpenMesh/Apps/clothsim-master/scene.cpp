#pragma warning(disable : 4786)
#define GL_GLEXT_PROTOTYPES

#include "scene.h"
#include "constants.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

using namespace std;
using namespace glm;

Scene::Scene() {
  gravity = vec3(0, -9.81, 0);
  draw_fill = true;
  draw_nodes = false;
  draw_wire = false;
}

Scene::~Scene() {
  /*
  for (auto i = objects.begin(); i != objects.end(); ++i) {
    delete (*i);
  }
  */
}

void Scene::draw() {
  for (auto i = objects.begin(); i != objects.end(); ++i) {
    (*i)->draw();
  }
}

void Scene::update(float t) {
  for (auto i = objects.begin(); i != objects.end(); ++i) {
    (*i)->update(t);
  }
}

void Scene::add_object(SceneObject* so) {
  so->set_scene(this);
  so->set_model_gravity(gravity);
  objects.push_back(so);
}

void Scene::set_view(vec3 eye, vec3 look, vec3 up) {
  view = glm::lookAt(eye, look, up);
}

void Scene::set_projection(float fovy, float aspect, float near, float far) {
  projection = glm::perspective(radians(fovy), aspect, near, far);
}

void Scene::set_gravity(vec3 dir) {
  gravity = dir;
  for (auto i = objects.begin(); i != objects.end(); ++i) {
    (*i)->set_model_gravity(gravity);
  }
}

void Scene::set_draw_fill(bool b) {
  draw_fill = b;
}

void Scene::set_draw_nodes(bool b) {
  draw_nodes = b;
}

void Scene::set_draw_wire(bool b) {
  draw_wire = b;
}

mat4& Scene::get_view() {
  return view;
}

mat4& Scene::get_projection() {
  return projection;
}

vector<SceneObject*>& Scene::get_objects() {
  return objects;
}

bool Scene::get_draw_fill() const {
  return draw_fill;
}

bool Scene::get_draw_nodes() const {
  return draw_nodes;
}

bool Scene::get_draw_wire() const {
  return draw_wire;
}

const mat4& Scene::get_view() const {
  return view;
}

const mat4& Scene::get_projection() const {
  return projection;
}

const vector<SceneObject*>& Scene::get_objects() const {
  return objects;
}

SceneObject::SceneObject(vec3 pos) {
  position = pos;
  translate(pos.x, pos.y, pos.z);
}

void SceneObject::create_shader_program(ifstream& vertex_input, ifstream& fragment_input) {
  stringstream shader_sstr;
  shader_sstr << vertex_input.rdbuf();
  string vertex_shader_string = shader_sstr.str();
  const char* vertex_shader_data = vertex_shader_string.c_str();
  
  shader_sstr.str(string());
  shader_sstr << fragment_input.rdbuf();
  string fragment_shader_string = shader_sstr.str();
  const char* fragment_shader_data = fragment_shader_string.c_str();
  

  vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_shader_data, NULL);
  glCompileShader(vertex_shader);
  
  GLint success;
  GLchar info_log[512];
  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
    cout << "Error compiling vertex shader" << endl << info_log << endl;
    cout << "Source:" << endl;
    cout << vertex_shader_data << endl;
  }

  fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_shader_data, NULL);
  glCompileShader(fragment_shader);

  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
    cout << "Error compiling fragment shader" << endl << info_log << endl;
    cout << "Source:" << endl;
    cout << fragment_shader_data << endl;
  }

  shader_program = glCreateProgram();
  glAttachShader(shader_program, vertex_shader);
  glAttachShader(shader_program, fragment_shader);
  glLinkProgram(shader_program);
  glUseProgram(shader_program);
}

void SceneObject::get_shader_locations() {
  model_location = glGetUniformLocation(shader_program, "M");
  view_location = glGetUniformLocation(shader_program, "V");
  projection_location = glGetUniformLocation(shader_program, "P");
  color_location = glGetUniformLocation(shader_program, "color");

  position_location = glGetAttribLocation(shader_program, "position");
}

vec3 SceneObject::impulse_response(vec3& pos, vec3& vel) {
  return vec3(0, 0, 0);
}

vec3 SceneObject::penalty_response(vec3& pos, vec3& vel) {
  return vec3(0, 0, 0);
}

void SceneObject::translate(float x, float y, float z) {
  model = model * glm::translate(mat4(), vec3(x, y, z));
  inverse_model = inverse(model);
}

void SceneObject::rotate(vec3 axis, float theta) {
  mat4 trans;
  trans = glm::rotate(trans, radians(theta), axis);
  model = model * trans;
  inverse_model = inverse( model );
}

void SceneObject::scale(float x, float y, float z) {
  model = model * glm::scale(mat4(), vec3(x, y, z));
  inverse_model = inverse(model);
}

void SceneObject::clear_model() {
  model = mat4();
  inverse_model = mat4();
}

mat4& SceneObject::get_model() {
  return model;
}

const mat4& SceneObject::get_model() const {
  return model;
}

mat4& SceneObject::get_inverse_model() {
  return inverse_model;
}

const mat4& SceneObject::get_inverse_model() const {
  return inverse_model;
}


void SceneObject::set_model_gravity(vec3 scene_gravity) {
  vec4 new_gravity = glm::inverse(model) * vec4(scene_gravity, 0.0);
  model_gravity = vec3(new_gravity.x, new_gravity.y, new_gravity.z);
}

vec3& SceneObject::get_model_gravity() {
  return model_gravity;
}

const vec3& SceneObject::get_model_gravity() const {
  return model_gravity;
}

void SceneObject::set_scene(Scene* s) {
  scene = s;
}

Scene* SceneObject::get_scene() {
  return scene;
}

const Scene* SceneObject::get_scene() const {
  return scene;
}


void SceneObject::create_shader_program(int type){
  const GLchar* vertex_shader_data;
  const GLchar* fragment_shader_data;
  if(type == 0){
    vertex_shader_data = {
        "attribute vec3 position;\n"
        "attribute vec3 adjacent_left;\n"
        "attribute vec3 adjacent_down;\n"
        "attribute vec3 adjacent_up;\n"
        "attribute vec3 adjacent_right;\n"
        "varying vec3 normal;\n"
        "uniform mat4 V;\n"
        "uniform mat4 P;\n"
        "void main() {\n"
        "  vec3 left = adjacent_left - position;\n"
        "  vec3 right = adjacent_right - position;\n"
        "  vec3 up = adjacent_up - position;\n"
        "  vec3 down = adjacent_down - position;\n"
        "  vec3 temp; \n"
        "  temp += cross(up, left);\n"
        "  temp += cross(left, down);\n"
        "  temp += cross(down, right);\n"
        "  temp += cross(right, up);\n"
        "  normal = normalize(temp);\n"
        "  gl_Position = P * V * vec4(position, 1.0);\n"
        "}\n"
      };
    fragment_shader_data = {
        "uniform vec3 color;\n"
        "uniform vec3 back_color;\n"
        "varying vec3 normal;\n"
        "void main() {\n"
          "vec3 act_normal = gl_FrontFacing ? normal : -normal;\n"
          "float attenuation = max(dot(act_normal, vec3(0.0, 1.0, 0.0)) , 0.0);\n"
          "attenuation += 0.2;\n"
          "vec3 act_color = gl_FrontFacing ? color : back_color;\n"
          "gl_FragColor = vec4(act_color, 1) * attenuation;\n"
        "}\n"
      };
  } else if (type == 1){//sphere
    vertex_shader_data = {
      "attribute vec3 position;\n"
      "uniform mat4 M;\n"
      "uniform mat4 V;\n"
      "uniform mat4 P;\n"
      "varying vec3 normal;\n"
      "void main() {\n"
        "normal = normalize(position);\n"
        "gl_Position = P * V * M * vec4(position, 1.0);\n"
      "}\n"
    };
    fragment_shader_data = {
      "uniform vec3 color;\n"
      "varying vec3 normal;\n"
      "void main() {\n"
        "float attenuation = max(dot(normal, vec3(0.0, 1.0, 0.0)) , 0.0);\n"
        "attenuation += 0.2;\n"
        "gl_FragColor = vec4(color,1) * attenuation;\n"
      "}\n"
    };
  } else if(type == 2){//triangle
    vertex_shader_data = {
      "attribute vec3 position;\n"
      "uniform mat4 M;\n"
      "uniform mat4 V;\n"
      "uniform mat4 P;\n"
      "void main() {\n"
        "gl_Position = P * V * M * vec4(position, 1.0);\n"
      "}\n"
    };
    fragment_shader_data = {
      "uniform vec3 color;\n"
      "void main() {\n"
        "gl_FragColor = vec4(color, 1);\n"
      "}\n"
    };
  } else if(type == 3){//cube
    vertex_shader_data = {
      "attribute vec3 position;\n"
      "uniform mat4 M;\n"
      "uniform mat4 V;\n"
      "uniform mat4 P;\n"
      "varying vec3 normal;\n"
      "void main() {\n"
        "normal = normalize(position);\n"
        "gl_Position = P * V * M * vec4(position, 1.0);\n"
      "}\n"
    };
    fragment_shader_data = {
      "uniform vec3 color;\n"
      "varying vec3 normal;\n"
      "void main() {\n"
        "float attenuation = max(dot(normal, vec3(0.0, 1.0, 0.0)) , 0.0);\n"
        "attenuation += 0.2;\n"
        "gl_FragColor = vec4(color,1) * attenuation;\n"
      "}\n"
    };
  }

  vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_shader_data, NULL);
  glCompileShader(vertex_shader);
  
  GLint success;
  GLchar info_log[512];
  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
    cout << "Error compiling vertex shader" << endl << info_log << endl;
    cout << "Source:" << endl;
    cout << vertex_shader_data << endl;
  }

  fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_shader_data, NULL);
  glCompileShader(fragment_shader);

  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
    cout << "Error compiling fragment shader" << endl << info_log << endl;
    cout << "Source:" << endl;
    cout << fragment_shader_data << endl;
  }

  shader_program = glCreateProgram();
  glAttachShader(shader_program, vertex_shader);
  glAttachShader(shader_program, fragment_shader);
  glLinkProgram(shader_program);
  glUseProgram(shader_program);
}