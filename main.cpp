#include "boid.h"
#include <chrono>
#include <random>


struct BoidApp : Argon {
	static constexpr uint32_t spawnRate = 100;
	std::vector<Boid> boids;
	std::default_random_engine generator;
	std::chrono::time_point<std::chrono::high_resolution_clock> lastCreation;
	int32_t w, h;

	BoidApp() : Argon("Boids"), generator(std::chrono::system_clock::now().time_since_epoch().count()) {}

	void onLoad(WindowEvent event) {
		setAntiAlias(true);
		lastCreation = std::chrono::high_resolution_clock::now();
		w = ww();
		h = wh();
	}
	void onWindowResize(WindowEvent event) {
		// Scale position to new size
		for(Boid& boid : boids) {
			boid.pos.x = boid.pos.x / w * event.w;
			boid.pos.y = boid.pos.y / h * event.h;
		}
		w = event.w;
		h = event.h;
	}
	void gameLoop(double deltaTime) {
		if(mouseDown()) {addBoid(mx(), my());}
		#pragma omp parallel for schedule(dynamic)
		for(Boid& boid : boids) {
			boid.update(deltaTime, boids, w, h);
		}
		for(Boid& boid : boids) {boid.draw(*this);}
	}

	void onKeyDown(KeyEvent e) {
		if(e.keycode == SDLK_RETURN) {
			boids.clear();
		}
	}

	void addBoid(float x, float y) {
		if(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - lastCreation).count() >= spawnRate) {
			std::uniform_real_distribution<double> vel(-1, 1);
			boids.emplace_back(Boid(x, y, vel(generator), vel(generator)));
			lastCreation = std::chrono::high_resolution_clock::now();
		}
	}

};


int main(int argc, char** argv) {
	BoidApp b;
	b.begin();

	return 0;
}