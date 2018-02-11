#include "Physics.h"

// -------------------- OpenMesh
#include <OpenMesh/Core/Geometry/VectorT.hh>

void Rigidbody::init(MyMesh &bodyMesh)
{
    m_kdTree.init(bodyMesh);

    // request for vertex normal
    bodyMesh.request_vertex_normals();
    // we need face normals to update the vertex normals
    bodyMesh.request_face_normals();
    // let the mesh update the normals
    bodyMesh.update_normals();
    // dispose the face normals, as we don't need them anymore
    bodyMesh.release_face_normals();

    m_pMesh = &bodyMesh;
}

void Rigidbody::realse()
{
    m_pMesh->release_vertex_normals();
}

bool Rigidbody::isCollided(const Vector3& vertex)
{
    bool result = false;
    Vector3 localClosestPoint = Vec::Zero;
    double localDistance = 0.0;

    // retrive the closest point in body mesh from the given vertex
    m_kdTree.getClosestPoint(vertex, localClosestPoint, localDistance);

    // displacement vector
    OpenMesh::Vec3f uv;

    uv[0] = localClosestPoint[0] - vertex[0];
    uv[1] = localClosestPoint[1] - vertex[1];
    uv[2] = localClosestPoint[2] - vertex[2];

    MyMesh::VertexIter vIt,vBegin,vEnd;
    vBegin = m_pMesh->vertices_begin();
    vEnd = m_pMesh->vertices_end();
    for (vIt = vBegin; vIt != vEnd; ++vIt)
    {
        if( m_pMesh->point(*vIt)[0] == localClosestPoint[0] &&
            m_pMesh->point(*vIt)[1] == localClosestPoint[1] &&
            m_pMesh->point(*vIt)[2] == localClosestPoint[2] )
            {
                break;
            }
    }

    // normal for the closest point
    OpenMesh::Vec3f normal = m_pMesh->normal(*vIt);

    float dotResult = OpenMesh::dot(normal, uv);

    if(dotResult > 0.0f)
    {
        result = true;
    }

    return result;
}