#include "Body.h"
#include <limits>
#include <cmath>
#include <fstream>

#define MEAN_ANKLE_HEIGHT 5
#define LEG_DIVISION_CRITIAL_DIST 5
#define PI 3.1415926
#define ARC_STEP 12// 15 degree
Body::Body(MyMesh mesh, float height){
	bodyMesh = mesh;
	this->height = height;
	getModelHeight();
	sliceModel();
	std::cout <<"hhhhhh" << std::endl;
	//bodyDirection();
	find_shoulder();
	find_hip();
	find_chest();
	std::ofstream ofile("benchmark.txt");
	ofile << xminpoint[0] << " " << xminpoint[1] << " " << xminpoint[2] << " (left shoulder)" << std::endl;
    ofile << xmaxpoint[0] << " " << xmaxpoint[1] << " " << xmaxpoint[2] << " (right shoulder)" << std::endl;
    ofile << leftHipPoint[0] << " " << leftHipPoint[1] << " " << leftHipPoint[2] << " (left chest)" << std::endl;
    ofile << rightHipPoint[0] << " " << rightHipPoint[1] << " " << rightHipPoint[2] << " (right chest)" << std::endl;
	ofile << leftChestPoint[0] << " " << leftChestPoint[1] << " " << leftChestPoint[2] << " (left chest)" << std::endl;
    ofile << rightChestPoint[0] << " " << rightChestPoint[1] << " " << rightChestPoint[2] << " (right chest)" << std::endl;

    ofile.close();
}
//-------------------------Preprocess----------------------------//
// iterator through all the vertice and find the highest and lowest point
void Body::getModelHeight(){
	top = std::numeric_limits<float>::min();
	bottom = std::numeric_limits<float>::max();
	for(MyMesh::FaceIter f_it = bodyMesh.faces_begin(); f_it != bodyMesh.faces_end(); ++f_it) {
		for (MyMesh::FaceVertexIter fv_it=bodyMesh.fv_iter(*f_it); fv_it.is_valid(); ++fv_it)
  		{
	    	if(top < bodyMesh.point(*fv_it)[1]) top = bodyMesh.point(*fv_it)[1];
	    	if(bottom > bodyMesh.point(*fv_it)[1]) bottom = bodyMesh.point(*fv_it)[1];
		}
    }
    modelHeight = std::abs(top-bottom);
    ratio = modelHeight / height;
}

//finds the shoulders, defined as the points with the 
//maximum and minimum x values in the 806th slice
void Body::find_shoulder(){
	
	float xmax = std::numeric_limits<float>::min();
	float xmin = std::numeric_limits<float>::max();
	//find shoulder
	std::vector<MyMesh::Point> points = zSegement[806];
	for (int i = 0; i < points.size(); ++i){
		if(xmax < points[i][0]) {
    		xmax = points[i][0];
    		xmaxpoint[0] = points[i][0];
    		xmaxpoint[1] = points[i][1];
    		xmaxpoint[2] = points[i][2];

    	}
    	if(xmin > points[i][0]) {
    		xmin = points[i][0];
    		xminpoint[0] = points[i][0];
    		xminpoint[1] = points[i][1];
    		xminpoint[2] = points[i][2];
		}
	}
        

}


