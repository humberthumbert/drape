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
#define UNI_KST 2.0f
#define UNI_KSH 3.0f
#define UNI_KB 1.0f

//------------------- DEBUG
#define DEBUG false

#endif /* end of GLOBALTYPES_H */