#include "app.h"
#include <vector>

using namespace std;

int main(int argc, char* argv[]) {
	srand(time(NULL));
	int num = 0;
	if(argv[1]) {
		char* p;
		long conv = strtol(argv[1], &p, 10);
		if (*p == '\0' && conv < INT_MAX) {
			num = conv;
		}
	}
	Application app("Boid Simulation", "logo.bmp", SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,500,500, num);

	return 0;
}

// int main(int argc, char* argv[]) {
// 	srand(time(NULL));

// 	bool running = true;

//   SDL_Init(SDL_INIT_VIDEO);
//   SDL_Window *window;
//   atexit(SDL_Quit);

//   window = SDL_CreateWindow("Boids",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,640,480,SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
//   SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
//   makeBoid(100,100);
// 	while(running) {
//     //Check Events
//     SDL_Event event;
//     SDL_PollEvent(&event);
//   	switch(event.type) {
//   		case SDL_QUIT:
//   			running = false;
//   			break;
//   		case SDL_MOUSEBUTTONDOWN: 
//   			makeBoid(event.button.x,event.button.y);
//   			break;
//   	}

//   	//Draw Boids
//     SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
//     SDL_RenderClear(renderer);
//     for(auto b : boids) {
//     	b->draw(renderer);
//     }
// 	}

// 	SDL_Quit();
//   return 0;
// }d