void Body::find_hip(){
	float xmax = std::numeric_limits<float>::min();
	float xmin = std::numeric_limits<float>::max();
	
	
	std::vector<MyMesh::Point> points = zSegement[480];

	for (int i = 0; i < points.size(); ++i){
		if(xmax < points[i][0]) {
    		xmax = points[i][0];
    	}
    	if(xmin > points[i][0]) {
    		xmin = points[i][0];
		}
	}
	float totalWidth = std::abs(xmax - xmin);

	std::vector<std::vector<MyMesh::Point> > xSlices = std::vector<std::vector<MyMesh::Point> >(100);

	//slice in x
	for(int i = 0; i < points.size(); ++i){
		float percentage = std::abs(points[i][0] - xmin) / totalWidth;
		int index = round(percentage * 100);
		if(index >= 100){
			index = 99;
		}
		xSlices[index].push_back(points[i]);
	}

	for(int i = 0; i < xSlices.size(); ){
		for(int j = 0; j < 10 && i < xSlices.size(); ++j){
			std::cout << xSlices[i].size() << " ";
			++i;
		}
		std::cout << std::endl;
	}

	int consecutiveRowsZero = 0;
	int posIndex = 0;
	//go in positive direction
	for(int i = 50; i < 101; ++i){
		if(xSlices[i].size() == 0){
			consecutiveRowsZero++;
		}else{
			consecutiveRowsZero = 0;
		}

		if(consecutiveRowsZero > 10){
			posIndex = i - consecutiveRowsZero;
			break;
		}
	}

	consecutiveRowsZero = 0;
	int negIndex = 0;
	//go in negative direction
	for(int i = 50; i > -1; --i){
		if(xSlices[i].size() == 0){
			consecutiveRowsZero++;
		}else{
			consecutiveRowsZero = 0;
		}

		if(consecutiveRowsZero > 10){
			negIndex = i + consecutiveRowsZero;
			break;
		}
	}

	std::vector<MyMesh::Point>& leftPoints = xSlices[negIndex];
	xmin = std::numeric_limits<float>::max();
	for (int i = 0; i < leftPoints.size(); ++i){
    	if(xmin > leftPoints[i][0]) {
    		xmin = leftPoints[i][0];
    		leftHipPoint[0] = leftPoints[i][0];
    		leftHipPoint[1] = leftPoints[i][1];
    		leftHipPoint[2] = leftPoints[i][2];
		}
	}

	std::vector<MyMesh::Point>& rightPoints = xSlices[posIndex];
	xmax = std::numeric_limits<float>::min();
	for (int i = 0; i < rightPoints.size(); ++i){
    	if(xmax < rightPoints[i][0]) {
    		xmax = rightPoints[i][0];
    		rightHipPoint[0] = rightPoints[i][0];
    		rightHipPoint[1] = rightPoints[i][1];
    		rightHipPoint[2] = rightPoints[i][2];
		}
	}
}
//find hip must be called before this method
//because it's assumed that the armpit x coordinates
//is roughly the same as the hips coordinate, and
//this is an approximation of the chests as being at the
//mid point between each sides of the body and the center 
//of the body
void Body::find_chest(){
	float zmax = std::numeric_limits<float>::min();
	float zmin = std::numeric_limits<float>::max();

	std::vector<MyMesh::Point> points = zSegement[712];
	//find mid point of z
	for (int i = 0; i < points.size(); ++i){
		if(zmax < points[i][2]) {
    		zmax = points[i][2];
    	}
    	if(zmin > points[i][2]) {
    		zmin = points[i][2];
		}
	}
	float midZ = zmin + (std::abs(zmax - zmin) / 2.0);
	
	float width = std::abs(leftHipPoint[0] - rightHipPoint[0]);
	float chestDistance = width / 4;
	float leftChestX = leftHipPoint[0] + chestDistance;
	float rightChestX = rightHipPoint[0] - chestDistance;

	float leftChestMinDistance = std::numeric_limits<float>::max();
	float rightChestMinDistance = std::numeric_limits<float>::max();

	for(int i = 0; i < points.size(); ++i){
		if(points[i][2] > midZ){
			float distanceFromLeft = std::abs(points[i][0] - leftChestX);
			float distanceFromRight = std::abs(points[i][0] - rightChestX);

			if(distanceFromLeft < leftChestMinDistance){
				leftChestMinDistance = distanceFromLeft;
				leftChestPoint[0] = points[i][0];
	    		leftChestPoint[1] = points[i][1];
	    		leftChestPoint[2] = points[i][2];
			}

			if(distanceFromRight < rightChestMinDistance){
				rightChestMinDistance = distanceFromRight;
				rightChestPoint[0] = points[i][0];
	    		rightChestPoint[1] = points[i][1];
	    		rightChestPoint[2] = points[i][2];
			}
		}
	}
}

