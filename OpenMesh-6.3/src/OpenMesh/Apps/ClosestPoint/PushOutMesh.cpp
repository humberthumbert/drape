#include "PushOutMesh.h"

// -------------------- OpenMesh
#include <OpenMesh/Core/Geometry/VectorT.hh>

void
MeshExtruder::doPushOut(const MyMesh& body, MyMesh& cloth)
{
    m_closestPoint.init(body);

    // request for vertex normal
    cloth.request_vertex_normals();
    // we need face normals to update the vertex normals
    cloth.request_face_normals();

    // let the mesh update the normals
    cloth.update_normals();

    // dispose the face normals, as we don't need them anymore
    cloth.release_face_normals();

    // iterator, the start, and the end
    MyMesh::VertexIter vIt,vBegin,vEnd;
    vBegin = cloth.vertices_begin();
    vEnd = cloth.vertices_end();

    // iterate through all vertices in cloth mesh
    for (vIt = vBegin; vIt != vEnd; ++vIt)
    {
        MyMesh::Point vertex = cloth.point(*vIt);
        MyMesh::Point localClosestPoint;
        double localDistance;

        // retrive the closest point in body mesh from the given vertex
        m_closestPoint.getClosestPoint(vertex, localClosestPoint, localDistance);

        // displacement vector
        OpenMesh::Vec3f uv;

        uv[0] = localClosestPoint[0] - vertex[0];
        uv[1] = localClosestPoint[1] - vertex[1];
        uv[2] = localClosestPoint[2] - vertex[2];

         for (MyMesh::VertexVertexIter vv_it = 
                    cloth.vv_iter(vIt.handle()); vv_it; ++vv_it)
          {
            // do something with e.g. mesh.point(*vv_it)
          }


        // normal for the vertex
        OpenMesh::Vec3f normal = cloth.normal(*vIt);

        float dotResult = OpenMesh::dot(normal, uv);

        if(dotResult > 0.0f)
        {
            cloth.set_point(*vIt, localClosestPoint);
        }
    }

    // release the vertex normals
    cloth.release_vertex_normals();
}