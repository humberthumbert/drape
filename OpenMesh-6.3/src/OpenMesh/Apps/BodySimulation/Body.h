#ifndef BODY_H
#define BODY_H

// -------------------- OpenMesh
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>

// -------------------- STL
#include <vector>
#include <functional>
#include <string>
#include <stdio.h>
#include <iostream>

#include "GlobalTypes.h"
#include "KDTree.h"
#include "Vector.h"
//typedef OpenMesh::PolyMesh_ArrayKernelT<>  MyMesh;
typedef enum { CROCTCH, L_ARMPIT, R_ARMPIT, F_NECKPT, S_NECKPT, B_NECKPT, SHOULDER_PT } BODY_LANDMARK;

class Body{
private:
	MyMesh bodyMesh;
	KDTree kdTree;

	// id
	long n_id;
	// actual height
	float height;
	// ratio of model height to actual height
	float ratio;
	//-------------------------Preprocess----------------------------//
	// the distance between the highest and lowest point
	float modelHeight, modelWidth, top, bottom, left, right; 
	// iterator through all the vertice and find the highest and lowest point
	void getModelHeight();
	// divide the model into 100 sections based on z coord, xy plane
	std::vector<std::vector<MyMesh::Point> > ySegment;// = std::vector<std::vector<MyMesh::Point> >(100);
	std::vector<std::vector<MyMesh::Point> > xSegment;// = std::vector<std::vector<MyMesh::Point> >(100);
	
	// slice the model into 100 section
	void sliceModel();
	// x,y,z coordinate (assume z as the upward one with unit vector<0,0,1>)
	Vector3 xCoord, yCoord, zCoord;
	// identify where the body face as x positive coordinate
	// detail method see "Automatic body landmark identification for various body figures" Hyunsook Han, Yunja Nam
	void bodyDirection();
	// helper function
	std::vector<std::vector<MyMesh::Point> > getFullDivision(float &k, float centerX, float centerY, std::vector<MyMesh::Point> ankle);
	// turn the body to the correct direction
	void bodyRotation();

	//-------------------------Process----------------------------//
	// landmark for 0:croctch, 1/2:armpits, 3/4:shoulder points, 5/6:hip points, 7/8:chest points.
	std::vector<MyMesh::Point> landmarks;
	MyMesh::Point croctchPT, leftArmpitPT, rightArmpitPT, leftShoulderPT, rightShoulderPT, 
					leftHipPT, rightHipPT, leftChestPT, rightChestPT;
	float modelWaist, modelChest;
	// automatic get landmarks
	void calLandmarks();
	void calCroctch();
	void calArmpits();
	void calWaist();
	void calFrontNeckPT();
	void calSideNeckPT();
	void calBackNeckPT();
	void calShoulderPT();
	void calChest();
	void calHip();
	std::vector<MyMesh::Point> integrate(int segStart, int segEnd, int direction);
	std::vector<std::vector<MyMesh::Point> > segIntersect(int segYStart, int segYEnd, int segXStart, int segXEnd, int cutModel);
	
public:
	Body();
	Body(MyMesh mesh, float height);
	Body(char *filename, float height);

	Vector3 repulsionResponse(Vector3& position, Vector3& force);
	// setter and getter
	MyMesh getBody(){ return bodyMesh; }
	//void setBody(MyMesh bodyMesh);
	Vector3 getXcoord(){return xCoord;}
	Vector3 getYcoord(){return yCoord;}
	Vector3 getZcoord(){return zCoord;}
	std::vector<MyMesh::Point> getLandmarks(){ return landmarks; }
	float getHeight(){ return height; }
	void setHeight(float newHeight){ height = newHeight; }
	std::vector<MyMesh::Point> getXSegment(int i){ return xSegment[i]; }
	std::vector<MyMesh::Point> getYSegment(int i){ return ySegment[i]; }
};

#endif