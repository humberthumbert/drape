#ifndef PARTICLE_H
#define PARTICLE_H

#include "GlobalTypes.h"
#include "Vector.h"

// -------------------- STL
#include <functional>

// -------------------- OpenMesh
//#include <OpenMesh/Core/Geometry/VectorT.hh>

class Particle
{
private:
	bool m_active;
	bool m_movable;
	float m_mass;
	//MyMesh::VertexIter v_it; // OpenMesh vertex iterator to the vertex this particle belongs to
	Vector3 m_pos;
	Vector3 m_oldPos;
	Vector3 m_acceleration;
	Vector3 m_norm;

public:
	Particle(const Vector3 &pos, float mass)
	:m_movable(true), m_mass(mass), m_pos(pos), m_oldPos(pos), m_acceleration(Vec::Zero), m_norm(Vec::Zero)
	{
	    m_active = true;
	}

	Particle() {}

	void addForce(const Vector3 &f) { m_acceleration += f / m_mass; }
    
	void timeStep(float deltaTime)
	{
		if(m_movable)
		{
			Vector3 temp = m_pos;
			m_pos = m_pos + (m_pos-m_oldPos)*(1.0 - PARTICLE_VELOCITY_DAMPING) + m_acceleration*deltaTime*deltaTime;
			m_oldPos = temp;
			resetAcceleration();
		}
	}

	bool isValid() { return m_active; }
	Vector3 getPos() { return m_pos; }
	void setMovable(bool value) { m_movable = value; }

	void offsetPos(const Vector3 &v)
	{
		if (m_movable)
		{
		    m_oldPos = m_pos;
			m_pos += v;
		}
	}

	void resetAcceleration() 
	{
		m_acceleration = Vec::Zero;
	}

	void resetNormal()
	{
	    m_norm = Vec::Zero;
	}

	void addToNormal(const Vector3& normal)
	{
	    m_norm += normal;
	}

	Vector3& getNormal() 
	{ 
		return m_norm;
	}

	void setToPrevPosition()
	{
	    m_pos = m_oldPos;
	}

	// might be used for std::set
	// std::size_t getHash() 
	// {
	//     size_t hash = std::hash<float>()(pos[0]);
	//     hash = hash ^ (std::hash<float>()(pos[1]) << 1);
	//     hash = hash ^ (std::hash<float>()(pos[2]) << 1);
	//     return hash;
	// }

	// bool operator==(const Particle & p) const
	// {
	//     Vector3 oPos = p.getPos();
	//     return pos[0] == oPos[0] && pos[1] == oPos[1] && pos[2] == oPos[2];
	// }

	// bool operator!=(const Particle & p) const
	// {
	//     return !(*this == p);
	// }
};

#endif /* end of PARTICLE_H */