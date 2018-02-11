#ifndef SPRING_H
#define SPRING_H

#include "Particle.h"

class Spring
{
private:
    float m_ks;
    float m_rest;

public:
    Particle *p1, *p2; // p1->p2
    
    explicit Spring(Particle *p1, Particle *p2, float ks) : m_ks(ks), p1(p1), p2(p2)
    {
        m_rest = Vector3(p1->getPos() - p2->getPos()).length();
    }
    explicit Spring() {}

    // Apply spring force
    void ApplyForce() const
    {
        Vector3 displacement(p1->getPos() - p2->getPos());
        Vector3 fSpring = displacement.getNorm() * m_ks * (displacement.length() - m_rest);

        p1->addForce(-fSpring);
        p2->addForce(fSpring);
    }

    float getConstant() { return m_ks; }
};

#endif /* end of spring */