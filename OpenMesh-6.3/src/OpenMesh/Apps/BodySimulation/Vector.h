#ifndef VECTOR_H
#define VECTOR_H

#include <math.h>

struct Vector3
{
    float m_x, m_y, m_z;

    Vector3(float x, float y, float z): m_x(x), m_y(y), m_z(z) {}
    Vector3(const Vector3& other): m_x(other.m_x), m_y(other.m_y), m_z(other.m_z){}
    Vector3() {}
    
    float length() const
    {
        return sqrt(m_x*m_x + m_y*m_y + m_z*m_z);
    }

    float& operator[](unsigned t)
    {
        if (t==0)
            return m_x;
        if (t==1)
            return m_y;
        if (t==2)
            return m_z;

        return m_x;
    }

    float operator[](unsigned t) const
    {
        if (t==0)
            return m_x;
        if (t==1)
            return m_y;
        if (t==2)
            return m_z;
        
        return m_x;
    }

    bool operator==(Vector3 vec) const 
    {
        return (m_x == vec.m_x) && (m_y == vec.m_y) && (m_z == vec.m_z);
    }
    friend std::ostream& operator << (std::ostream& o, const Vector3& vec) {
        return o << "x: "<<vec.m_x << " y:" << vec.m_y << " z:" << vec.m_z;
    }
    Vector3 getNorm() const
    {
        float l = length();
        return Vector3(m_x / l, m_y / l, m_z /l);
    }

    void normalize()
    {
        float l = length();
        m_x /= l;
        m_y /= l;
        m_z /= l;
    }

    Vector3& operator=(const Vector3 &v)
    {
        m_x = v.m_x;
        m_y = v.m_y;
        m_z = v.m_z;

        return *this;
    }

    void operator+= (const Vector3 &v)
    {
        m_x += v.m_x;
        m_y += v.m_y;
        m_z += v.m_z;
    }
    
    Vector3 operator/ (const float a) const
    {
        return Vector3(m_x/a, m_y/a, m_z/a);
    }
    
    Vector3 operator- (const Vector3 &v) const
    {
        return Vector3(m_x-v.m_x, m_y-v.m_y, m_z-v.m_z);
    }
    
    Vector3 operator+ (const Vector3 &v) const
    {
        return Vector3(m_x+v.m_x, m_y+v.m_y, m_z+v.m_z);
    }
    
    Vector3 operator* (const float a) const
    {
        return Vector3(m_x*a, m_y*a, m_z*a);
    }
    
    Vector3 operator-() const
    {
        return Vector3(-m_x,-m_y,-m_z);
    }
    
    Vector3 cross(const Vector3 &v) const
    {
        return Vector3(m_y*v.m_z - m_z*v.m_y, m_z*v.m_x - m_x*v.m_z, m_x*v.m_y - m_y*v.m_x);
    }
    
    float dot(const Vector3 &v) const
    {
        return m_x*v.m_x + m_y*v.m_y + m_z*v.m_z;
    }

    static Vector3 Zero()
    {
        return Vector3(0.0f, 0.0f, 0.0f);
    }

    static Vector3 Identity()
    {
        return Vector3(1.0f, 1.0f, 1.0f);
    }
};

namespace Vec
{
    const static Vector3 Zero(0.0f, 0.0f, 0.0f);
    const static Vector3 Identity(1.0f, 1.0f, 1.0f);
};

#endif