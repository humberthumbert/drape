#ifndef CLOTHNODE_H
#define CLOTHNODE_H

#include "Body.h"
#include "GlobalTypes.h"
#include "Vector.h"
// -------------------- STL
#include <functional>
#include <iostream>
//class Spring;
class ClothFace;
class ClothNode
{
private:
	int index;
  	Body* body;
  	float mass;
  	bool collided = false;
  	bool mFixed;
  	//SemiImplicitEulerIntegrator mIntegrator;
	Vector3 position, oldPosition;
	Vector3 velocity, oldVelocity;
	Vector3 force;
	std::set<ClothFace*> neighborFaces;
	std::vector<ClothNode*> structAdjacents;
	std::vector<ClothNode*> shearAdjacents;
	std::vector<ClothNode*> bendAdjacents;
public:
	friend std::ostream& operator << (std::ostream& o, const ClothNode& cn) {
    	return o << cn.position.m_x << " " << cn.position.m_y << " " << cn.position.m_z << std::endl;
  	}
  	
	Vector3 averageVelocity;
	ClothNode(Vector3 vec, int idx, Body* body, float m);
	ClothNode(ClothNode* cn);
	void timeStep(float deltaTime);
	void update(float deltaTime);
	/* Setter */
	void addStructAdjacent(ClothNode* structAdj) { structAdjacents.push_back(structAdj); }
	void addShearAdjacent(ClothNode* shearAdj) { shearAdjacents.push_back(shearAdj); }
	void addBendAdjacent(ClothNode* bendAdj) { bendAdjacents.push_back(bendAdj); }
	void setNeighborFaces(std::set<ClothFace*> nf) {neighborFaces = nf;}
	void setPosition(Vector3 pos) { position = pos; }
	void setVelocity(Vector3 vel) { velocity = vel; }
	void setForce(Vector3 fc) { force = fc; }
	void setMass(float m) { mass = m; }
	void setFixed(bool f) { mFixed = f; }

	/* Getter */
	int getIndex() { return index; }
	Vector3 getPosition() { return position; }
	Vector3 getVelocity() { return velocity; }
	Vector3 getOldPosition() { return oldPosition; }
	Vector3 getOldVelocity() { return oldVelocity; }
	Vector3 getForce() { return force; }
	float getMass() { return mass; }
	bool isCollided() { return collided; }
	bool isFixed() { return mFixed; }
  	//std::vector<Spring*> getSprings() { return springs; }
	std::vector<ClothNode*> getStructAdjacent() 	{ return structAdjacents; 	}
	std::vector<ClothNode*> getShearAdjacent() 	{ return shearAdjacents; 	}
	std::vector<ClothNode*> getBendAdjacent() 	{ return bendAdjacents;		}
	std::set<ClothFace*> getNeighborFaces() { return neighborFaces;	}
	/* Change */
	void addPosition(Vector3 dp) { position += dp; }
	void addVelocity(Vector3 dv) { velocity += dv; }
	void addForce(Vector3 fc) { 
		force += fc; 
		//if(fc.m_x!=0 || fc.m_z!=0 || fc.m_y!=0) printf("force changed [%f %f %f]\n", fc.m_x, fc.m_y, fc.m_z);
	}
};


class ClothFace{
public:
	ClothNode* cn1, *cn2, *cn3;
	Vector3 normal;

	ClothFace(ClothNode* c1, ClothNode* c2, ClothNode* c3){
    	update(c1, c2, c3);
	}
	void update(ClothNode* c1, ClothNode* c2, ClothNode* c3){
	    cn1 = c1;
	    cn2 = c2;
	    cn3 = c3;
	    Vector3 cn12 = cn2->getPosition()-cn1->getPosition();
	    Vector3 cn13 = cn3->getPosition()-cn1->getPosition();
	    normal = (cn12.cross(cn13)).getNorm();    
	}

};

#endif /* end of CLOTHNODE_H */