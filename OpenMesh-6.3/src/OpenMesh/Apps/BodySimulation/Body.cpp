#include "Body.h"
#include <limits>
#include <cmath>
#include <fstream>

#define MEAN_ANKLE_HEIGHT 10
#define LEG_DIVISION_CRITIAL_DIST 5
#define PI 3.1415926
#define ARC_STEP 12// 15 degree
#define X 0
#define Y 1
#define Z 2
Body::Body(MyMesh mesh, float height){
	bodyMesh = mesh;
	this->height = height;
	getModelHeight();
	sliceModel();
	bodyDirection();
	bodyRotation();
	kdTree.init(bodyMesh);
	calLandmarks();
	std::ofstream ofile("benchmark.txt");
	ofile << croctchPT << " (croctch)" << std::endl;
	ofile << leftArmpitPT << " (left armpit)" << std::endl;
	ofile << rightArmpitPT << " (right armpit)" << std::endl;
	ofile << leftShoulderPT << " (left shoulder)" << std::endl;
    ofile << rightShoulderPT << " (right shoulder)" << std::endl;
    ofile << leftChestPT << " (left chest)" << std::endl;
    ofile << rightChestPT << " (right chest)" << std::endl;
	ofile << leftHipPT << " (left hip)" << std::endl;
    ofile << rightHipPT << " (right hip)" << std::endl;
    ofile << modelWaist/ratio << " (waist)" << std::endl;
    ofile.close();
}
//-------------------------Preprocess----------------------------//
// iterator through all the vertice and find the highest and lowest point
void Body::getModelHeight(){
	top = std::numeric_limits<float>::min();
	bottom = std::numeric_limits<float>::max();
	right = std::numeric_limits<float>::min();
	left = std::numeric_limits<float>::max();
	for (MyMesh::VertexIter v_it=bodyMesh.vertices_begin(); v_it!=bodyMesh.vertices_end(); ++v_it)
	{
		if(top < bodyMesh.point(*v_it)[Y]) top = bodyMesh.point(*v_it)[Y];
	    if(bottom > bodyMesh.point(*v_it)[Y]) bottom = bodyMesh.point(*v_it)[Y];
	    if(right < bodyMesh.point(*v_it)[X]) right = bodyMesh.point(*v_it)[X];
		if(left > bodyMesh.point(*v_it)[X]) left = bodyMesh.point(*v_it)[X];   
	}
    modelHeight = std::abs(top-bottom);
    modelWidth = std::abs(right-left);
    ratio = modelHeight / height;
}

