#include <SDL2/SDL.h>
#include <vector>
#include "boid.h"

class Application {
private:
	int premade = 0;
	int maxBoids = 200;
	int bps = 1000/10;
	int lastBoidTime = 0;
public:
	const char* name;
	const char* logo;
	int x;
	int y;
	int w;
	int h;
	int dw;
	int dh;
	bool running = 0;
	bool mdown = 0;
	int boidCount = 0;
	std::vector<Boid*> boids;

	SDL_Window* win;
	SDL_Renderer* ren;
	Application(const char* _name,const char* _logo,int _x, int _y, int _w, int _h, int _premade = 0);
	~Application();
	void loop();
	void app();
	void render();
	void newBoid(int x, int y);
	void quit();
	void onEvent(SDL_Event e);
	static int onResize(void* data, SDL_Event * event);
};