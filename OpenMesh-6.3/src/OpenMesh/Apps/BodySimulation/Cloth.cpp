#include "Cloth.h"
#include <iostream>

Cloth::Cloth(QuadMesh mesh)
{    

     // creating particles
    for (MyMesh::VertexIter v_it=mesh.vertices_begin(); v_it!=mesh.vertices_end(); ++v_it)
    {
        Vector3 pos(mesh.point(*v_it)[0], mesh.point(*v_it)[1], mesh.point(*v_it)[2]);
        Particle* part = new Particle(pos, 1.0f);
        m_particles.push_back(part);
    }

    // creating face and structural and shear spring
    std::set<std::string> springSet;
    for(MyMesh::FaceIter f_it = mesh.faces_begin(); f_it != mesh.faces_end(); ++f_it) {
        std::vector<Particle*> vertices;
        std::vector<std::string> verticesIdx;
        for (MyMesh::FaceVertexIter fv_it=mesh.fv_iter(*f_it); fv_it.is_valid(); ++fv_it)
        {
            vertices.push_back(m_particles[fv_it->idx()]);
            verticesIdx.push_back(fv_it->idx()+"");

        }
        Particle *topLeft = vertices[0];
        Particle *topRight = vertices[1];
        Particle *botRight = vertices[2];
        Particle *botLeft = vertices[3];
        // structural spring | use verticesIdx to eliminate duplicated spring
        AddSpring(springSet, verticesIdx[0], verticesIdx[1], topLeft, topRight, UNI_KST);

        AddSpring(springSet, verticesIdx[0], verticesIdx[3], topLeft, botLeft, UNI_KST);

        AddSpring(springSet, verticesIdx[2], verticesIdx[1], botRight, topRight, UNI_KST);

        AddSpring(springSet, verticesIdx[2], verticesIdx[3], botRight, botLeft, UNI_KST);
        // shear spring

        AddSpring(topLeft, botRight, UNI_KSH);
        AddSpring(topRight, botLeft, UNI_KSH);
        // bending spring


        // two cases
        if (botLeft->isValid() && topLeft->isValid() && botRight->isValid())
        {
            AddFace(botRight, botLeft, topLeft);
        }
        else if(botRight->isValid() && botLeft->isValid() && topRight->isValid())
        {
            AddFace(botRight, botLeft, topRight);
        }
        if(topRight->isValid() && botRight->isValid() && topLeft->isValid())
        {
            AddFace(topRight, botRight, topLeft);
        }
        else if(topRight->isValid() && botLeft->isValid() && topLeft->isValid())
        {
            AddFace(topRight, botLeft, topLeft);             
        }
    }

    
    // create bending spring
    for (MyMesh::VertexIter v_it=mesh.vertices_sbegin(); v_it!=mesh.vertices_end(); ++v_it)
    {
        for (MyMesh::VertexVertexIter vv_it=mesh.vv_iter(*v_it); vv_it.is_valid(); ++vv_it) {
            // for each vertex iterate the secondary neighbor, since one vertex's four cross secondary neighbor has been
            // add as shear spring, only secondary neighbors in the same column or row will be add as bending spring.

            for (MyMesh::VertexVertexIter v3_it=mesh.vv_iter(*vv_it); v3_it.is_valid(); ++v3_it) {
                AddSpring(springSet, v_it->idx() + "", v3_it->idx() + ""                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        , m_particles[v_it->idx()], m_particles[v3_it->idx()], UNI_KB);
            }

        }
    }
}

Cloth::~Cloth()
{
    for(unsigned int x=0; x < m_particles.size(); ++x)
    {
        delete m_particles[x];
    }
}

void Cloth::timeStep(float deltaTime)
{
    // Apply forces
    std::vector<Spring>::iterator itrSpr;
    for(itrSpr = m_springs.begin(); itrSpr != m_springs.end(); ++itrSpr )
    {

    //#ifdef DEBUG
        //printf("spring btw [%f, %f, %f] and [%f, %f, %f] with ks=%f\n", itrSpr->p1->getPos().m_x, itrSpr->p1->getPos().m_y,
        //    itrSpr->p1->getPos().m_z, itrSpr->p2->getPos().m_x, itrSpr->p2->getPos().m_y, itrSpr->p2->getPos().m_z,
        //    itrSpr->getConstant());
    //#endif
        itrSpr->ApplyForce();
    }

    std::vector<Particle*>::iterator itrPar;
    for (itrPar = m_particles.begin(); itrPar != m_particles.end(); ++itrPar)
    {

        
        (*itrPar)->timeStep(deltaTime);

    }
}

