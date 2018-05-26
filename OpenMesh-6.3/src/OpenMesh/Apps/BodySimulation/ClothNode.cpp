#include "ClothNode.h"

ClothNode::ClothNode(Vector3 vec, int idx, Body* b,float m) {
	body = b;
	index = idx;
	position = Vector3(vec.m_x, vec.m_y, vec.m_z);
	oldPosition = position;
    velocity = Vec::Zero;
    oldVelocity = velocity;
    mass = m;
    mFixed = false;
    force = Vec::Zero;
}

ClothNode::ClothNode(ClothNode* cn){
	position = cn->getPosition();
	oldPosition = position;
	velocity = cn->getVelocity();
	oldVelocity = velocity;
	force = cn->getForce();
	structAdjacents = cn->getStructAdjacent();
	shearAdjacents = cn->getShearAdjacent();
	bendAdjacents = cn->getBendAdjacent();
	mass = cn->getMass();
}

void barycentricCoord(Vector3 a, Vector3 b, Vector3 c, Vector3 p, float &w1, float &w2, float &w3) {
    Vector3 v0 = b - a, v1 = c - a, v2 = p - a;
    float d00 = v0.dot(v0);
    float d01 = v0.dot(v1);
    float d11 = v1.dot(v1);
    float d20 = v2.dot(v0);
    float d21 = v2.dot(v1);
    float denom = d00 * d11 - d01 * d01;
    w2 = (d11 * d20 - d01 * d21) / denom;
    w3 = (d00 * d21 - d01 * d20) / denom;
    w1 = 1.0f - w2 - w3;
}

Vector3* repulsionResponse(ClothFace* cf, ClothNode* cn, float deltaTime){
	Vector3* result = new Vector3[4]; for(int i = 0; i < 4; ++i) result[i] = Vec::Zero;
	float w1, w2, w3;
	Vector3 x1 = cf->cn1->getOldPosition();
	Vector3 x2 = cf->cn2->getOldPosition();
	Vector3 x3 = cf->cn3->getOldPosition();
	Vector3 x4 = cn->getOldPosition();
	Vector3 n = cf->normal;
	barycentricCoord(x1, x2, x3, x4 , w1, w2, w3);
	Vector3 inPlane = x4 - x1*w1 - x2*w2 - x3*w3;
	float d = THICKNESS - inPlane.dot(n);
	float f1 = d * deltaTime * UNI_KST;
	float f2 = (0.1*d)/deltaTime-cn->getOldVelocity().dot(n)*cn->getMass(); // the normal component of relative velocity
	if(f2 <= 0) return result;
	Vector3 Ir = f1 < f2 ? n*(-1*f1) : n*(-1*f2);

	Vector3 I = Ir/(1+w1*w1+w2*w2+w3*w3)*2;
	result[0] = (I/cf->cn1->getMass()*w1);
	result[1] = (I/cf->cn2->getMass()*w2);
	result[2] = (I/cf->cn3->getMass()*w3);
	result[3] = -(I/cn->getMass());
	return result;
}

void ClothNode::update(float deltaTime){
	velocity = averageVelocity;
	averageVelocity = Vec::Zero;
	position += velocity * deltaTime;
}

