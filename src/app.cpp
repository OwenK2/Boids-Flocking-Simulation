#include "app.h"
#include <vector>
using namespace std;

Application::Application(const char* _name,const char* _logo,int _x, int _y, int _w, int _h, int _premade) : name(_name), logo(_logo), x(_x), y(_y), w(_w), h(_h), premade(_premade) {
	SDL_Init(SDL_INIT_VIDEO);
	// SDL_WINDOW_FULLSCREEN_DESKTOP
	int flags = SDL_RENDERER_PRESENTVSYNC|SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE|SDL_WINDOW_ALLOW_HIGHDPI;
	win = SDL_CreateWindow(name,x,y,w,h,flags);
	ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_SOFTWARE);
	SDL_Surface* logoSurface = SDL_LoadBMP(logo);
	SDL_SetWindowTitle(win,name);
	SDL_SetWindowIcon(win, logoSurface);
	SDL_FreeSurface(logoSurface);
	SDL_GL_GetDrawableSize(win, &dw, &dh);
	SDL_GetRendererOutputSize(ren, &w, &h);
	SDL_SetEventFilter(onResize, this);
	running = 1;
	for(int i = 0;i<premade;++i) {
		newBoid(RAND(0,w), RAND(0,h));
	}
	loop();
}
Application::~Application() {
	SDL_DestroyWindow(win);
	SDL_DestroyRenderer(ren);
	SDL_Quit();
}

void Application::loop() {
	int FPS = 60;
	int frameTime = 1000 / FPS;
	Uint32 time = SDL_GetTicks();
  SDL_Event e;
  while(running) {
    Uint32 now = SDL_GetTicks();
    int maxBehind = 10;
    if(time <= now) {
	    while(time <= now  && (maxBehind--)) {
				app();
				time += frameTime;
	    }
	    render();
    }
    else {
    	SDL_Delay(time - now);
    }
	  while(SDL_PollEvent(&e)) {
    	onEvent(e);
  	}
  }
  quit();
}
void Application::app() {
  SDL_SetRenderDrawColor(ren, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(ren);
	for(auto b : boids) {
		b->flock(boids);
		b->update(w, h);
		b->draw(ren);
	}
	Uint32 now = SDL_GetTicks();
	if(mdown && now - lastBoidTime >= bps) {
		int mx, my;
		SDL_GetMouseState(&mx, &my);
		newBoid(mx,my);
		lastBoidTime = now;
	}
}
void Application::onEvent(SDL_Event e) {
	switch(e.type) {
		case SDL_QUIT:{
			quit();
			break;
		}
		case SDL_MOUSEBUTTONDOWN: {
			mdown = 1;
			break;
		}
		case SDL_MOUSEBUTTONUP: {
			mdown = 0;
			break;
		}
	}
}
void Application::render() {
	SDL_RenderPresent(ren);
}
void Application::newBoid(int x, int y) {
	if(boidCount >= maxBoids) return;
	Boid* boid = new Boid(x, y);
	boidCount ++;
	boids.push_back(boid);
}
int Application::onResize(void* data, SDL_Event * event) {
  if(event->type == SDL_WINDOWEVENT && event->window.event == SDL_WINDOWEVENT_RESIZED) {
  	Application* app = ((Application*)data);
		SDL_GL_GetDrawableSize(app->win, &app->dw, &app->dh);
		SDL_GetRendererOutputSize(app->ren, &app->w, &app->h);
	  SDL_SetRenderDrawColor(app->ren, 0, 0, 0, SDL_ALPHA_OPAQUE);
	  SDL_RenderClear(app->ren);
		for(auto b : app->boids) {
			b->draw(app->ren);
		}
  	app->render();
    return 0;
  }
  return 1;
}
void Application::quit() {
	running = 0;
	SDL_DestroyWindow(win);
	SDL_DestroyRenderer(ren);
	SDL_Quit();
}