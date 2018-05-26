#pragma once

#include <chrono>

#include "window.h" // must be declared first here
#include "scene.h"
#include "camera.h"

// -------------------- OpenMesh
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>

class Simulation {
 private:
  /* DATA */
  Scene* scene;
  Window* window;
  bool is_running;
  std::chrono::high_resolution_clock::time_point start_time;
  std::chrono::high_resolution_clock::time_point previous_time;
  Camera* camera;

  Simulation();
  ~Simulation();

 public:
  void start();
  void stop();
  void reset();
  void add_object(SceneObject* obj);
  void update_scene(float t);
  void run_simulation();
  void step();
  static Simulation* get_instance();
  static void delete_instance();
  static void create_scene_a();
  static void create_scene_b();
  static void create_scene_t();
  static void create_scene_c();
  static void create_scene_s();
  static void create_scene_my_cloth();
  static float get_current_delta(); // hacky fix for point-triangle intersection

  /* GETTERS/SETTERS */
  Scene* get_scene();
  Camera* get_camera();
  Window* get_window();
  bool get_is_running() const;

  const Scene* get_scene() const;
  const Camera* get_camera() const;
  const Window* get_window() const;

  void set_view_projection();
};
