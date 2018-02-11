#ifndef PHYSICS_H
#define PHYSICS_H

#include "GlobalTypes.h"
#include "KDTree.h"
#include "Particle.h"

class Rigidbody
{
public:
    Rigidbody(){}

    ~Rigidbody(){}

    // before checking collision, init the KDTree
    void init(MyMesh &bodyMesh);

    void realse();

    bool isCollided(const Vector3& vertex);

private:
    KDTree m_kdTree;
    MyMesh* m_pMesh;
};

#endif /* end of PHYSICS_H */