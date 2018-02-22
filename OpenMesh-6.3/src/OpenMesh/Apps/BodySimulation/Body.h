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

#include "Vector.h"
typedef OpenMesh::PolyMesh_ArrayKernelT<>  MyMesh;
typedef enum { CROCTCH, L_ARMPIT, R_ARMPIT, F_NECKPT, S_NECKPT, B_NECKPT, SHOULDER_PT } BODY_LANDMARK;

class Body{
private:
	MyMesh bodyMesh;
	// id
	long n_id;
	// actual height
	float height;
	// ratio of model height to actual height
	float ratio;
	//-------------------------Preprocess----------------------------//
	// the distance between the highest and lowest point
	double modelHeight, top, bottom; 
	// iterator through all the vertice and find the highest and lowest point
	void getModelHeight();
	// divide the model into 100 sections based on z coord, xy plane
	std::vector<std::vector<MyMesh::Point> > zSegement = std::vector<std::vector<MyMesh::Point> >(1001);
	// slice the model into 100 section
	void sliceModel();
	// x,y,z coordinate (assume z as the upward one with unit vector<0,0,1>)
	Vector3 xCoord, yCoord, zCoord;
	// identify where the body face as x positive coordinate
	// detail method see "Automatic body landmark identification for various body figures" Hyunsook Han, Yunja Nam
	void bodyDirection();
	// helper function
	std::vector<std::vector<MyMesh::Point> > getFullDivision(float &k, float centerX, float centerY, std::vector<MyMesh::Point> ankle);

	
	//-------------------------Process----------------------------//
	// landmark for croctch, armpits, front neck point, side neck point, back neck point, shoulder points.
	std::vector<MyMesh::Point> landmarks;
	// automatic get landmarks
	void calLandmarks();
	void calCroctch();
	void calArmpits();
	void calFrontNeckPT();
	void calSideNeckPT();
	void calBackNeckPT();
	void calShoulderPT();
	void find_shoulder();
	void find_chest();
	void find_hip();
	
public:
	Body();
	Body(MyMesh mesh, float height);
	Body(char *filename, float height);

	//NOTE: left and right is respective to the viewer's perspective

	//points for the shoulder
	float xmaxpoint[3];
	float xminpoint[3];
	//points for the chests
	float leftChestPoint[3];
	float rightChestPoint[3];
	//poitns for the hips
	float leftHipPoint[3];
	float rightHipPoint[3];
	// setter and getter
	//void setBody(MyMesh bodyMesh);
	std::vector<MyMesh::Point> getLandmarks(){ return landmarks; }
	float getHeight(){ return height; }
	void setHeight(float newHeight){ height = newHeight; }
};

#endif