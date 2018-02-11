include (ACGCommon)

include_directories (
  ../../..
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/external/ann_1.1.2/include
  ${GLUT_INCLUDE_DIR}
  ${OPENGL_INCLUDE_DIRS} 
)

# ANN libs
link_directories(${CMAKE_SOURCE_DIR}/external/ann_1.1.2/lib)

set (targetName ClosestPoint)

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

# collect all header and source files
acg_append_files (headers "*.h" .)
acg_append_files (sources "*.cpp" .)
acg_add_executable (${targetName} ${headers} ${sources})

target_link_libraries (${targetName}
  OpenMeshCore
  OpenMeshTools
  ANN
  ${OPENGL_LIBRARIES}
  ${GLUT_LIBRARIES}
)