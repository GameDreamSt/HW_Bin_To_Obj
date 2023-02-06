#pragma once

#include <string>
#include <cmath>

using namespace std;

class Vector3
{
public:
    float x;
    float y;
    float z;

    Vector3();
    Vector3(float X, float Y, float Z);

    float sin(float angle);

    float cos(float angle);

    void RotateByX(float alpha);

    void RotateByY(float alpha);

    void RotateByZ(float alpha);

    Vector3 operator +(const Vector3& foo);
    Vector3& operator +=(const Vector3& foo);

    Vector3& operator *=(const float& foo);

    string ToString();
};