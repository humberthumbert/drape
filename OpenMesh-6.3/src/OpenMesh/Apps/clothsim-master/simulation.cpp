#include "simulation.h"
#include "cloth.h"
#include "triangle.h"
#include "sphere.h"
#include "cube.h"
#include "constants.h"

#include <glm/glm.hpp>
#include <chrono>

using namespace glm;
using namespace std;
using namespace Constants::Simulation;

using create_scene_func = void (*)();

//MyMesh mesh;
QuadMesh clothmesh;
//Body* body;
Cloth* cloth = nullptr;
char* clothFile;
char* bodyFile;
// So that we can reset the simulation to the correct initial state.
create_scene_func g_create_scene = &Simulation::create_scene_my_cloth;

Simulation* g_simulation = nullptr;
Cloth* g_cloth = nullptr;

Sphere* g_sphere_1_scene_a = nullptr;
Sphere* g_sphere_2_scene_a = nullptr;

Sphere* g_sphere_1_scene_b = nullptr;

Triangle* g_triangle_1_scene_t = nullptr;

Cube* g_cube_1_scene_c = nullptr;
Sphere* g_sphere_1_scene_c = nullptr;

float current_delta = 0.0;

static void keypress_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  Simulation* s = Simulation::get_instance();
  switch (action) {
    case GLFW_PRESS:
      switch (key) {
        case ' ':
          if (s->get_is_running()) {
            cout << "[Simulation] stopping" << endl;
            s->stop();
          } else {
            s->start();
            cout << "[Simulation] starting" << endl;
          }
          break;
        case 'N':
          if (!s->get_is_running()) {
            cout << "[Simulation] stepping" << endl;
            s->step();
          }
          break;
        case 'R': {
            cout << "[Simulation] resetting" << endl;
            s->reset(); 
            g_create_scene();
            s->get_window()->refresh();
          }
          break;
        case 'W': // up
          if (mods == 1) {
            s->get_camera()->rotate_vertical(VIEW_ROTATE_DELTA);
          } else {
            s->get_camera()->strafe_vertical(VIEW_MOVE_DELTA);
          }
          s->set_view_projection();
          break;
        case 'A': // left
          if (mods == 1) {
            s->get_camera()->rotate_horizontal(VIEW_ROTATE_DELTA);
          } else {
            s->get_camera()->strafe_horizontal(VIEW_MOVE_DELTA);
          }
          s->set_view_projection();
          break;
        case 'D': // right
          if (mods == 1) {
            s->get_camera()->rotate_horizontal(-VIEW_ROTATE_DELTA);
          } else {
            s->get_camera()->strafe_horizontal(-VIEW_MOVE_DELTA);
          }
          s->set_view_projection();
          break;
        case 'S': // down
          if (mods == 1) {
            s->get_camera()->rotate_vertical(-VIEW_ROTATE_DELTA);
          } else {
            s->get_camera()->strafe_vertical(-VIEW_MOVE_DELTA);
          }
          s->set_view_projection();
          break;
        case 'E': // near
          s->get_camera()->strafe_zoom(VIEW_MOVE_DELTA);
          s->set_view_projection();
          break;
        case 'Q': // far
          s->get_camera()->strafe_zoom(-VIEW_MOVE_DELTA);
          s->set_view_projection();
          break;
        case 'L':
          s->get_scene()->set_draw_wire(!s->get_scene()->get_draw_wire());
          break;
        case 'P':
          s->get_scene()->set_draw_nodes(!s->get_scene()->get_draw_nodes());
          break;
        case 'F':
          s->get_scene()->set_draw_fill(!s->get_scene()->get_draw_fill());
          break;
      }
      switch (scancode) {
        case 9:
          s->get_window()->close();
        break;
      }
      break;
    case GLFW_REPEAT:
      switch(key) {
        case 'N':
          if (!s->get_is_running()) {
            cout << "[Simulation] stepping" << endl;
            s->step();
          }
          break;
        case 'W': // up
          if (mods == 1) {
            s->get_camera()->rotate_vertical(VIEW_ROTATE_DELTA);
          } else {
            s->get_camera()->strafe_vertical(VIEW_MOVE_DELTA);
          }
          s->set_view_projection();
          break;
        case 'A': // left
          if (mods == 1) {
            s->get_camera()->rotate_horizontal(VIEW_ROTATE_DELTA);
          } else {
            s->get_camera()->strafe_horizontal(VIEW_MOVE_DELTA);
          }
          s->set_view_projection();
          break;
        case 'D': // right
          if (mods == 1) {
            s->get_camera()->rotate_horizontal(-VIEW_ROTATE_DELTA);
          } else {
            s->get_camera()->strafe_horizontal(-VIEW_MOVE_DELTA);
          }
          s->set_view_projection();
          break;
        case 'S': // down
          if (mods == 1) {
            s->get_camera()->rotate_vertical(-VIEW_ROTATE_DELTA);
          } else {
            s->get_camera()->strafe_vertical(-VIEW_MOVE_DELTA);
          }
          s->set_view_projection();
          break;
        case 'E': // near
          s->get_camera()->strafe_zoom(VIEW_MOVE_DELTA);
          s->set_view_projection();
          break;
        case 'Q': // far
          s->get_camera()->strafe_zoom(-VIEW_MOVE_DELTA);
          s->set_view_projection();
          break;
      }
      break;
    case GLFW_RELEASE:
      break;
  }
}

static void click_callback(GLFWwindow* window, int button, int action, int mods) {
  // to send a ray into scene, ray has position = origin, direction = (x, y, -1.0)
  // take ray and position and transform by reverse MVP to bring to object space
  double xpos, ypos;
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    glfwGetCursorPos(window, &xpos, &ypos);
    cout << "click at (" << xpos << ", " << ypos << ")" << endl;
  }
}

