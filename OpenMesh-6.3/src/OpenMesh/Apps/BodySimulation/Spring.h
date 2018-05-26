#ifndef SPRING_H
#define SPRING_H

#include <iostream>
#include <functional>
#include "ClothNode.h"
#include "GlobalTypes.h"
class Spring
{
private:
    int type;
    float m_ks, m_kd;
    float m_rest;

public:
    ClothNode* cn1;
    ClothNode* cn2; // cn1->cn2
    
    Spring(ClothNode* c1, ClothNode* c2, int t)
    {
        cn1 = c1;
        cn2 = c2;
        type = t;
        if(type == STRUCT){
            m_ks = UNI_KST; m_kd = DP_KST;
        } else if(type == SHEAR){
            m_ks = UNI_KSH; m_kd = DP_KSH;
        } else {
            m_ks = UNI_KB; m_kd = DP_KB;
        }
        m_rest = (cn1->getPosition() - cn2->getPosition()).length();
    }
    Spring() {}

    // Apply spring force
    void ApplyForce() const
    {
        Vector3 p1 = cn1->getPosition();
        Vector3 p2 = cn2->getPosition();
        Vector3 v1 = cn1->getVelocity();
        Vector3 v2 = cn2->getVelocity();
        Vector3 deltaP = p1-p2;
        Vector3 deltaV = v1-v2;
        float dist = deltaP.length();

        float leftTerm = -m_ks*(dist-m_rest);
        float rightTerm = m_kd*(deltaP.dot(deltaV)/dist);
        Vector3 springForce = deltaP.getNorm()*(leftTerm+rightTerm);
        cn1->addForce(springForce);
        cn2->addForce(-springForce);
        /*Vector3 displacement(cn1->getPosition() - cn2->getPosition());
        float amountUsed = displacement.length();
        Vector3 fSpring = displacement.getNorm() * (m_ks * (amountUsed-m_rest));
        cn1->addForce(-fSpring);
        cn2->addForce(fSpring);*/
    }
    void ApplyProvotDynamicInverse(){
        Vector3 p1 = cn1->getPosition();
        Vector3 p2 = cn2->getPosition();
        Vector3 deltaP =  p1-p2;
        float dist = deltaP.length();
        if(dist > m_rest){
            dist -= m_rest;
            dist /= 2.0f;
            deltaP = deltaP.getNorm();
            deltaP = deltaP*dist;
            cn1->addVelocity(-deltaP);
            cn2->addVelocity(deltaP);
        }/*
        float currLength = (cn1->getPosition()-cn2->getPosition()).length();
        float deformationRate = (currLength - m_rest);// > 0 ? currLength - m_rest : m_rest - currLength;
        if (deformationRate > 0.1 * m_rest) {
            float desiredLength = m_rest * 1.1;

            if (cn1->isFixed() && cn2->isFixed()) {
                return;
            }
            else if (cn1->isFixed()) {
                Vector3 dir = (cn2->getPosition()-cn1->getPosition()).getNorm();
                Vector3 fixedPosition = cn1->getPosition() + dir * desiredLength;
                cn2->setPosition(fixedPosition);
            }
            else if (cn2->isFixed()) {
                Vector3 dir = (cn1->getPosition()-cn2->getPosition()).getNorm();
                Vector3 fixedPosition = cn2->getPosition() + dir * desiredLength;
                cn1->setPosition(fixedPosition);
            }
            else {
                Vector3 dirToNodeTwo = (cn2->getPosition()-cn1->getPosition()).getNorm();
                Vector3 dirToNodeOne = (cn1->getPosition()-cn2->getPosition()).getNorm();

                float distanceToMove = deformationRate / 2;

                Vector3 nodeOneFixedPosition = cn1->getPosition() + dirToNodeTwo * distanceToMove;
                Vector3 nodeTwoFixedPosition = cn1->getPosition() + dirToNodeOne * distanceToMove;

                cn1->setPosition(nodeOneFixedPosition);
                cn2->setPosition(nodeTwoFixedPosition);
            }
        }*/
    }
    float getConstant() { return m_ks; }
    int getType() {return type;}
};

#endif /* end of spring */