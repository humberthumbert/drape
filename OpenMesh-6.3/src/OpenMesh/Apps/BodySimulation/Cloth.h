#ifndef CLOTH_H
#define CLOTH_H

// -------------------- OpenMesh
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>


#include "Spring.h"
#include "ClothNode.h"
#include "GlobalTypes.h"
#include "Body.h"
// -------------------- STL
#include <vector>
#include <functional>
#include <string>
#include <stdio.h>
#include <set>

class Cloth
{
private:
	QuadMesh clothMesh;
	Body* body;
	std::vector<ClothNode*> clothNodes;
	std::vector<ClothFace*> clothFaces;
	std::vector<std::vector<std::vector<std::set<ClothFace*> > > > spatialFaces;
  	std::vector<Spring*> clothSprings;
    float top, bottom, left, right, front, back;
    void addFaces(ClothNode* c1, ClothNode* c2, ClothNode* c3);
	void addSpring(std::set<std::string> &springSet, std::string p1str, std::string p2str, ClothNode* p1, ClothNode* p2, int type);
	//void AddBody(Body* body);
	void spatialCloth();
	void spatialClear();
public:
	explicit Cloth(QuadMesh mesh, Body* b);
	explicit Cloth(Body* b);
	explicit Cloth() {}
	~Cloth();
	void parseCloth();
	void timeStep(float deltaTime);
	void addForceToAll(const Vector3 &f);
	void collisionCheck(Body &body);

	// Getter
	QuadMesh getClothMesh() { return clothMesh; }
	std::vector<ClothNode*> getClothNodes() { return clothNodes;	}
	ClothNode* getClothNode(unsigned int x) { return clothNodes[x]; }
	std::vector<ClothFace*> getClothFaces() { return clothFaces; }
	std::set<ClothFace*> getSpatialFaces(Vector3 pos);
	std::vector<Spring*> getClothSprings(){return clothSprings; }
	Body* getBody() { return body; }
	
	// for debug
	void printAllNodes();
	void printAllSprings();
};

#endif /* end of CLOTH_H */