#ifndef BOID_H
#define BOID_H

#define ARGON_INCLUDE_GFX
#include "Argon.h"
#include <vector>

struct vec2 {
	double x = 0, y = 0;
	vec2() {}
	vec2(double x, double y) : x(x), y(y) {}
	vec2(const vec2& v) : x(v.x), y(v.y) {}
	double angle() {return atan2(y, x);}
	double mag() {return sqrt(x*x + y*y);}
	double mag2() {return x*x + y*y;}
	vec2& norm() {*this /= mag();return *this;}
	vec2& setMag(double c) {*this = *this / mag() * c;return *this;}
	vec2& limit(double c) {double m = mag(); if(m > c) {setMag(c);}return *this;}
	vec2 operator+(double c) {vec2 res = *this;res.x += c; res.y += c;return res;}
	vec2 operator-(double c) {vec2 res = *this;res.x -= c; res.y -= c;return res;}
	vec2 operator*(double c) {vec2 res = *this;res.x *= c; res.y *= c;return res;}
	vec2 operator/(double c) {vec2 res = *this;res.x /= c; res.y /= c;return res;}
	vec2& operator+=(double c) {x += c; y += c;return *this;}
	vec2& operator-=(double c) {x -= c; y -= c;return *this;}
	vec2& operator*=(double c) {x *= c; y *= c;return *this;}
	vec2& operator/=(double c) {x /= c; y /= c;return *this;}
	vec2 operator+(vec2 v) {vec2 res = *this;res.x += v.x; res.y += v.y;return res;}
	vec2 operator-(vec2 v) {vec2 res = *this;res.x -= v.x; res.y -= v.y;return res;}
	vec2& operator+=(vec2 v) {x += v.x; y += v.y;return *this;}
	vec2& operator-=(vec2 v) {x -= v.x; y -= v.y;return *this;}
	vec2& operator=(double c) {x = c; y = c;return *this;}
	vec2& operator=(const vec2& v) {x = v.x; y = v.y; return *this;}

	static double distance(const vec2& a, const vec2& b) {
		return sqrt((a.x - b.x) * (a.x - b.x)  + (a.y - b.y) * (a.y - b.y));
	}
	static double distance2(const vec2& a, const vec2& b) {
		return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y);
	}
};



struct Boid {
	static constexpr uint32_t colors[] = {0xf1c40fff, 0xe74c3cff, 0x9b59b6ff, 0x3498dbff, 0x2ecc71ff, 0x1abc9cff, 0xd35400ff};
	static constexpr uint8_t numColors = sizeof(colors) / sizeof(uint32_t);
	static constexpr double size = 10.0;
	static constexpr double speed = 200.0;
	static constexpr double perception = 50 * 50; //squared for faster calculation
	static constexpr double seperationDistance = (size*3)*(size*3); //squared for faster calculation
	static constexpr double cohesionForce = .01;
	static constexpr double alignmentForce = 1;
	static constexpr double seperationForce = 5;
	static constexpr double edgeFearRange = 20;
	static constexpr double edgeTurnForce = 1;
	vec2 pos;
	vec2 vel;
	uint32_t color;

	Boid(double x, double y, double vx, double vy);

	void update(double dt, std::vector<Boid>& boids, int32_t w, int32_t h);
	void draw(Argon& argon);
};

#endif