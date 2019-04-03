#include "boid.h"
#include <cstdlib>
#include <vector>
#include <cmath>
#include "poly.h"
#include <SDL2/SDL.h>
#define RAND(min,max) (static_cast<float> (rand()) / static_cast<float> (RAND_MAX)) * (max - min) + min

Boid::Boid(int x, int y) {
	pos = new Vector(x,y);
	vel = new Vector(RAND(-1,1),RAND(-1,1));
	acc = new Vector;
	int index = rand() % 6;
	color[0] = colors[index];
	color[1] = colors[index+1];
	color[2] = colors[index+2];
}
Boid::~Boid() {printf("Boid Died\n");};

void Boid::flock(std::vector<Boid*> boids) {
	int flock = 0; int seperated = 0;
	Vector c;
	Vector s;
	Vector a;
	for(auto boid : boids) {
		if(boid == this) continue;
		float d = (pos->x - boid->pos->x)*(pos->x - boid->pos->x) + (pos->y - boid->pos->y)*(pos->y - boid->pos->y) + (pos->z - boid->pos->z)*(pos->z - boid->pos->z);
		if(d < perception*perception) {
			flock ++;
			c += boid->pos;
			a += boid->vel;
			if(d < space*space) {
				seperated ++;
				Vector tmp(*pos);
				tmp -= boid->pos;
				tmp.normalize();
				tmp /= d;
				s += tmp;
			}
		}
	}
	if(flock > 0) {
		if(c.magSq() > 0) {
			c /= flock;
			c -= pos;
			c.setMag(maxSpd);
			c -= vel;
			c.limit(maxForce);
			c *= cWeight;
			*acc += c;
		}
		if(a.magSq() > 0) {
      a /= flock;
      a.setMag(maxSpd);
      a -= vel;
      a.limit(maxForce);
      a *= aWeight;
      *acc += a;
		}
		if(seperated > 0 && s.magSq() > 0) {
      s /= seperated;
      s.setMag(maxSpd);
      s -= vel;
      s.limit(maxForce);
      s *= sWeight;
      *acc += s;
		}
	}
}
void Boid::update(int w,int h) {
	*vel += acc;
	vel->setMag(maxSpd);
	*pos += vel;
	if(pos->x > w+size) {pos->x = -size;}
	if(pos->x < -size) {pos->x = w+size;}
	if(pos->y > h+size) {pos->y = -size;}
	if(pos->y < -size) {pos->y = h+size;}
	*acc *= 0;
}
void Boid::draw(SDL_Renderer* ren) {
	double a = atan2(vel->y,vel->x);
	SDL_SetRenderDrawColor(ren, color[0], color[1], color[2], SDL_ALPHA_OPAQUE);
	Point p1 = {static_cast<float>(pos->x+size*cos(a)),static_cast<float>(pos->y+size*sin(a))};
	Point p2 = {static_cast<float>(pos->x+size*cos(a+3*M_PI_4)),static_cast<float>(pos->y+size*sin(a+3*M_PI_4))};
	Point p3 = {static_cast<float>(pos->x+size*cos(a-3*M_PI_4)),static_cast<float>(pos->y+size*sin(a-3*M_PI_4))};
	std::vector<Point> pts = {p1, p2, p3};
	// SDL_RenderDrawLine(ren, pos->x+size*cos(a),pos->y+size*sin(a),pos->x+size*cos(a+3*M_PI_4),pos->y+size*sin(a+3*M_PI_4));
	// SDL_RenderDrawLine(ren, pos->x+size*cos(a+3*M_PI_4),pos->y+size*sin(a+3*M_PI_4),pos->x+size*cos(a-3*M_PI_4),pos->y+size*sin(a-3*M_PI_4));
	// SDL_RenderDrawLine(ren, pos->x+size*cos(a-3*M_PI_4),pos->y+size*sin(a-3*M_PI_4),pos->x+size*cos(a),pos->y+size*sin(a));
	Poly p(ren, pts);
	p.fill();
}
bool Boid::operator==(Boid* b) {
	return this == b;
}