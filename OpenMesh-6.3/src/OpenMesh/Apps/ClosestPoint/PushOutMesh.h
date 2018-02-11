#ifndef PUSH_OUT_MESH_H
#define PUSH_OUT_MESH_H


// Globally defined type and constants
#include "GlobalType.h"

// dependents
#include "ClosestPoint.h"

class MeshExtruder
{
public:
    // constructor
    MeshExtruder() {}

    // destructor
    ~MeshExtruder() {}

    // push out the cloth mesh from the body mesh
    void doPushOut(const MyMesh& body, MyMesh& cloth);

private:
  ClosestPoint m_closestPoint;
};

#endif /* end of PUSH_OUT_MESH_H */