// slice the model into 100 section
void Body::sliceModel(){
	double percentage = 0.0f;
	for(MyMesh::FaceIter f_it = bodyMesh.faces_begin(); f_it != bodyMesh.faces_end(); ++f_it) {
		for (MyMesh::FaceVertexIter fv_it=bodyMesh.fv_iter(*f_it); fv_it.is_valid(); ++fv_it)
  		{
  			percentage = std::abs(bodyMesh.point(*fv_it)[1] - bottom) / modelHeight;
	    	int z = (int)(floor(percentage*1000)) ;
	    //	if (z == 1000) z = 999;
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
		std::cout << k <<"x + " << b << " = y" << std::endl;
		
		bool isFound=false;
		for(unsigned int i = 0; i < ankle.size(); ++i){
			// distance = |ax0+by0+c|/sqrt(a^2+b^2)
			float distance = distPT2Line(ankle[i][0], ankle[i][1], k, b);
			std::cout << "Distance is " << distance << " 50*ratio = " << ratio << std::endl;
			if(distance < LEG_DIVISION_CRITIAL_DIST * ratio){
				isFound=true;
				break;
			}
		}
		if(!isFound){
			isSeperated=true;
			std::cout << "found! k is " << k << std::endl;
		}
	}
	std::vector<std::vector<MyMesh::Point> > result(2);
	for(unsigned int i = 0; i < ankle.size(); ++i){
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
	for(unsigned int i = 0; i < ankle.size(); ++i){
		totalX += ankle[i][0];
		totalY += ankle[i][1];
	}
	float centerX = totalX / ankle.size(), centerY = totalY / ankle.size();
	std::cout << "Center X = " << centerX << ", Y = " << centerY << std::endl;
	// Find a line seperating all points into two groups
	// kx1+b=y1, kx2+b=y2, k=(y1-y2)/(x1-x2), b=y2-(x1-x2)/(y1-y2)
	float k = slope(ankle[0][0], ankle[0][1], centerX, centerY);
	std::vector<std::vector<MyMesh::Point> > ankles= getFullDivision(k, centerX, centerY, ankle);
	// Get the center point of each part of cross section
	float ankle1X = 0.0f, ankle1Y = 0.0f;
	for(unsigned int i = 0; i < ankles[0].size(); ++i){
		ankle1X += ankles[0][i][0];
		ankle1Y += ankles[0][i][1];
	}
	ankle1X = ankle1X / ankles[0].size();
	ankle1Y = ankle1Y / ankles[0].size();
	
	float ankle2X = 0.0f, ankle2Y = 0.0f;
	for(unsigned int i = 0; i < ankles[1].size(); ++i){
		ankle2X += ankles[1][i][0];
		ankle2Y += ankles[1][i][1];
	}
	ankle2X = ankle2X / ankles[1].size();
	ankle2Y = ankle2Y / ankles[1].size();
	// k is the left-right direction line slope and 1/k is the front-bak direction
	k = slope(ankle1X, ankle1Y, ankle2X, ankle2Y);

	// Second, use the foot to identify front and back
	std::vector<MyMesh::Point> foot = zSegement[0];
	MyMesh::Point footTip = foot[0];
	float furtherest = std::numeric_limits<float>::min();
	for(unsigned int i = 0; i < foot[i].size(); ++i){
		float dist = distPT2Line(foot[i][0], foot[i][1], k, ankle1Y-k*ankle1X);
		if(dist > furtherest){
			furtherest = dist;
			footTip = foot[i];
		}
	}
	int upward = 1;
	if(footTip[0]*k + ankle1Y-k*ankle1X - footTip[1] < 0){ upward = -1; }

	yCoord = Vector3(upward/k, upward, 0);//front direction
	k = std::tan(std::atan(upward*k)+PI/2);
	xCoord = Vector3(k, 1, 0);// left direction
	zCoord = Vector3(0, 0, 1);// up direction
	std::cout << "x coordniate: " << xCoord.m_x << " " << xCoord.m_y << " " << xCoord.m_z << std::endl;
	std::cout << "y coordniate: " << yCoord.m_x << " " << yCoord.m_y << " " << yCoord.m_z << std::endl;
	std::cout << "z coordniate: " << zCoord.m_x << " " << zCoord.m_y << " " << zCoord.m_z << std::endl;
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
	