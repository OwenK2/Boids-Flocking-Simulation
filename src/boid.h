#ifndef BOID_H
#define BOID_H
#include <vector>
#include "vector.h"
#include <SDL2/SDL.h>
#define RAND(min,max) (static_cast<float> (rand()) / static_cast<float> (RAND_MAX)) * (max - min) + min


class Boid {
private:
	int perception = 50;
	int space = 25;
	int size = 10;
	float maxSpd = 1.4;
	float maxForce = .5;
  float maxAccleration = 0.05;
  float cWeight = .1;
  float aWeight = 1;
  float sWeight = 1.5;
	const int colors[18] = {237,85,101,255,206,84,140,193,82,72,207,173,74,137,220,150,122,220};

public:
	Vector* pos;
	Vector* vel;
	Vector* acc;
	int color[3];
	Boid(int x, int y);
	~Boid();
	void flock(std::vector<Boid*> boids);
	void update(int w, int h);
	void draw(SDL_Renderer* ren);
	bool inRange(Boid* b);
	bool operator==(Boid* b);
};
#endif