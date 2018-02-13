#include "Body.h"
#include <limits>
#include <cmath>

final static int MEAN_ANKLE_HEIGHT = 5;
final static int LEG_DIVISION_CRITIAL_DIST = 5;
final static float PI = std::atan(1.0)*4;
final static int ARC_STEP = 12;// 15 degree
Body::Body(MyMesh mesh, float height){
	bodyMesh = mesh;
	this.height = height;
	getModelHeight();
}
//-------------------------Preprocess----------------------------//
// iterator through all the vertice and find the highest and lowest point
void Body::getModelHeight(){
	top = std::numeric_limits<float>::min();
	bottom = std::numeric_limits<float>::max();
	for(MyMesh::FaceIter f_it = bodyMesh.faces_begin(); f_it != bodyMesh.faces_end(); ++f_it) {
		for (MyMesh::FaceVertexIter fv_it=bodyMesh.fv_iter(*f_it); fv_it.is_valid(); ++fv_it)
  		{
	    	if(top < bodyMesh.point(*fv_it)[2]) top = bodyMesh.point(*fv_it);
	    	if(bottom > bodyMesh.point(*fv_it)[2]) bottom = bodyMesh.point(*fv_it);
		}
    }
    modelHeight = std::abs(top-bottom);
    ratio = modelHeight * height;
}
// slice the model into 100 section
void Body::sliceModel(){
	float percentage = 0.0f;
	for(MyMesh::FaceIter f_it = bodyMesh.faces_begin(); f_it != bodyMesh.faces_end(); ++f_it) {
		for (MyMesh::FaceVertexIter fv_it=bodyMesh.fv_iter(*f_it); fv_it.is_valid(); ++fv_it)
  		{
	    	percentage = std::abs(bodyMesh.point(*fv_it)[2] - bottom) / modelHeight;
	    	int z = (int)(percentage*100);
	    	zSegement[z].push_back(bodyMesh.point(*fv_it));
		}
    }
}

float distPT2Line(float x, float y, float k, float b){
	return std::abs(k*x-y+b)/std::sqrt(k*k+1);
}
float slope(float x1, float y1, float x2, float y2){
	return (y1-y2)/(x1-x2);
}

std::vector<std::vector<MyMesh::Point> > Body::getFullDivision(float &k, float centerX, float centerY, std::vector<MyMesh::Point> ankle){
	bool isSeperated = false;
	float b = 0.0f;
	// Select one point and add 15 degree to rotate the line until the line seperate
	// the cross section to two groups
	while(!isSeperated){
		k = std::tan(std::atan(k) + PI/ARC_STEP);
		b = centerY - k*centerX;
		bool isFound=false;
		for(int i = 0; i < ankle.size(); ++i){
			// distance = |ax0+by0+c|/sqrt(a^2+b^2)
			float distance = distPT2Line(ankle[i][0], ankle[i][1], k, b);
			if(distance < LEG_DIVISION_CRITIAL_DIST * ratio){
				isFound=true;
				break;
			}
		}
		if(!isFound)isSeperated=true;
	}
	std::vector<std::vector<MyMesh::Point> > result(2);
	for(int i = 0; i < ankle.size(); ++i){
		if(k*ankle[i][0]-ankle[i][1]+b > 0){
			result[0].push_back(ankle[i]);
		} else {
			result[1].push_back(ankle[i]);	
		}
	}
	return result;
}

// identify where the body face as x positive coordinate
// detail method see "Automatic body landmark identification for various body figures" Hyunsook Han, Yunja Nam
void Body::bodyDirection(){
	// First choose ankle section which is 0.05L, and find the center pointof all cross sections points
	float totalX = 0.0f, totalY = 0.0f;
	std::vector<MyMesh::Point> ankle = zSegement[MEAN_ANKLE_HEIGHT-1];
	for(int i = 0; i < ankle.size(); ++i){
		totalX += ankle[i][0];
		totalY += ankle[i][1];
	}
	float centerX = totalX / ankle.size(), centerY = totalY / ankle.size();
	// Find a line seperating all points into two groups
	// kx1+b=y1, kx2+b=y2, k=(y1-y2)/(x1-x2), b=y2-(x1-x2)/(y1-y2)
	float k = slope(ankle[0][0], ankle[0][1], centerX, centerY);
	std::vector<std::vector<MyMesh::Point> > ankles= getFullDivision(k, centerX, centerY, ankle);
	// Get the center point of each part of cross section
	float ankle1X = 0.0f, ankle1X = 0.0f;
	for(int i = 0; i < ankles[0].size(); ++i){
		ankle1X += ankles[0][i][0];
		ankle1Y += ankles[0][i][1];
	}
	ankle1X /= ankles[0].size;
	ankle1Y /= ankles[0].size;
	
	float ankle2X = 0.0f, ankle2X = 0.0f;
	for(int i = 0; i < ankles[1].size(); ++i){
		ankle2X += ankles[1][i][0];
		ankle2Y += ankles[1][i][1];
	}
	ankle2X /= ankles[1].size;
	ankle2Y /= ankles[1].size;
	// k is the left-right direction line slope and 1/k is the front-bak direction
	k = slope(ankle1X, ankle1Y, ankle2X, ankle2Y);

	// Second, use the foot to identify front and back
	std::vector<MyMesh::Point> foot = zSegement[0];
	MyMesh::Point footTip;
	float furtherest = std::numeric_limits<float>::min();
	for(int i = 0; i < foot[i].size(); ++i){
		float dist = distPT2Line(foot[i][0], foot[i][1], k, ankle1Y-k*ankle1X);
		if(dist > furtherest){
			furtherest = dist;
			footTip = foot[i];
		}
	}
	int upward = 1;
	if(footTip[0]*k + ankle1Y-k*ankle1X - footTip[1] < 0){ upward = -1; }

	yCoord = new Vector3(upward/k, upward, 0);//front direction
	k = std::tan(std::atan(upward*k)+PI/2);
	xCoord = new Vector3(k, 1, 0);// left direction
	zCoord = new Vector3(0, 0, 1);// up direction
}

//-------------------------Process----------------------------//
// automatic get landmarks
void calLandmarks(){

}

void calCroctch();
void calArmpits();
void calFrontNeckPT();
void calSideNeckPT();
void calBackNeckPT();
void calShoulderPT();
	