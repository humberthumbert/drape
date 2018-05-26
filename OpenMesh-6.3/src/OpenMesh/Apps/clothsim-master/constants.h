#pragma once
//------- OpenMesh
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
typedef OpenMesh::TriMesh_ArrayKernelT<>  MyMesh;
typedef OpenMesh::PolyMesh_ArrayKernelT<>  QuadMesh;
#define STRUCT 0
#define SHEAR 1
#define BEND 2
//#define SHADER_DIR "~/drape/OpenMesh-6.3/src/OpenMesh/Apps/clothsim-master/shaders/"
#define SHADER_DIR "./shaders/"
namespace Constants {

  namespace Cloth {
    static constexpr float SPRING_LENGTH_SIDE = 1.0;
    static constexpr float SPRING_LENGTH_DIAG = sqrt(2) * SPRING_LENGTH_SIDE;
    static constexpr float SPRING_CONSTANT = 10;
    static constexpr float SPRING_DAMPING = 2.0 * sqrt(SPRING_CONSTANT);
    static constexpr float CRITICAL_DEFORMATION_RATE = 0.05;
    static constexpr float DEFORMATION_VELOCITY_ATTENUATION = 0.99;
    static constexpr int WIDTH = 30;
    static constexpr int HEIGHT = 30;
    static constexpr int NUM_NODES = WIDTH * HEIGHT;
    static constexpr int NUM_NODEQUADS = (WIDTH-1) * (HEIGHT-1);
    static constexpr int NUM_VERTEX_COMPONENTS = NUM_NODEQUADS * 18;
    static const char* VERTEX_SHADER_FILE = SHADER_DIR "cloth.vert";
    static const char* FRAGMENT_SHADER_FILE = SHADER_DIR "cloth.frag";
  }

  namespace Simulation {
    static constexpr float WINDOW_WIDTH = 1200;
    static constexpr float WINDOW_HEIGHT = 1000;
    static constexpr int FPS = 100;
    static constexpr float MIN_DELTA = 1.0 / FPS;
    static constexpr float VIEW_MOVE_DELTA = 1.0;
    static constexpr float VIEW_ROTATE_DELTA = 0.2;
    static constexpr float EPSILON = 10e-6;
  }

  namespace Sphere {
    static const char* VERTEX_SHADER_FILE = SHADER_DIR "sphere.vert";
    static const char* FRAGMENT_SHADER_FILE = SHADER_DIR "sphere.frag";
  }

  namespace Triangle {
    static const char* VERTEX_SHADER_FILE = SHADER_DIR "triangle.vert";
    static const char* FRAGMENT_SHADER_FILE = SHADER_DIR "triangle.frag";
  }

  namespace Cube {
    static const char* VERTEX_SHADER_FILE = SHADER_DIR "cube.vert";
    static const char* FRAGMENT_SHADER_FILE = SHADER_DIR "cube.frag";
  }
}
