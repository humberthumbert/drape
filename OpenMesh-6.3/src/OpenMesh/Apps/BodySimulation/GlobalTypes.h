#ifndef GLOBALTYPES_H
#define GLOBALTYPES_H

//------- OpenMesh
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
typedef OpenMesh::TriMesh_ArrayKernelT<>  MyMesh;
typedef OpenMesh::PolyMesh_ArrayKernelT<>  QuadMesh;

// ------------------- Constants
#define PARTICLE_VELOCITY_DAMPING 0.1f // Damping is for air resistence, we might not want to use it once we have F(air resistence) done.
#define CONSTRAINT_ITERATIONS 15
#define STRUCT 0
#define SHEAR 1
#define BEND 2
#define UNI_KST 0.75f
#define UNI_KSH 0.75f
#define UNI_KB 0.95f
#define DP_KST -0.25f
#define DP_KSH -0.25f
#define DP_KB -0.25f
#define MASS 1.0f
#define GRAVITY 0.00981f
#define THICKNESS 0.001f
#define FRICTION_COEFFICIENT 0.1f
#define DISSIPATION_COEFFICIENT 0.1f;
//------------------- DEBUG
#define DEBUG false
#endif /* end of GLOBALTYPES_H */