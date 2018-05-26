include (ACGCommon)

set (CMAKE_CXX_STANDARD 11)

include_directories (
  ../../..
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/external/ann_1.1.2/include
  ${GLUT_INCLUDE_DIR}
  ${OPENGL_INCLUDE_DIRS} 
)
#add_definitions(${GCC_COVERAGE_COMPILE_FLAGS})
#set (CMAKE_CXX_FLAGS "-lglfw -lX11 -lXrandr -lXinerama -lXi -lXxf86vm -lXcursor -lpthread -ldl -lGLEW -lGLU -lGL -I./$(LIB_DIR)/glm")

# ANN libs
link_directories(${CMAKE_SOURCE_DIR}/external/ann_1.1.2/lib)

set (targetName clothsim)

# add_subdirectory(include)
# add_subdirectory(src)

find_package(OpenGL)
include_directories(${OpenGL_INCLUDE_DIRS})
if(NOT OPENGL_FOUND)
    message(ERROR " OpenGL not found")
endif()

find_package(GLUT)
include_directories(${GLUT_INCLUDE_DIR})
if(NOT GLUT_FOUND)
    message(ERROR " GLUT not found")
endif()

find_package(glfw3 3.1.2 REQUIRED)

# collect all header and source files
acg_append_files (headers "*.h" .)
acg_append_files (sources "*.cpp" .)
acg_add_executable (${targetName} ${headers} ${sources})

#target_compile_options(${targetName} PUBLIC -Wl, -lglfw)

target_link_libraries (${targetName}
  OpenMeshCore
  OpenMeshTools
  ANN
  ${OPENGL_LIBRARIES}
  ${GLUT_LIBRARIES}
  glfw
  GLEW
  X11
  Xrandr
  Xinerama
  Xi
  Xxf86vm
  Xcursor
  pthread
  dl
  GLU
  GL
)

target_include_directories(${targetName} PUBLIC "./shaders")