void ClothNode::timeStep(float deltaTime){
	
	// Advance to candidate positions -Xn+1 and velocities -Vn+1 at time Tn+1 with the clothinternal dynamics
	//oldVelocity = velocity;
	//std::cout << "Position: " << position << ", Velocity: " << velocity << ", Force: " << force << ", Mass: " << mass << std::endl;
	//std::cout << "Force/mass = " << force.m_x/mass << std::endl << std::endl;
	//velocity += force / mass * deltaTime;	oldPosition = position;
	//position += velocity * deltaTime;
	// Compute the average velocity -Vn+1/2
	//averageVelocity = (position-oldPosition)/deltaTime;
	//std::cout << "Before collsion"<<std::endl;
	//printf("x:%f, y:%f, z:%f", position.m_x, position.m_y, position.m_z);
	
	//std::cout << "Before Position: " << position << ", Velocity: " << velocity << ", Force: " << force << ", Mass: " << mass << std::endl;
	/*Vector3 response = body->repulsionResponse(position, velocity);
	Vector3 tangential = velocity - response;
	if(tangential.length()>=response.length()*FRICTION_COEFFICIENT){
		velocity = tangential - tangential.getNorm()*response.length()*FRICTION_COEFFICIENT - response*DISSIPATION_COEFFICIENT;
	} else {
		velocity = -response*DISSIPATION_COEFFICIENT;
	}
	if(response == Vector3(0,0,0)) collided = false;
	else {
		std::cout << "Position = " << position << std::endl;
		std::cout << "Velocity = " << velocity << std::endl;
		std::cout << "Response = " << response << std::endl <<std::endl;
		collided = true;
	}
	position += velocity * deltaTime;
	
	/*force = force + Vector3(0, -GRAVITY, 0)*mass;
	force = force + velocity*-0.0125f;
	force = force / mass;
	oldVelocity = velocity;
  	velocity = velocity + force * deltaTime;
  	Vector3 response = body->repulsionResponse(position, velocity);
	if(!(response==Vector3(0,0,0))) {
  		position = position;
  	}else {
  		oldPosition = position;
  		position = position + (oldVelocity + velocity) * 0.5f * deltaTime;
  	}

  	force = Vec::Zero;*/
	
	float deltaTimeMass = deltaTime/mass;
	velocity = velocity + force*deltaTimeMass;
	oldPosition = position;
	position = position + velocity*deltaTime;
  	Vector3 response = body->repulsionResponse(position, velocity);
	if(!(response==Vector3(0,0,0))) {
  		position = oldPosition;//position - velocity*deltaTime;
  		velocity = Vector3(0,0,0);
  	}  	
	//std::cout << "After Position: " << position << ", Velocity: " << velocity << ", Force: " << force << ", Mass: " << mass << std::endl;
	//char a;
	//std::cin >> a; 
    //std::cout << "Force " << force << std::endl;
	//Check Xn for proximity, then apply repulsion impulses and friction to the average velocity to get ~Vn+1/2
	
	
	/*
	averageVelocity += response/getMass();
    if (response.m_x != 0 || response.m_y != 0 || response.m_z != 0) {
      averageVelocity = averageVelocity*0.1f;
      //velocity = vec3(0,0,0);
    }*/
    //printf("Response: x:%f, y:%f, z:%f\n", response.m_x, response.m_y, response.m_z);
	// right now using grid, should change to BVH
	/*for(std::set<ClothFace*>::iterator it = neighborFaces.begin(); it != neighborFaces.end(); ++it){
		ClothFace* cf = *it;
		if(cf->cn1->getIndex() == getIndex() || cf->cn2->getIndex() == getIndex() || cf->cn3->getIndex() == getIndex()) continue;
		Vector3 normal = cf->normal;
		Vector3 v = cf->cn1->getOldPosition();
		v = v-oldPosition;
		float distance = (v - normal * v.dot(normal)).length();
		if(distance < THICKNESS) {
			// Self-Collision
			Vector3* response = repulsionResponse(cf, this, deltaTime);
			cf->cn1->averageVelocity += response[0];
			cf->cn2->averageVelocity += response[1];
			cf->cn3->averageVelocity += response[2];
			averageVelocity += response[3];
			Vector3 tangentialVelocity = normal*oldVelocity.dot(normal) + oldVelocity;
			if(response[3].length()*FRICTION_COEFFICIENT<tangentialVelocity.length()){
				averageVelocity += tangentialVelocity*(1-response[3].length()*FRICTION_COEFFICIENT/tangentialVelocity.length());
			}
		} 
	}
*/
    
}

