#ifndef CLOTH_H
#define CLOTH_H

#include "Particle.h"
#include "Spring.h"
#include "GlobalTypes.h"
#include "Physics.h"

// -------------------- STL
#include <vector>
#include <functional>
#include <string>
#include <stdio.h>

// -------------------- OpenMesh
//#include <OpenMesh/Core/Geometry/VectorT.hh>


// struct ConstraintHash 
// {
// 	std::size_t operator()(const Constraint& c) const 
// 	{
// 		return std::hash<unsigned int>()(c.p1->getHash() + c.p2->getHash());
// 	}
// };

// struct ConstraintCompare
// {
// 	bool operator()(const Constraint & lhs, const Constraint & rhs) const
// 	{
// 	    return (*lhs.p1 == *rhs.p1 && *lhs.p2 == *rhs.p2) || (*lhs.p1 == *rhs.p2 && *lhs.p2 == *rhs.p1);
// 	}
// };

// ------------------------------ end of struct ConstraintHash
struct Face
{
	explicit Face(Particle *p1, Particle *p2, Particle *p3)
	{
		points[0] = p1;
		points[1] = p2;
		points[2] = p3;
	}

	Particle *points[3];
};

class Cloth
{
private:
  int numParticles;
  std::vector<std::vector<Particle *> > m_particles; // must garantee that num of rows = num of cols
  std::vector<Spring> m_springs;
	std::vector<Face> m_faces;
	void AddSpring(Particle *p1, Particle *p2, float d) { m_springs.push_back(Spring(p1, p2, d)); }
	void AddFace(Particle *p1, Particle *p2, Particle *p3) { m_faces.push_back(Face(p1, p2, p3)); }

public:
  explicit Cloth(std::pair<float, float> **allPoints, int row, int col);
  explicit Cloth() {}
	~Cloth();
	void timeStep(float deltaTime);
	void addForceToAll(const Vector3 &f);
  void collisionCheck(Rigidbody &rigidbody);

  // Getter
	Particle* getParticle(unsigned int x, unsigned int y) { return m_particles[x][y]; }
	std::vector<Face>& getFaces() { return m_faces; }

	// Attributes getter
	Vector3 calcTriangleNormal(Particle *p1, Particle *p2, Particle *p3);
	void getNormalsForAll();

	// for debug
	void printAllParticles();
	void printAllSprings();
};

#endif /* end of CLOTH_H */