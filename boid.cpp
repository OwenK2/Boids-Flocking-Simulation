#include "boid.h"

Boid::Boid(double x, double y, double vx, double vy) {
	pos.x = x;
	pos.y = y;
	vel.x = vx;
	vel.y = vy;
	color = colors[rand() % numColors];
	vel.setMag(speed);
}


void Boid::update(double dt, std::vector<Boid>& boids, int32_t w, int32_t h) {
	// Flocking (compare to other boids in its perception range not including itself)
	vec2 cohesion, seperation, alignment; 
	uint32_t inFlock = 0;
	for(Boid& boid : boids) {
		if(&boid != this) {
			double d = vec2::distance2(pos, boid.pos);
			if(d < perception) {
				cohesion += boid.pos;
				alignment += boid.vel;
				++inFlock;
				if(d < seperationDistance) {
					seperation += (pos - boid.pos) / (d == 0 ? 1 : d/2.0);
				}
			}
		}
	}
	if(inFlock > 0) {
		cohesion /= inFlock;
		cohesion -= pos;
		alignment /= inFlock;
		alignment -= vel;
		vel += (cohesion * cohesionForce);
		vel += (alignment * alignmentForce);
		vel += (seperation * seperationForce);
	}

	//Move away from edges
	if(pos.x < edgeFearRange) {
		vel.x += edgeTurnForce;
	}
	if(pos.x > w - edgeFearRange) {
		vel.x -= edgeTurnForce;
	}
	if(pos.y < edgeFearRange) {
		vel.y += edgeTurnForce;
	}
	if(pos.y > h - edgeFearRange) {
		vel.y -= edgeTurnForce;
	}


	// Update Position
	vel.limit(speed * dt);
	pos += vel;

}
void Boid::draw(Argon& argon) {
	double a = vel.angle();
	argon.fillTriangle(
		pos.x + size * cos(a), 
		pos.y + size * sin(a), 
		pos.x + size * cos(a + M_PI_4 + M_PI_2), 
		pos.y + size * sin(a + M_PI_4 + M_PI_2), 
		pos.x + size * cos(a - M_PI_4 - M_PI_2), 
		pos.y + size * sin(a - M_PI_4 - M_PI_2), 
	color);
}