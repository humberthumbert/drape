#include "Cloth.h"

Cloth::Cloth(std::pair<float, float>** allPoints, int row, int col)
{    

    m_particles = std::vector<std::vector<Particle*> >(col, std::vector<Particle*>());
    
    // creating particles
    for(int x=0; x<row; ++x)
    {
        for (int y = 0; y < col; ++y)
        {
            //Modified
            Vector3 pos(allPoints[x][y].first, 0.0f, allPoints[x][y].second);
            Particle* part = new Particle(pos, 1.0f);
            m_particles[x].push_back(part);
        }
    }

    printAllParticles();

    //Adding springs (structural and shear)
    for(int x=0; x<row; ++x)
    {
        for(int y=0; y<col; ++y)
        {
            if (m_particles[x][y]->isValid())
            {
                if ((x < row - 1) && m_particles[x + 1][y]->isValid()) 
                    AddSpring(getParticle(x, y), getParticle(x + 1, y), UNI_KST);
                if ((y< col - 1) && m_particles[x][y + 1]->isValid())
                    AddSpring(getParticle(x, y), getParticle(x, y + 1), UNI_KST);
                if ((x < row - 1 && y < col - 1) && m_particles[x + 1][y + 1]->isValid())
                    AddSpring(getParticle(x, y), getParticle(x + 1, y + 1), UNI_KSH);
                if ((x > 1 && y > 1) && m_particles[x - 1][y - 1]->isValid())
                    AddSpring(getParticle(x, y), getParticle(x - 1, y - 1), UNI_KSH);
            }
        }
    }

    // bending spring
    // for (int x = 0; x < row; ++x)
    // {
    //     int y1 = 0;
    //     while (!m_particles[x][y1]->isValid())
    //     {
    //         ++y1;
    //     }

    //     int y2 = col - 1;
    //     while(!m_particles[x][y2]->isValid())
    //     {
    //         --y2;
    //     }

    //     // now we have two particles (x, y1) <-> (x, y2)
    //     // add bending if there are not the same
    //     if(y1 != y2)
    //     {
    //         AddSpring(getParticle(x, y1), getParticle(x, y2), UNI_KB);
    //     }
    // }

    // for (int y = 0; y < col; ++y)
    // {
    //     int x1 = 0;
    //     while (!m_particles[x1][y]->isValid())
    //     {
    //         ++x1;
    //     }

    //     int x2 = row - 1;
    //     while(!m_particles[x2][y]->isValid())
    //     {
    //         --x2;
    //     }

    //     // now we have two particles (x1, y) <-> (x2, y)
    //     // add bending if there are not the same
    //     if(x1 != x2)
    //     {
    //         AddSpring(getParticle(x1, y), getParticle(x2, y), UNI_KB);
    //     }
    // }

#ifdef DEBUG
    printAllSprings();
#endif

    // create all faces
    for (unsigned int x = 0; x < m_particles.size() - 1; ++x)
    {
        for (unsigned int y = 0; y < m_particles.size() - 1; ++y)
        {
            Particle *botLeft = getParticle(x, y);
            Particle *botRight = getParticle(x+1, y);
            Particle *topLeft = getParticle(x, y+1);
            Particle *topRight = getParticle(x+1, y+1);

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
    }
}

Cloth::~Cloth()
{
    for(int x=0; x < m_particles.size(); ++x)
    {
        for (int y = 0; y < m_particles[x].size(); ++y)
        {
            delete m_particles[x][y];
        }
    }
}

void Cloth::timeStep(float deltaTime)
{
    // Apply forces
    std::vector<Spring>::iterator itrSpr;
    for(itrSpr = m_springs.begin(); itrSpr != m_springs.end(); ++itrSpr )
    {

#ifdef DEBUG
        //printf("spring btw [%f, %f, %f] and [%f, %f, %f] with ks=%f\n", itrSpr->p1->getPos().m_x, itrSpr->p1->getPos().m_y,
        //    itrSpr->p1->getPos().m_z, itrSpr->p2->getPos().m_x, itrSpr->p2->getPos().m_y, itrSpr->p2->getPos().m_z,
        //    itrSpr->getConstant());
#endif
        itrSpr->ApplyForce();
    }

    for (unsigned int i = 0; i < m_particles.size(); ++i)
    {
        std::vector<Particle*>::iterator itrPar;
        for(itrPar = m_particles[i].begin(); itrPar != m_particles[i].end(); ++itrPar)
        {
            (*itrPar)->timeStep(deltaTime);
        }
    }
}

void Cloth::addForceToAll(const Vector3 &f)
{
    for (unsigned int i = 0; i < m_particles.size(); ++i)
    {
        for (unsigned int j = 0; j < m_particles[i].size(); ++j)
        {
            m_particles[i][j]->addForce(f);
        }
    }
}

void Cloth::collisionCheck(Rigidbody &rigidbody)
{
    for (unsigned int i = 0; i < m_particles.size(); ++i)
    {
        std::vector<Particle*>::iterator itrPar;
        for(itrPar = m_particles[i].begin(); itrPar != m_particles[i].end(); ++itrPar)
        {
            if(rigidbody.isCollided((*itrPar)->getPos()))
            {
                (*itrPar)->setToPrevPosition();
            }
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
        for (unsigned int y = 0; y < m_particles[x].size(); ++y)
        {
            m_particles[x][y]->resetNormal();
        }
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

void Cloth::printAllParticles()
{
    for (unsigned int i = 0; i < m_particles.size(); ++i)
    {
        for (unsigned int j = 0; j < m_particles[i].size(); ++j)
        {
            printf("[%f, %f, %f] ", m_particles[i][j]->getPos().m_x, m_particles[i][j]->getPos().m_y, m_particles[i][j]->getPos().m_z);
        }
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