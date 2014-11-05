#pragma once

#include <ostream>

/*
struct float3
{
    float x, y, z;

    float lenght()
    {
        return sqrt(x*x + y*y + z*z);
    }

    float3 normalize()
    {
        float3 tmp;
        if (x > 0) tmp.x = 1;
        if (x < 0) tmp.x = -1;
        if (x == 0) tmp.x = 0;

        if (y > 0) tmp.y = 1;
        if (y < 0) tmp.y = -1;
        if (y == 0) tmp.y = 0;

        if (z > 0) tmp.z = 1;
        if (z < 0) tmp.z = -1;
        if (z == 0) tmp.z = 0;

        return tmp;
    }
};

struct float4
{
    float x, y, z, w;
};

float3 make_float3(float X, float Y, float Z = - 5.f)
{
    float3 temp = { X, Y, Z };
    return temp;
}

float4 make_float4(float X, float Y, float Z, float W)
{
    float4 temp = { X, Y, Z, W };
    return temp;
}



float3 operator+(const float3& lhs, const float3& rhs)
{
    float3 tmp = { lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z };
    return tmp;
}

float3 operator-(const float3& lhs, const float3& rhs)
{
    float3 tmp = { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z };
    return tmp;
}

float3 operator*(const float3& lhs, const float& rhs)
{
    float3 tmp = { lhs.x * rhs, lhs.y * rhs, lhs.z * rhs };
    return tmp;
}
*/

std::ostream & operator<< (std::ostream &out, const float3& s) 
{
    return out << "(" << s.x << ", " << s.y << ", " << s.z << ")";
}


class Wing
{
    float radius, lenght;
    float3 pos;
    float force;

public:
    Wing()
        : lenght(2.f), radius(0.3f), force(0.f)
    {
        pos = make_float3(0.f, 0.f, 0.f);
    }

    float3 getPos() const { return pos; }
    float getRadius() const { return radius; }
    float getForce() const { return force; }
    void resetForce() { force = 0.f; }
    void addForce(const float yForce) { force += yForce; }
    
};