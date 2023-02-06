
#include <Vectors.h>

Vector3::Vector3()
{
    x = 0;
    y = 0;
    z = 0;
}

Vector3::Vector3(float X, float Y, float Z)
{
    x = X;
    y = Y;
    z = Z;
}

float Vector3::sin(float angle)
{
    return std::sin(angle);
}

float Vector3::cos(float angle)
{
    return std::cos(angle);
}

void Vector3::RotateByX(float alpha)
{
    float Y = y;
    float Z = z;

    y = Y * cos(alpha) - Z * sin(alpha);
    z = Y * sin(alpha) + Z * cos(alpha);
}

void Vector3::RotateByY(float alpha)
{
    float X = x;
    float Y = y;
    float Z = z;

    x = X * cos(alpha) - Y * sin(alpha);
    z = -X * sin(alpha) + Z * cos(alpha);
}

void Vector3::RotateByZ(float alpha)
{
    float X = x;
    float Y = y;

    x = X * cos(alpha) - Y * sin(alpha);
    y = X * sin(alpha) + Y * cos(alpha);
}

Vector3 Vector3::operator+(const Vector3& foo)
{
    Vector3 vec = Vector3(*this);

    vec.x += foo.x;
    vec.y += foo.y;
    vec.z += foo.z;

    return vec;
}

Vector3& Vector3::operator+=(const Vector3& foo)
{
    this->x += foo.x;
    this->y += foo.y;
    this->z += foo.z;

    return *this;
}

Vector3& Vector3::operator*=(const float& foo)
{
    this->x *= foo;
    this->y *= foo;
    this->z *= foo;

    return *this;
}

string Vector3::ToString()
{
    return string(to_string(x) + " " + to_string(y) + " " + to_string(z));
}