void Cloth::addForceToAll(const Vector3 &f)
{
    for (unsigned int i = 0; i < m_particles.size(); ++i)
    {
        m_particles[i]->addForce(f);
    }
}

void Cloth::collisionCheck(Body &body)
{
    for (unsigned int i = 0; i < m_particles.size(); ++i)
    {
        if(body.isCollided(m_particles[i]->getPos())){
            m_particles[i]->setToPrevPosition();
        }
    }
}

Vector3 Cloth::calcTriangleNormal(Particle *p1,Particle *p2,Particle *p3)
{
    Vector3 pos1 = p1->getPos();
    Vector3 pos2 = p2->getPos();
    Vector3 pos3 = p3->getPos();
    
    Vector3 v1 = pos2-pos1;
    Vector3 v2 = pos3-pos1;
    
    return v1.cross(v2);
}

void Cloth::getNormalsForAll()
{
    // reset normals
    for (unsigned int x = 0; x < m_particles.size(); ++x)
    {
        m_particles[x]->resetNormal();
    }

    //create normals
    for (unsigned int x = 0; x < m_faces.size(); ++x)
    {
                Vector3 normal = calcTriangleNormal(m_faces[x].points[0], m_faces[x].points[1], m_faces[x].points[2]);
                m_faces[x].points[0]->addToNormal(normal);
                m_faces[x].points[1]->addToNormal(normal);
                m_faces[x].points[2]->addToNormal(normal);
    }
    // for (unsigned int x = 0; x < m_particles.size() - 1; ++x)
    // {
    //     for (unsigned int y = 0; y < m_particles.size() - 1; ++y)
    //     {
    //         Particle *botLeft = getParticle(x, y);
    //         Particle *botRight = getParticle(x+1, y);
    //         Particle *topLeft = getParticle(x, y+1);
    //         Particle *topRight = getParticle(x+1, y+1);

    //         // two cases
    //         if (botLeft->isValid() && topLeft->isValid() && botRight->isValid())
    //         {
    //             Vector3 normal = calcTriangleNormal(botRight, botLeft, topLeft);
    //             botRight->addToNormal(normal);
    //             botLeft->addToNormal(normal);
    //             topLeft->addToNormal(normal);
    //         }
    //         else if(botRight->isValid() && botLeft->isValid() && topRight->isValid())
    //         {
    //             Vector3 normal = calcTriangleNormal(botRight, botLeft, topRight);
    //             botRight->addToNormal(normal);
    //             botLeft->addToNormal(normal);
    //             topRight->addToNormal(normal);
    //         }

    //         if(topRight->isValid() && botRight->isValid() && topLeft->isValid())
    //         {
    //             Vector3 normal = calcTriangleNormal(topRight, botRight, topLeft);
    //             topRight->addToNormal(normal);
    //             botRight->addToNormal(normal);
    //             topLeft->addToNormal(normal);
    //         }
    //         else if(topRight->isValid() && botLeft->isValid() && topLeft->isValid())
    //         {
    //             Vector3 normal = calcTriangleNormal(topRight, botLeft, topLeft);
    //             topRight->addToNormal(normal);
    //             botLeft->addToNormal(normal);
    //             topLeft->addToNormal(normal);                    
    //         }
    //     }
    // }
}

void Cloth::AddSpring(std::set<std::string> &springSet,std::string p1str, std::string p2str, Particle *p1, Particle *p2, float d) { 

    std::string p1p2 = p1str+"|"+p2str, p2p1 = p2str+"|"+p1str;
    if(springSet.find(p1p2) == springSet.end() && springSet.find(p2p1) == springSet.end()){
        springSet.insert(p2p1);
        m_springs.push_back(Spring(p1, p2, d));  
    }

}

void Cloth::AddSpring(Particle *p1, Particle *p2, float d) { 
    m_springs.push_back(Spring(p1, p2, d));  
}

void Cloth::printAllParticles()
{
    for (unsigned int i = 0; i < m_particles.size(); ++i)
    {
        printf("[%f, %f, %f] ", m_particles[i]->getPos().m_x, m_particles[i]->getPos().m_y, m_particles[i]->getPos().m_z);
        printf("\n");
    }
}

void Cloth::printAllSprings()
{
    for (unsigned int i = 0; i < m_springs.size(); ++i)
    {
        printf("spring btw [%f, %f, %f] and [%f, %f, %f] with ks=%f\n", 
                m_springs[i].p1->getPos().m_x, m_springs[i].p1->getPos().m_y, m_springs[i].p1->getPos().m_z, 
                m_springs[i].p2->getPos().m_x, m_springs[i].p2->getPos().m_y, m_springs[i].p2->getPos().m_z,
                m_springs[i].getConstant());
    }
}