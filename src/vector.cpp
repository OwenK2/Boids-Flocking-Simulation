#include "vector.h"
#include <cmath>

#include <cstdio>

Vector::Vector() {}
Vector::Vector(Vector& v) {x = v.x;y = v.y;z = v.z;}
Vector::Vector(float _a) : x(_a), y(_a), z(0) {}
Vector::Vector(float _x, float _y) : x(_x), y(_y), z(0) {}
Vector::Vector(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
Vector::~Vector() {}


bool Vector::operator==(Vector a) {
	return (x == a.x && y == a.y && z == a.z);
}
bool Vector::operator==(Vector* a) {
	return (x == a->x && y == a->y && z == a->z);
}
bool Vector::operator!=(Vector a) {
	return (x != a.x || y != a.y || z != a.z);
}
bool Vector::operator!=(Vector* a) {
	return (x != a->x || y != a->y || z != a->z);
}
void Vector::operator=(Vector a) {
	x = a.x;
	y = a.y;
	z = a.z;
}
void Vector::operator=(Vector* a) {
	x = a->x;
	y = a->y;
	z = a->z;
}

void Vector::operator+=(Vector a) {
	x += a.x;
	y += a.y;
	z += a.z;
}
void Vector::operator+=(Vector* a) {
	x += a->x;
	y += a->y;
	z += a->z;
}
void Vector::operator+=(int a) {
	x += a;
	y += a;
	z += a;
}
void Vector::operator+=(float a) {
	x += a;
	y += a;
	z += a;
}

void Vector::operator-=(Vector a) {
	x -= a.x;
	y -= a.y;
	z -= a.z;
}
void Vector::operator-=(Vector* a) {
	x -= a->x;
	y -= a->y;
	z -= a->z;
}
void Vector::operator-=(int a) {
	x -= a;
	y -= a;
	z -= a;
}
void Vector::operator-=(float a) {
	x -= a;
	y -= a;
	z -= a;
}

void Vector::operator*=(Vector a) {
	x *= a.x;
	y *= a.y;
	z *= a.z;
}
void Vector::operator*=(Vector* a) {
	x *= a->x;
	y *= a->y;
	z *= a->z;
}
void Vector::operator*=(int a) {
	x *= a;
	y *= a;
	z *= a;
}
void Vector::operator*=(float a) {
	x *= a;
	y *= a;
	z *= a;
}

void Vector::operator/=(Vector a) {
	x /= a.x;
	y /= a.y;
	z /= a.z;
}
void Vector::operator/=(Vector* a) {
	x /= a->x;
	y /= a->y;
	z /= a->z;
}
void Vector::operator/=(int a) {
	x /= a;
	y /= a;
	z /= a;
}
void Vector::operator/=(float a) {
	x /= a;
	y /= a;
	z /= a;
}

void Vector::normalize() {
	float m = mag();
	if(m != 0) {
		x /= m;
		y /= m;
		z /= m;
	}
}
void Vector::log() {
	printf("[%f, %f, %f]\n", x, y, z);
}
void Vector::limit(int l) {
  float m = mag();
  if(m > l) {
    if(m != 0) {x /= m;y /= m;z /= m;}
    x *= l;y *= l;z *= l;
  }
}
void Vector::limit(float l) {
  float m = mag();
  if(m > l) {
    if(m != 0) {x /= m;y /= m;z /= m;}
    x *= l;y *= l;z *= l;
  }
}
void Vector::setMag(int mg) {
  normalize();
  float m = (float)mg;
  x *= m;y *= m;z *= m;
}
void Vector::setMag(float m) {
  normalize();
  x *= m;y *= m;z *= m;
}
float Vector::magSq() {
	return x*x + y*y + z*z;
}
float Vector::mag() {
	return sqrt(magSq());
}