Simulation::Simulation() {
  window = new Window(WINDOW_WIDTH, WINDOW_HEIGHT);
  glfwSetKeyCallback(window->get_window(), keypress_callback);
  glfwSetMouseButtonCallback(window->get_window(), click_callback);

  reset();
}

Simulation::~Simulation() {
  delete scene;
  delete window;
  delete camera;
}

void Simulation::reset() {
  is_running = false;
  scene = new Scene();
  window->set_scene(scene);
  camera = new Camera(vec3(-3.0, 1.0, 2.5),
      vec3(0.0, 0.0, 0.0) ,
      vec3(0.0, 1.0, 0.0));
  camera->set_projection(180.0, 90.0);
  set_view_projection();
}

void Simulation::add_object(SceneObject* obj) {
  scene->add_object(obj);
}

void Simulation::update_scene(float t) {
  scene->update(t);
}

void Simulation::start() {
  is_running = true;
}

void Simulation::stop() {
  is_running = false;
}

bool Simulation::get_is_running() const {
  return is_running;
}

void Simulation::run_simulation() {
  start_time = chrono::high_resolution_clock::now();
  set_view_projection();
  while(!window->shouldClose()) {
    auto now = chrono::high_resolution_clock::now();

    if (is_running) {
      current_delta = chrono::duration_cast<chrono::duration<float>>(now - previous_time).count();
      if (current_delta < MIN_DELTA) {
        continue;
      }
      set_view_projection();
      scene->update(current_delta);
    }

    previous_time = now;
    window->refresh();
    window->getEvents();
  }
}

void Simulation::step() {
  set_view_projection();
  scene->update(MIN_DELTA);
}

void Simulation::set_view_projection() {
  scene->set_view(camera->get_position(),
      camera->get_look_at(),
      camera->get_up());
  scene->set_projection(camera->get_view_angle(), WINDOW_WIDTH/WINDOW_HEIGHT, 1.0, camera->get_view_distance());
}

Camera* Simulation::get_camera() {
  return camera;
}

Scene* Simulation::get_scene() {
  return scene;
}

Window* Simulation::get_window() {
  return window;
}

const Camera* Simulation::get_camera() const {
  return camera;
}

const Scene* Simulation::get_scene() const {
  return scene;
}

const Window* Simulation::get_window() const {
  return window;
}

Simulation* Simulation::get_instance() {
  if (!g_simulation) { 
    g_simulation = new Simulation();
  }
  return g_simulation;
}

void Simulation::delete_instance() {
  if (!g_simulation) {
    return;
  }
  delete g_simulation;
  g_simulation = nullptr;
}

void Simulation::create_scene_a() {
  Simulation* simulation = Simulation::get_instance();

  g_cloth = new Cloth(vec3(-15, 6, -10));
  g_sphere_1_scene_a = new Sphere(vec3(-5, -6, 0), 5);
  g_sphere_2_scene_a = new Sphere(vec3(0, -6, 10), 5);

  simulation->add_object(g_cloth);
  simulation->add_object(g_sphere_1_scene_a);
  simulation->add_object(g_sphere_2_scene_a);
}

void Simulation::create_scene_b() {
  Simulation* simulation = Simulation::get_instance();

  g_cloth = new Cloth(vec3(-15, 4, -10));
  g_sphere_1_scene_b = new Sphere(vec3(-5, -4, -4), 5);

  simulation->add_object(g_cloth);
  simulation->add_object(g_sphere_1_scene_b);
}

void Simulation::create_scene_t() {
  Simulation* simulation = Simulation::get_instance();

  g_cloth = new Cloth(vec3(0, 0, 0));
  g_triangle_1_scene_t = new Triangle();
  
  g_triangle_1_scene_t->translate(5, -5, 5);
  g_triangle_1_scene_t->scale(20, 20, 20);
  g_triangle_1_scene_t->rotate(vec3( 1,0,0 ), -90);

  simulation->add_object(g_cloth);
  simulation->add_object(g_triangle_1_scene_t);
}

void Simulation::create_scene_c() {
  Simulation* simulation = Simulation::get_instance();

  g_cloth = new Cloth(vec3(-15, 6, -10));
  g_cube_1_scene_c = new Cube();
  g_sphere_1_scene_c = new Sphere(vec3(-5, -2, -4), 5);

  g_cube_1_scene_c->translate(0, -20, 0);
  g_cube_1_scene_c->scale(100, 5, 100);

  simulation->add_object(g_cloth);
  simulation->add_object(g_cube_1_scene_c);
  simulation->add_object(g_sphere_1_scene_c);
}

void Simulation::create_scene_s() {
  Simulation* simulation = Simulation::get_instance();

  g_cloth = new Cloth(vec3(-15, 4, -10));

  simulation->add_object(g_cloth);
}

void Simulation::create_scene_my_cloth() {

  Simulation* simulation = Simulation::get_instance();
  //std::cout << clothFile << std::endl << std::endl;
  OpenMesh::IO::Options clothopt;
  if ( ! OpenMesh::IO::read_mesh(clothmesh, clothFile, clothopt)){
    std::cerr << "Error loading cloth mesh fromt file " << clothFile << std::endl;
  }
  
  
  cloth = new Cloth(vec3(-15, 4, -10), clothmesh);

  simulation->add_object(cloth);
}

float Simulation::get_current_delta() {
  return current_delta;
}

int main(int argc, char* argv[]) {
  clothFile = argv[1];
  g_create_scene();
  Simulation::get_instance()->run_simulation();
  Simulation::delete_instance();
  return 0;
}