// slice the model into 100 section
void Body::sliceModel(){
	float percentage = 0.0f;
	for (MyMesh::VertexIter v_it=bodyMesh.vertices_begin(); v_it!=bodyMesh.vertices_end(); ++v_it)
	{
		percentage = std::abs(bodyMesh.point(*v_it)[Y] - bottom) / modelHeight;
    	int y = (int)(percentage*100);
		if(y == 100) y = 99;
	   	ySegment[y].push_back(bodyMesh.point(*v_it));
    	percentage = std::abs(bodyMesh.point(*v_it)[X] - left) / modelWidth;
    	int x = (int)(percentage*100);
		if(x == 100) x = 99;
	   	xSegment[x].push_back(bodyMesh.point(*v_it));
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
		for(unsigned int i = 0; i < ankle.size(); ++i){
			// distance = |ax0+by0+c|/sqrt(a^2+b^2)
			float distance = distPT2Line(ankle[i][X], ankle[i][Z], k, b);
			if(distance < LEG_DIVISION_CRITIAL_DIST * ratio){
				isFound=true;
				break;
			}
		}
		if(!isFound){
			isSeperated=true;
		}
	}
	std::vector<std::vector<MyMesh::Point> > result(2);
	for(unsigned int i = 0; i < ankle.size(); ++i){
		if(k*ankle[i][X]-ankle[i][Z]+b > 0){
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
	float totalX = 0.0f, totalZ = 0.0f;
	std::vector<MyMesh::Point> ankle = ySegment[MEAN_ANKLE_HEIGHT-1];
	for(unsigned int i = 0; i < ankle.size(); ++i){
		totalX += ankle[i][X];
		totalZ += ankle[i][Z];
	}
	float centerX = totalX / ankle.size(), centerZ = totalZ/ ankle.size();
	// Find a line seperating all points into two groups
	// kx1+b=y1, kx2+b=y2, k=(y1-y2)/(x1-x2), b=y2-(x1-x2)/(y1-y2)
	float k = slope(ankle[0][X], ankle[0][Z], centerX, centerZ);
	std::vector<std::vector<MyMesh::Point> > ankles = getFullDivision(k, centerX, centerZ, ankle);
	// Get the center point of each part of cross section
	float ankle1X = 0.0f, ankle1Z = 0.0f;
	for(unsigned int i = 0; i < ankles[0].size(); ++i){
		ankle1X += ankles[0][i][X];
		ankle1Z += ankles[0][i][Z];
	}
	ankle1X = ankle1X / ankles[0].size();
	ankle1Z = ankle1Z / ankles[0].size();
	
	float ankle2X = 0.0f, ankle2Z = 0.0f;
	for(unsigned int i = 0; i < ankles[1].size(); ++i){
		ankle2X += ankles[1][i][X];
		ankle2Z += ankles[1][i][Z];
	}
	ankle2X = ankle2X / ankles[1].size();
	ankle2Z = ankle2Z / ankles[1].size();
	// k is the left-right direction line slope and 1/k is the front-bak direction
	k = slope(ankle1X, ankle1Z, ankle2X, ankle2Z);
		
	// Second, use the foot to identify front and back
	std::vector<MyMesh::Point> foot = ySegment[0];
	MyMesh::Point footTip = foot[0];
	float furtherest = std::numeric_limits<float>::min();
	for(unsigned int i = 0; i < foot[i].size(); ++i){
		float dist = distPT2Line(foot[i][X], foot[i][Z], k, ankle1Z-k*ankle1X);
		if(dist > furtherest){
			furtherest = dist;
			footTip = foot[i];
		}
	}
	int upward = 1;
	if(footTip[X]*k + ankle1Z-k*ankle1X - footTip[Z] < 0){ upward = -1; }

	zCoord = Vector3(1, 0, -upward/k);//front direction
	k = std::tan(std::atan(-upward/k)+PI/2);
	xCoord = Vector3(1, 0, k);// left direction
	yCoord = Vector3(0, 1, 0);// up direction
}

void Body::bodyRotation(){
	Vector3 zc(0,0,-1);
	float angle = std::acos(zc.dot(zCoord)/(zc.length()*zCoord.length()));
	float sin_t = std::sin(angle);
  	float cos_t = std::cos(angle);
  	for (MyMesh::VertexIter v_it=bodyMesh.vertices_begin(); v_it!=bodyMesh.vertices_end(); ++v_it)
	{
		float x = bodyMesh.point(*v_it)[0];
  		float y = bodyMesh.point(*v_it)[1];
  		float z = bodyMesh.point(*v_it)[2];
  		bodyMesh.point(*v_it)[0] = x * cos_t - z * sin_t;
        bodyMesh.point(*v_it)[1] = y;
  		bodyMesh.point(*v_it)[2] = z * cos_t + x * sin_t;
	}
	for(int i = 0; i < 100; i++){
		for(unsigned j = 0; j < xSegment[i].size(); j++){
			float x = xSegment[i][j][0];
  			float y = xSegment[i][j][1];
  			float z = xSegment[i][j][2];
  			xSegment[i][j][0] = x * cos_t - z * sin_t;
        	xSegment[i][j][1] = y;
  			xSegment[i][j][2] = z * cos_t + x * sin_t;
		}
	}
	for(int i = 0; i < 100; i++){
		for(unsigned j = 0; j < ySegment[i].size(); j++){
			float x = ySegment[i][j][0];
  			float y = ySegment[i][j][1];
  			float z = ySegment[i][j][2];
  			ySegment[i][j][0] = x * cos_t - z * sin_t;
        	ySegment[i][j][1] = y;
  			ySegment[i][j][2] = z * cos_t + x * sin_t;
		}
	}
}

//-------------------------Process----------------------------//
// automatic get landmarks
void Body::calLandmarks(){
	calCroctch();
	calArmpits();
	calWaist();
	calShoulderPT();
	calHip();
	calChest();
}

MyMesh::Point lowestInSection(std::vector<MyMesh::Point> section){
	if(section.size() == 0) return MyMesh::Point(100,-100,100);
	MyMesh::Point lowest = section[0];
	for(unsigned i = 0; i < section.size(); i++){
		if(lowest[Y] > section[i][Y]) lowest = section[i];
	}
	return lowest;
}
void Body::calCroctch(){
	croctchPT = xSegment[45][0];
	for(int i = 45; i <= 55; i++){
		MyMesh::Point temp = lowestInSection(xSegment[i]);
		if(croctchPT[Y] < temp[Y]) croctchPT = temp; 
	}
	landmarks.push_back(croctchPT);
}

std::vector<MyMesh::Point> Body::integrate(int segStart, int segEnd, int direction/*0 for Y, 1 for X*/){
	std::vector<std::vector<MyMesh::Point> > resource;
	std::vector<MyMesh::Point> result;
	if(direction == 0){
		resource = ySegment;
	} else {
		resource = xSegment;
	}
	for(int i = segStart; i <= segEnd; i++){
		for(unsigned j = 0; j < resource[i].size(); j++){
			result.push_back(resource[i][j]);
		}
	}
	return result;
}

// current using the dumbest way
std::vector<std::vector<MyMesh::Point> > Body::segIntersect(int segYStart, int segYEnd, int segXStart, int segXEnd, int cutModel){
	std::vector<MyMesh::Point> area;
	float left  = std::numeric_limits<float>::max();
	float right  = std::numeric_limits<float>::min();
	float bottom  = std::numeric_limits<float>::max();
	float top  = std::numeric_limits<float>::min();
	for(int i = segYStart; i <= segYEnd; i++){
		for(unsigned s = 0; s< ySegment[i].size(); s++){
			for(int j = segXStart; j <= segXEnd; j++){
				bool isBreak = false;
				for(unsigned t = 0; t < xSegment[j].size(); t++){
					if(ySegment[i][s][0] == xSegment[j][t][0] &&
						ySegment[i][s][1] == xSegment[j][t][1] && 
						ySegment[i][s][2] == xSegment[j][t][2])
					{	
						area.push_back(ySegment[i][s]);
						if(cutModel == 1){
							if(left > ySegment[i][s][X]) left = ySegment[i][s][X];
							if(right < ySegment[i][s][X]) right = ySegment[i][s][X];
						} else {
							if(bottom > ySegment[i][s][Y]) bottom = ySegment[i][s][Y];
							if(top < ySegment[i][s][Y]) top = ySegment[i][s][Y];	
						}
						isBreak = true;
						break;
					}
				}
				if(isBreak) break;
			}
		}
	}
	std::vector<std::vector<MyMesh::Point> > result(100);
	if(cutModel == 1){
		float width = std::abs(left-right);
		for(unsigned i = 0; i < area.size(); i++){
			int index = std::abs(area[i][X]-left) * 100 / width;
			if(index == 100) index = 99;
			result[index].push_back(area[i]);
		}
	} else {
		float height = std::abs(top-bottom);
		for(unsigned i = 0; i < area.size(); i++){
			int index = std::abs(area[i][Y]-bottom) * 100 / height;
			if(index == 100) index = 99;
			result[index].push_back(area[i]);
		}
	}
	return result;
}

void Body::calArmpits(){
	std::vector<std::vector<MyMesh::Point> > leftArmpitArea = segIntersect(65, 70, 20, 40, 1);
	leftArmpitPT = leftArmpitArea[0][0];
	leftArmpitPT[Y] = std::numeric_limits<float>::min();
	for(unsigned i = 0; i < leftArmpitArea.size(); i++){
		MyMesh::Point temp = lowestInSection(leftArmpitArea[i]);
		 if(leftArmpitPT[Y] < temp[Y]) leftArmpitPT = temp;
	}
	
	std::vector<std::vector<MyMesh::Point> > rightArmpitArea = segIntersect(65, 70, 60, 80, 1);
	rightArmpitPT = rightArmpitArea[0][0];
	rightArmpitPT[Y] = std::numeric_limits<float>::min();
	for(unsigned i = 0; i < rightArmpitArea.size(); i++){
		MyMesh::Point temp = lowestInSection(rightArmpitArea[i]);
		if(rightArmpitPT[Y] < temp[Y]) rightArmpitPT = temp;
	}
	landmarks.push_back(leftArmpitPT);
	landmarks.push_back(rightArmpitPT);
}

void Body::calWaist(){
	std::vector<std::vector<MyMesh::Point> > waistArea = segIntersect(45, 60, 20, 80, 0);
	float left = std::numeric_limits<float>::max();
	float right = std::numeric_limits<float>::min();
	float back = std::numeric_limits<float>::max();
	float front = std::numeric_limits<float>::min();
	modelWaist = std::numeric_limits<float>::max();
	for(unsigned i = 0; i < waistArea.size(); i++){
		for(unsigned j = 0; j < waistArea[i].size(); j++){
			if(left > waistArea[i][j][X]) left = waistArea[i][j][X];
			if(right < waistArea[i][j][X]) right = waistArea[i][j][X];
			if(back > waistArea[i][j][Z]) back = waistArea[i][j][Z];
			if(front > waistArea[i][j][Z]) front = waistArea[i][j][Z];
		}
		float a = std::abs(right-left);
		float b = std::abs(front-back);
		float thiswaist = PI*(3*(a+b)-std::sqrt((3*a+b)*(a+3*b)));
		if(thiswaist < modelWaist) modelWaist = thiswaist;
	}
}

void Body::calShoulderPT(){
	std::vector<MyMesh::Point> points = integrate(79, 82, 0);
	leftShoulderPT = points[points.size()-1], rightShoulderPT = points[0];
	leftShoulderPT[X] = std::numeric_limits<float>::max();
	rightShoulderPT[X] = std::numeric_limits<float>::min();
	for (unsigned i = 0; i < points.size(); ++i){
		if(leftShoulderPT[X] > points[i][X]) {
			leftShoulderPT = points[i];
    	}
    	if(rightShoulderPT[X] < points[i][X]) {
    		rightShoulderPT = points[i];
    	}
	}
	landmarks.push_back(leftShoulderPT);
	landmarks.push_back(rightShoulderPT);
}

void Body::calHip(){
	std::vector<std::vector<MyMesh::Point> > leftHipArea = segIntersect(40, 48, 20, 40, 0);
	leftHipPT[Z] = std::numeric_limits<float>::max();
	for(unsigned i = 0; i < leftHipArea.size(); i++){
		for(unsigned j = 0; j < leftHipArea[i].size(); j++){
			if(leftHipPT[Z] > leftHipArea[i][j][Z]) leftHipPT = leftHipArea[i][j];
		}
	}

	std::vector<std::vector<MyMesh::Point> > rightHipArea = segIntersect(40, 48, 60, 80, 0);
	rightHipPT[Z] = std::numeric_limits<float>::max();
	for(unsigned i = 0; i < rightHipArea.size(); i++){
		for(unsigned j = 0; j < rightHipArea[i].size(); j++){
			if(rightHipPT[Z] > rightHipArea[i][j][Z]) rightHipPT = rightHipArea[i][j];
		}
	}
	landmarks.push_back(leftHipPT);
	landmarks.push_back(rightHipPT);
}

//find hip must be called before this method
//because it's assumed that the armpit x coordinates
//is roughly the same as the hips coordinate, and
//this is an approximation of the chests as being at the
//mid point between each sides of the body and the center 
//of the body
void Body::calChest(){
	std::vector<std::vector<MyMesh::Point> > chestArea = segIntersect(65, 70, 30, 70, 0);
	float left = std::numeric_limits<float>::max();
	float right = std::numeric_limits<float>::min();
	float back = std::numeric_limits<float>::max();
	float front = std::numeric_limits<float>::min();
	modelChest = std::numeric_limits<float>::max();
	for(unsigned i = 0; i < chestArea.size(); i++){
		for(unsigned j = 0; j < chestArea[i].size(); j++){
			if(left > chestArea[i][j][X]) left = chestArea[i][j][X];
			if(right < chestArea[i][j][X]) right = chestArea[i][j][X];
			if(back > chestArea[i][j][Z]) back = chestArea[i][j][Z];
			if(front > chestArea[i][j][Z]) front = chestArea[i][j][Z];
		}
		float a = std::abs(right-left);
		float b = std::abs(front-back);
		float thischest = PI*(3*(a+b)-std::sqrt((3*a+b)*(a+3*b)));
		if(thischest < modelChest) modelChest = thischest;
	}

	/*
	float ymax = std::numeric_limits<float>::min();
	float ymin = std::numeric_limits<float>::max();

	std::vector<MyMesh::Point> points = ySegment[71];// 712
	//find mid point of y
	for (unsigned i = 0; i < points.size(); ++i){
		if(ymax < points[i][2]) {
    		ymax = points[i][2];
    	}
    	if(ymin > points[i][2]) {
    		ymin = points[i][2];
		}
	}
	float midY = ymin + (std::abs(ymax - ymin) / 2.0);
	
	float width = std::abs(leftHipPT[X] - rightHipPT[X]);
	float chestDistance = width / 4;
	float leftChestX = leftHipPT[X] + chestDistance;
	float rightChestX = rightHipPT[X] - chestDistance;

	float leftChestMinDistance = std::numeric_limits<float>::max();
	float rightChestMinDistance = std::numeric_limits<float>::max();

	for(unsigned i = 0; i < points.size(); ++i){
		if(points[i][2] > midY){
			float distanceFromLeft = std::abs(points[i][0] - leftChestX);
			float distanceFromRight = std::abs(points[i][0] - rightChestX);

			if(distanceFromLeft < leftChestMinDistance){
				leftChestMinDistance = distanceFromLeft;
				leftChestPT = points[i];
			}

			if(distanceFromRight < rightChestMinDistance){
				rightChestMinDistance = distanceFromRight;
				rightChestPT = points[i];
			}
		}
	}
	landmarks.push_back(leftChestPT);
	landmarks.push_back(rightChestPT);*/
}


bool Body::isCollided(const Vector3& vertex)
{
    bool result = false;
    Vector3 localClosestPoint = Vec::Zero;
    double localDistance = 0.0;

    // retrive the closest point in body mesh from the given vertex
    kdTree.getClosestPoint(vertex, localClosestPoint, localDistance);

    // displacement vector
    OpenMesh::Vec3f uv;

    uv[0] = localClosestPoint[0] - vertex[0];
    uv[1] = localClosestPoint[1] - vertex[1];
    uv[2] = localClosestPoint[2] - vertex[2];

    MyMesh::VertexIter vIt,vBegin,vEnd;
    vBegin = bodyMesh.vertices_begin();
    vEnd = bodyMesh.vertices_end();
    for (vIt = vBegin; vIt != vEnd; ++vIt)
    {
        if( bodyMesh.point(*vIt)[0] == localClosestPoint[0] &&
           	bodyMesh.point(*vIt)[1] == localClosestPoint[1] &&
            bodyMesh.point(*vIt)[2] == localClosestPoint[2] )
            {
                break;
            }
    }

    // normal for the closest point
    OpenMesh::Vec3f normal = bodyMesh.normal(*vIt);

    float dotResult = OpenMesh::dot(normal, uv);

    if(dotResult > 0.0f)
    {
        result = true;
    }

    return result;
}