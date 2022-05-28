#ifndef ARGON_H
#define ARGON_H

#include <vector>
#include <SDL2/SDL.h>
#ifdef ARGON_INCLUDE_GFX
	#include <SDL2/SDL2_gfxPrimitives.h>
#endif
#ifdef ARGON_INCLUDE_TTF
	#include <SDL2/SDL_ttf.h>
#endif

// Macros
#define ARGON_FULLSCREEN 1
#define ARGON_HIDDEN 2
#define ARGON_BORDERLESS 4
#define ARGON_RESIZABLE 8
#define ARGON_MINIMIZED 16
#define ARGON_MAXIMIZED 32
#define ARGON_HIGHDPI 64
#define ARGON_ON_TOP 128
#define ARGON_NO_TASKBAR 256
#define ARGON_MOUSE_CAPTURE 512
#define ARGON_VSYNC 1024


// Event Structs
struct Event {
	uint32_t timestamp;
};
struct WindowEvent : Event {
	uint16_t x;
	uint16_t y;
	uint16_t w;
	uint16_t h;
};
struct KeyEvent : Event {
	SDL_Scancode scancode;
	SDL_Keycode keycode;
	bool repeat;
	struct {
		bool none     : 1;
		bool shift    : 1;
		bool lshift   : 1;
		bool rshift   : 1;
		bool ctrl     : 1;
		bool lctrl    : 1;
		bool rctrl    : 1;
		bool alt      : 1;
		bool lalt     : 1;
		bool ralt     : 1;
		bool meta     : 1;
		bool lmeta    : 1;
		bool rmeta    : 1;
		bool capslock : 1;
		bool numlock  : 1;
		bool altgr    : 1;
	} mod;
};
struct MouseEvent : Event {
	uint16_t x;
	uint16_t y;
	int16_t rx;
	int16_t ry;
	bool down;
};
struct MouseButtonEvent : Event {
	uint16_t x;
	uint16_t y;
	uint8_t which;
	uint8_t clicks;
};
struct WheelEvent : Event {
	int32_t dx;
	int32_t dy;
	bool down;
	bool flipped;
};
struct FileDropEvent : Event {
	char* path;
};

typedef SDL_Texture* Texture;

// Additional GFX Helper functions
#ifdef ARGON_INCLUDE_TTF 
	typedef TTF_Font Font;
#endif


class Argon {
public:
	Argon(const char* title, int32_t x, int32_t y, int32_t w, int32_t h, uint16_t flags=ARGON_VSYNC|ARGON_RESIZABLE|ARGON_HIGHDPI) : title(title), _wx(x), _wy(y), _ww(w), _wh(h), flags(flags) {
		if(SDL_Init(SDL_INIT_VIDEO) < 0) {
			fprintf(stderr, "Failed to initialize SDL\n");
			return;
		}
		#ifdef ARGON_INCLUDE_TTF
			TTF_Init();
		#endif
	}
	Argon(const char* title, int32_t w, int32_t h, uint16_t flags) : Argon(title, SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,w,h,flags) {}
	Argon(const char* title, int32_t w, int32_t h) : Argon(title, SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,w,h) {}
	Argon(const char* title, uint16_t flags) : Argon(title, SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,500,500, flags) {}
	Argon(const char* title) : Argon(title, SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,500,500) {};
	Argon() : Argon("Argon", SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,500,500) {};
	~Argon() {
		destroyAllTextures();
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		#ifdef ARGON_DYNAMIC_RESIZE
			SDL_DelEventWatch(handleResize, this);
		#endif
		#ifdef ARGON_INCLUDE_TTF
			TTF_Quit();
		#endif
		SDL_Quit();
	}

	void begin() {
		int wf = 0, rf = SDL_RENDERER_TARGETTEXTURE | SDL_RENDERER_ACCELERATED;
		if(flags & ARGON_FULLSCREEN) {wf |= SDL_WINDOW_FULLSCREEN_DESKTOP;}
		if(flags & ARGON_HIDDEN) {wf |= SDL_WINDOW_HIDDEN;}
		if(flags & ARGON_BORDERLESS) {wf |= SDL_WINDOW_BORDERLESS;}
		if(flags & ARGON_RESIZABLE) {wf |= SDL_WINDOW_RESIZABLE;}
		if(flags & ARGON_MINIMIZED) {wf |= SDL_WINDOW_MINIMIZED;}
		if(flags & ARGON_MAXIMIZED) {wf |= SDL_WINDOW_MAXIMIZED;}
		if(flags & ARGON_HIGHDPI) {wf |= SDL_WINDOW_ALLOW_HIGHDPI;}
		if(flags & ARGON_ON_TOP) {wf |= SDL_WINDOW_ALWAYS_ON_TOP;}
		if(flags & ARGON_NO_TASKBAR) {wf |= SDL_WINDOW_SKIP_TASKBAR;}
		if(flags & ARGON_MOUSE_CAPTURE) {wf |= SDL_WINDOW_MOUSE_CAPTURE;}
		if(flags & ARGON_VSYNC) {vsyncEnabled=true;rf |= SDL_RENDERER_PRESENTVSYNC;}

		window = SDL_CreateWindow(title, _wx, _wy, _ww, _wh, wf);
		renderer = SDL_CreateRenderer(window, -1, rf);
		SDL_SetWindowTitle(window, title);
		SDL_RenderSetIntegerScale(renderer, SDL_TRUE);
		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
		int ww, wh, mx, my, wx, wy;
		SDL_GetWindowSize(window, &ww, &wh);
		SDL_GetMouseState(&mx, &my);
		SDL_GetWindowPosition(window, &wx, &wy);
		_ww = (uint16_t)ww;
		_wh = (uint16_t)wh;
		_wx = (uint16_t)wx;
		_wy = (uint16_t)wy;
		_mx = (uint16_t)mx;
		_my = (uint16_t)my;

		SDL_EventState(SDL_DROPFILE, SDL_ENABLE);
		#ifdef ARGON_DYNAMIC_RESIZE
			SDL_AddEventWatch(handleResize, this);
		#endif

		running = true;

		// Flush event queue & trigger onLoad event
		SDL_Event event;
		while(SDL_PollEvent(&event)) {}
		onLoad({{SDL_GetTicks()}, _wx, _wy, _ww, _wh});

		// Setup & run game loop
		uint64_t now = 0, prev = SDL_GetPerformanceCounter();
		while(running) {
			// Event Handling
			while(SDL_PollEvent(&event)) {
				handleEvent(event);
			}
			if(!running) {break;} //If quit occurs

			// Call Game Loop
			now = SDL_GetPerformanceCounter();
			if(prev < now) {
				gameLoop((now - prev)/(double)SDL_GetPerformanceFrequency());
				prev = now;
			}

			//Render
			SDL_RenderSetLogicalSize(renderer, _ww, _wh);
			SDL_RenderPresent(renderer);
			clear();
		}

	}

	void stop() {onUnload({{SDL_GetTicks()}, _wx, _wy, _ww, _wh});running = false;}

	// Event Handlers
	virtual void onQuit(const Event event) {}
	virtual void onLoad(const WindowEvent event) {}
	virtual void onUnload(const WindowEvent event) {}
	virtual void onMouseDown(const MouseButtonEvent event) {}
	virtual void onMouseUp(const MouseButtonEvent event) {}
	virtual void onMouseMove(const  MouseEvent event) {}
	virtual void onMouseWheel(const WheelEvent event) {}
	virtual void onKeyDown(const KeyEvent event) {}
	virtual void onKeyUp(const KeyEvent event) {}
	virtual void onWindowShow(const WindowEvent event) {}
	virtual void onWindowExpose(const WindowEvent event) {}
	virtual void onWindowMove(const WindowEvent event) {}
	virtual void onWindowResize(const WindowEvent event) {}
	virtual void onWindowMinimize(const WindowEvent event) {}
	virtual void onWindowMaximize(const WindowEvent event) {}
	virtual void onWindowRestore(const WindowEvent event) {}
	virtual void onMouseEnter(const MouseEvent event) {}
	virtual void onMouseLeave(const MouseEvent event) {}
	virtual void onWindowFocus(const WindowEvent event) {}
	virtual void onWindowBlur(const WindowEvent event) {}
	virtual void onWindowClose(const WindowEvent event) {}
	virtual void onWindowTakeFocus(const WindowEvent event) {}
	virtual void onWindowHitTest(const WindowEvent event) {}
	virtual void onFileDrop(const FileDropEvent event) {}

	// Main Game Loop
	virtual void gameLoop(double deltaTime) {}

	// Translating SDL Functions
	void maximize() {SDL_MaximizeWindow(window);}
	void minimize() {SDL_MinimizeWindow(window);}
	void show() {SDL_ShowWindow(window);}
	void hide() {SDL_HideWindow(window);}
	void raise() {SDL_RaiseWindow(window);}
	void restore() {SDL_RestoreWindow(window);}
	void fullscreen() {SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);}
	void setMaxSize(int w, int h) {SDL_SetWindowMaximumSize(window, w, h);}
	void setMinSize(int w, int h) {SDL_SetWindowMinimumSize(window, w, h);}
	void setOpacity(float opacity) {SDL_SetWindowOpacity(window, opacity);}
	void setPosition(int x, int y) {SDL_SetWindowPosition(window, x, y);}
	void setMousePosition(int x, int y) {SDL_WarpMouseInWindow(window, x, y);}
	void setSize(int w, int h) {SDL_SetWindowSize(window, w, h);}
	void setTitle(const char* title) {SDL_SetWindowTitle(window, title);}
	void setIsResizable(bool resizable) {SDL_SetWindowResizable(window, (SDL_bool)resizable);}
	void setBordered(bool bordered) {SDL_SetWindowBordered(window,(SDL_bool) bordered);}
	void setBrightness(float brightness) {SDL_SetWindowBrightness(window, brightness);}
	bool setCaptureMouse(bool capture = true) {return SDL_CaptureMouse((SDL_bool)capture) == 0;}
	bool setRelativeMouseMode(bool relative) {return SDL_SetRelativeMouseMode((SDL_bool)relative) == 0;}
	void setCursorDisplay(bool show) {SDL_ShowCursor(show ? SDL_ENABLE : SDL_DISABLE);}
	bool alert(const char* message) {return SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, getTitle(), message, window) == 0;}
	bool alert(const char* title, const char* message) {return SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, title, message, window) == 0;}
	int alert(const char* title, const char* message, uint8_t numButtons, SDL_MessageBoxButtonData* btnData, SDL_MessageBoxFlags flag = SDL_MESSAGEBOX_INFORMATION) {int btn;SDL_MessageBoxData data = {flag, window, title, message, numButtons, btnData, NULL};SDL_ShowMessageBox(&data, &btn);return btn;	}

	// Get Variables
	bool isRunning() {return running;}
	int32_t wx() {return _wx;}
	int32_t wy() {return _wy;}
	int32_t ww() {return _ww;}
	int32_t wh() {return _wh;}
	int32_t mx() {return _mx;}
	int32_t my() {return _my;}
	bool mouseDown() {return down;}
	bool getBorderSizes(int* top, int* right, int* bottom, int* left) {return SDL_GetWindowBordersSize(window, top, left, bottom, right) == 0;}
	float getBrightness() {return SDL_GetWindowBrightness(window);}
	void getMaxSize(int* w, int* h) {SDL_GetWindowMaximumSize(window, w, h);}
	void getMinSize(int* w, int* h) {SDL_GetWindowMinimumSize(window, w, h);}
	float getOpacity() {float o = -1;SDL_GetWindowOpacity(window, &o);return o;}
	const char* getTitle() {return SDL_GetWindowTitle(window);}
	bool isAntiAliasing() {return antiAlias;}

	//Setters
	void setAntiAlias(bool val) {antiAlias = val;}
	void setBackground(uint32_t c) {background = {(uint8_t)(c>>24), (uint8_t)(c>>16), (uint8_t)(c>>8), (uint8_t)c};}
	void setBackground(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {background = {r, g, b, a};}

	// Rendering to texture
	Texture* createTexture() {
		if(textures.size() >= 255) {return NULL;}
		textures.emplace_back(SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, _ww, _wh));
		return &textures.back();
	}
	bool destroyTexture(Texture* texture) {
		for(uint8_t i = 0; i < textures.size(); ++i) {
			if(textures[i] == *texture) {
				if(*texture != NULL) {SDL_DestroyTexture(*texture);}
				textures.erase(textures.begin() + i);
				return true;
			}
		}
		return false;
	}
	void destroyAllTextures() {
		for(auto texture : textures) {
			if(texture != NULL) {SDL_DestroyTexture(texture);}
		}
		textures.clear();
	}
	uint8_t* lockTexture(Texture* texture, int32_t* w, int32_t* h) {
		uint8_t* pixels = NULL;
		if(*texture != NULL) {
			int res = SDL_QueryTexture(*texture, NULL, NULL, w, h);
			if(res == 0) {
				int pitch;
				if(SDL_LockTexture(*texture, NULL, reinterpret_cast<void**>(&pixels), &pitch) != 0) {
					pixels = NULL;
				}
			}
		}
		return pixels;
	}
	void getTextureSize(Texture* texture, int32_t* w, int32_t* h) {
		SDL_QueryTexture(*texture, NULL, NULL, w, h);
	}
	void unlockTexture(Texture* texture) {
		SDL_UnlockTexture(*texture);
	}
	void drawTexture(Texture* texture) {
		SDL_RenderCopy(renderer, *texture, NULL, NULL);
	}


	// Graphics
	void clear() {
		setColor(background.r, background.g, background.b, background.a);
		SDL_RenderClear(renderer);
	}
	void pixel(int32_t x, int32_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
		setColor(r, g, b, a);
		SDL_RenderDrawPoint(renderer, x, y);
	}
	void pixel(int32_t x, int32_t y, uint32_t color = 0xFFFFFFFF) {
		setColor(color);
		SDL_RenderDrawPoint(renderer, x, y);
	}
	void fillRect(int32_t x, int32_t y, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
		setColor(r, g, b, a);
		SDL_Rect rect = {x, y, w, h};
		SDL_RenderFillRect(renderer, &rect);
	}
	void fillRect(int32_t x, int32_t y, uint16_t w, uint16_t h, uint32_t color = 0xFFFFFFFF) {
		setColor(color);
		SDL_Rect rect = {x, y, w, h};
		SDL_RenderFillRect(renderer, &rect);
	}
	void rect(int32_t x, int32_t y, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
		setColor(r, g, b, a);
		SDL_Rect rect = {x, y, w, h};
		SDL_RenderDrawRect(renderer, &rect);
	}
	void rect(int32_t x, int32_t y, uint16_t w, uint16_t h, uint32_t color = 0xFFFFFFFF) {
		setColor(color);
		SDL_Rect rect = {x, y, w, h};
		SDL_RenderDrawRect(renderer, &rect);
	}



	#ifndef ARGON_INCLUDE_GFX
		void line(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
			setColor(r, g, b, a);
			SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
		}
		void line(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color = 0xFFFFFFFF) {
			setColor(color);
			SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
		}
		void hline(int32_t x1, int32_t x2, int32_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
			setColor(r, g, b, a);
			SDL_RenderDrawLine(renderer, x1, y, x2, y);
		}
		void hline(int32_t x1, int32_t x2, int32_t y, uint32_t color = 0xFFFFFFFF) {
			setColor(color);
			SDL_RenderDrawLine(renderer, x1, y, x2, y);
		}
		void vline(int32_t x, int32_t y1, int32_t y2, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
			setColor(r, g, b, a);
			SDL_RenderDrawLine(renderer, x, y1, x, y2);
		}
		void vline(int32_t x, int32_t y1, int32_t y2, uint32_t color = 0xFFFFFFFF) {
			setColor(color);
			SDL_RenderDrawLine(renderer, x, y1, x, y2);
		}
	#else
		void roundedRectangle(int32_t x, int32_t y, uint16_t w, uint16_t h, uint16_t radius, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
			roundedRectangleRGBA(renderer, x, y, x+w, y+h, radius, r, g, b, a);
		}
		void roundedRectangle(int32_t x, int32_t y, uint16_t w, uint16_t h, uint16_t radius, uint32_t color = 0xFFFFFFFF) {
			roundedRectangleRGBA(renderer, x, y, x+w, y+h, radius, (uint8_t)(color>>24), (uint8_t)(color>>16), (uint8_t)(color>>8), (uint8_t)color);
		}
		void fillRoundedRectangle(int32_t x, int32_t y, uint16_t w, uint16_t h, uint16_t radius, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
			roundedBoxRGBA(renderer, x, y, x+w, y+h, radius, r, g, b, a);
		}
		void fillRoundedRectangle(int32_t x, int32_t y, uint16_t w, uint16_t h, uint16_t radius, uint32_t color = 0xFFFFFFFF) {
			roundedBoxRGBA(renderer, x, y, x+w, y+h, radius, (uint8_t)(color>>24), (uint8_t)(color>>16), (uint8_t)(color>>8), (uint8_t)color);
		}
		void line(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
			if(antiAlias) aalineRGBA(renderer, x1, y1, x2, y2, r, g, b, a);
			else aalineRGBA(renderer, x1, y1, x2, y2, r, g, b, a);
		}
		void line(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color = 0xFFFFFFFF) {
			if(antiAlias) aalineRGBA(renderer, x1, y1, x2, y2, (uint8_t)(color>>24), (uint8_t)(color>>16), (uint8_t)(color>>8), (uint8_t)color);
			else lineRGBA(renderer, x1, y1, x2, y2, (uint8_t)(color>>24), (uint8_t)(color>>16), (uint8_t)(color>>8), (uint8_t)color);
		}
		void hline(int32_t x1, int32_t x2, int32_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
			if(antiAlias) aalineRGBA(renderer, x1, y, x2, y, r, g, b, a);
			else lineRGBA(renderer, x1, y, x2, y, r, g, b, a);
		}
		void hline(int32_t x1, int32_t x2, int32_t y, uint32_t color = 0xFFFFFFFF) {
			if(antiAlias) aalineRGBA(renderer, x1, y, x2, y, (uint8_t)(color>>24), (uint8_t)(color>>16), (uint8_t)(color>>8), (uint8_t)color);
			else lineRGBA(renderer, x1, y, x2, y, (uint8_t)(color>>24), (uint8_t)(color>>16), (uint8_t)(color>>8), (uint8_t)color);
		}
		void vline(int32_t x, int32_t y1, int32_t y2, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
			if(antiAlias) aalineRGBA(renderer, x, y1, x, y2, r, g, b, a);
			else lineRGBA(renderer, x, y1, x, y2, r, g, b, a);
		}
		void vline(int32_t x, int32_t y1, int32_t y2, uint32_t color = 0xFFFFFFFF) {
			if(antiAlias) aalineRGBA(renderer, x, y1, x, y2, (uint8_t)(color>>24), (uint8_t)(color>>16), (uint8_t)(color>>8), (uint8_t)color);
			else lineRGBA(renderer, x, y1, x, y2, (uint8_t)(color>>24), (uint8_t)(color>>16), (uint8_t)(color>>8), (uint8_t)color);
		}
		void thickLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint8_t thickness, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
			thickLineRGBA(renderer, x1, y1, x2, y2, thickness, r, g, b, a);
		}
		void thickLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint8_t thickness, uint32_t color = 0xFFFFFFFF) {
			thickLineRGBA(renderer, x1, y1, x2, y2, thickness, (uint8_t)(color>>24), (uint8_t)(color>>16), (uint8_t)(color>>8), (uint8_t)color);
		}
		void circle(int32_t x, int32_t y, uint16_t radius, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
			if(antiAlias) aacircleRGBA(renderer, x, y, radius, r, g, b, a);
			else circleRGBA(renderer, x, y, radius, r, g, b, a);
		}
		void circle(int32_t x, int32_t y, uint16_t radius, uint32_t color = 0xFFFFFFFF) {
			if(antiAlias) aacircleRGBA(renderer, x, y, radius, (uint8_t)(color>>24), (uint8_t)(color>>16), (uint8_t)(color>>8), (uint8_t)color);
			else circleRGBA(renderer, x, y, radius, (uint8_t)(color>>24), (uint8_t)(color>>16), (uint8_t)(color>>8), (uint8_t)color);
		}
		void fillCircle(int32_t x, int32_t y, uint16_t radius, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
			filledCircleRGBA(renderer, x, y, radius, r, g, b, a);
		}
		void fillCircle(int32_t x, int32_t y, uint16_t radius, uint32_t color = 0xFFFFFFFF) {
			filledCircleRGBA(renderer, x, y, radius, (uint8_t)(color>>24), (uint8_t)(color>>16), (uint8_t)(color>>8), (uint8_t)color);
		}
		void ellipse(int32_t x, int32_t y, uint16_t rx, uint16_t ry, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
			if(antiAlias) aaellipseRGBA(renderer, x, y, rx, ry, r, g, b, a);
			else ellipseRGBA(renderer, x, y, rx, ry, r, g, b, a);
		}
		void ellipse(int32_t x, int32_t y, uint16_t rx, uint16_t ry, uint32_t color = 0xFFFFFFFF) {
			if(antiAlias) aaellipseRGBA(renderer, x, y, rx, ry, (uint8_t)(color>>24), (uint8_t)(color>>16), (uint8_t)(color>>8), (uint8_t)color);
			else ellipseRGBA(renderer, x, y, rx, ry, (uint8_t)(color>>24), (uint8_t)(color>>16), (uint8_t)(color>>8), (uint8_t)color);
		}
		void fillEllipse(int32_t x, int32_t y, uint16_t rx, uint16_t ry, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
			filledEllipseRGBA(renderer, x, y, rx, ry, r, g, b, a);
		}
		void fillEllipse(int32_t x, int32_t y, uint16_t rx, uint16_t ry, uint32_t color = 0xFFFFFFFF) {
			filledEllipseRGBA(renderer, x, y, rx, ry, (uint8_t)(color>>24), (uint8_t)(color>>16), (uint8_t)(color>>8), (uint8_t)color);
		}
		void arc(int32_t x, int32_t y, uint16_t radius, int16_t start, int16_t end, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
			arcRGBA(renderer, x, y, radius, start, end, r, g, b, a);
		}
		void arc(int32_t x, int32_t y, uint16_t radius, int16_t start, int16_t end, uint32_t color = 0xFFFFFFFF) {
			arcRGBA(renderer, x, y, radius, start, end, (uint8_t)(color>>24), (uint8_t)(color>>16), (uint8_t)(color>>8), (uint8_t)color);
		}
		void triangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
			if(antiAlias) aatrigonRGBA(renderer, x1, y1, x2, y2, x3, y3, r, g, b, a);
			else trigonRGBA(renderer, x1, y1, x2, y2, x3, y3, r, g, b, a);
		}
		void triangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, uint32_t color = 0xFFFFFFFF) {
			if(antiAlias) aatrigonRGBA(renderer, x1, y1, x2, y2, x3, y3, (uint8_t)(color>>24), (uint8_t)(color>>16), (uint8_t)(color>>8), (uint8_t)color);
			else trigonRGBA(renderer, x1, y1, x2, y2, x3, y3, (uint8_t)(color>>24), (uint8_t)(color>>16), (uint8_t)(color>>8), (uint8_t)color);
		}
		void fillTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
			filledTrigonRGBA(renderer, x1, y1, x2, y2, x3, y3, r, g, b, a);
		}
		void fillTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, uint32_t color = 0xFFFFFFFF) {
			filledTrigonRGBA(renderer, x1, y1, x2, y2, x3, y3, (uint8_t)(color>>24), (uint8_t)(color>>16), (uint8_t)(color>>8), (uint8_t)color);
		}
		void polygon(uint8_t numPts, const int16_t* x, const int16_t* y, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
			if(antiAlias) aapolygonRGBA(renderer, x, y, numPts, r, g, b, a);
			else polygonRGBA(renderer, x, y, numPts, r, g, b, a);
		}
		void polygon(uint8_t numPts, const int16_t* x, const int16_t* y, uint32_t color = 0xFFFFFFFF) {
			if(antiAlias) aapolygonRGBA(renderer, x, y, numPts, (uint8_t)(color>>24), (uint8_t)(color>>16), (uint8_t)(color>>8), (uint8_t)color);
			else polygonRGBA(renderer, x, y, numPts, (uint8_t)(color>>24), (uint8_t)(color>>16), (uint8_t)(color>>8), (uint8_t)color);
		}
		void fillPolygon(uint8_t numPts, const int16_t* x, const int16_t* y, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
			filledPolygonRGBA(renderer, x, y, numPts, r, g, b, a);
		}
		void fillPolygon(uint8_t numPts, const int16_t* x, const int16_t* y, uint32_t color = 0xFFFFFFFF) {
			filledPolygonRGBA(renderer, x, y, numPts, (uint8_t)(color>>24), (uint8_t)(color>>16), (uint8_t)(color>>8), (uint8_t)color);
		}
	#endif
	#ifdef ARGON_INCLUDE_TTF
			Font* createFont(const char* fontFile, uint16_t fontSize) {
				return TTF_OpenFont(fontFile, fontSize);
			}
			void freeFont(Font* font) {
				if(font != NULL) TTF_CloseFont(font);
			}
			void string(const char* txt, Font* font, int32_t x, int32_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
				SDL_Surface* surface = TTF_RenderText_Blended(font, txt, {r, g, b, a});
				SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
				int texW = 0;
				int texH = 0;
				SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
				SDL_Rect dstrect = {x, y, texW, texH};
				SDL_RenderCopy(renderer, texture, NULL, &dstrect);
				SDL_DestroyTexture(texture);
				SDL_FreeSurface(surface);
			}
			void string(const char* txt, Font* font, int32_t x, int32_t y, uint32_t color = 0xFFFFFFFF) {
				string(txt, font, x, y, (uint8_t)(color>>24), (uint8_t)(color>>16), (uint8_t)(color>>8), (uint8_t)color);
			}
			void stringDimensions(const char* string, Font* font, int* w, int* h) {
				TTF_SizeText(font, string, w, h);
			}
	#endif

private:
	const char* title;
	SDL_Window* window;
	SDL_Renderer* renderer;
	bool running = false;
	bool vsyncEnabled = false;

	// Window Data
	uint16_t _wx;
	uint16_t _wy;
	uint16_t _ww;
	uint16_t _wh;

	// Mouse Data
	uint16_t _mx;
	uint16_t _my;
	bool down = false;

	// Flags
	uint16_t flags;

	//Other
	struct {
		uint8_t r = 0;
		uint8_t g = 0;
		uint8_t b = 0;
		uint8_t a = 255;
	} background;

	std::vector<Texture> textures;
	bool antiAlias = false;

	// Event System
	void handleEvent(SDL_Event& event) {
		switch(event.type) {
			case SDL_QUIT: {
				onQuit({event.quit.timestamp});
				stop();
				break;
			}
			case SDL_MOUSEBUTTONDOWN: {
				down = true;
				onMouseDown({{event.button.timestamp}, _mx, _my, event.button.button, event.button.clicks});
				break;
			}
			case SDL_MOUSEBUTTONUP: {
				down = false;
				onMouseUp({{event.button.timestamp}, _mx, _my, event.button.button, event.button.clicks});
				break;
			}
			case SDL_MOUSEMOTION: {
				_mx = static_cast<uint16_t>(event.motion.x);
				_my = static_cast<uint16_t>(event.motion.y);
				onMouseMove({{event.motion.timestamp}, _mx, _my, static_cast<int16_t>(event.motion.xrel), static_cast<int16_t>(event.motion.yrel), down});
				break;
			}
			case SDL_MOUSEWHEEL: {
				onMouseWheel({{event.wheel.timestamp}, event.wheel.x, event.wheel.y, down, event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED});
				break;
			}
			case SDL_KEYDOWN: {
				onKeyDown({{event.key.timestamp},event.key.keysym.scancode,event.key.keysym.sym,event.key.repeat > 0,{!static_cast<bool>((event.key.keysym.mod & KMOD_SHIFT) || (event.key.keysym.mod & KMOD_ALT) || (event.key.keysym.mod & KMOD_CTRL) || (event.key.keysym.mod & KMOD_GUI) || (event.key.keysym.mod & KMOD_CAPS) || (event.key.keysym.mod & KMOD_NUM) || (event.key.keysym.mod & KMOD_MODE)),static_cast<bool>(event.key.keysym.mod & KMOD_SHIFT),static_cast<bool>(event.key.keysym.mod & KMOD_LSHIFT),static_cast<bool>(event.key.keysym.mod & KMOD_RSHIFT),static_cast<bool>(event.key.keysym.mod & KMOD_CTRL),static_cast<bool>(event.key.keysym.mod & KMOD_LCTRL),static_cast<bool>(event.key.keysym.mod & KMOD_RCTRL),static_cast<bool>(event.key.keysym.mod & KMOD_ALT),static_cast<bool>(event.key.keysym.mod & KMOD_LALT),static_cast<bool>(event.key.keysym.mod & KMOD_RALT),static_cast<bool>(event.key.keysym.mod & KMOD_GUI),static_cast<bool>(event.key.keysym.mod & KMOD_LGUI),static_cast<bool>(event.key.keysym.mod & KMOD_RGUI),static_cast<bool>(event.key.keysym.mod & KMOD_CAPS),static_cast<bool>(event.key.keysym.mod & KMOD_NUM),static_cast<bool>(event.key.keysym.mod & KMOD_MODE)}});
				break;
			}
			case SDL_KEYUP: {
				onKeyUp({{event.key.timestamp},event.key.keysym.scancode,event.key.keysym.sym,event.key.repeat > 0,{!static_cast<bool>((event.key.keysym.mod & KMOD_SHIFT) || (event.key.keysym.mod & KMOD_ALT) || (event.key.keysym.mod & KMOD_CTRL) || (event.key.keysym.mod & KMOD_GUI) || (event.key.keysym.mod & KMOD_CAPS) || (event.key.keysym.mod & KMOD_NUM) || (event.key.keysym.mod & KMOD_MODE)),static_cast<bool>(event.key.keysym.mod & KMOD_SHIFT),static_cast<bool>(event.key.keysym.mod & KMOD_LSHIFT),static_cast<bool>(event.key.keysym.mod & KMOD_RSHIFT),static_cast<bool>(event.key.keysym.mod & KMOD_CTRL),static_cast<bool>(event.key.keysym.mod & KMOD_LCTRL),static_cast<bool>(event.key.keysym.mod & KMOD_RCTRL),static_cast<bool>(event.key.keysym.mod & KMOD_ALT),static_cast<bool>(event.key.keysym.mod & KMOD_LALT),static_cast<bool>(event.key.keysym.mod & KMOD_RALT),static_cast<bool>(event.key.keysym.mod & KMOD_GUI),static_cast<bool>(event.key.keysym.mod & KMOD_LGUI),static_cast<bool>(event.key.keysym.mod & KMOD_RGUI),static_cast<bool>(event.key.keysym.mod & KMOD_CAPS),static_cast<bool>(event.key.keysym.mod & KMOD_NUM),static_cast<bool>(event.key.keysym.mod & KMOD_MODE)}});
				break;
			}
			case SDL_WINDOWEVENT: {
				switch(event.window.event) {
					case SDL_WINDOWEVENT_SHOWN: {
						onWindowShow({{event.window.timestamp}, _wx, _wy, _ww, _wh});
						break;
					}
					case SDL_WINDOWEVENT_EXPOSED: {
						onWindowExpose({{event.window.timestamp}, _wx, _wy, _ww, _wh});
						break;
					}
					case SDL_WINDOWEVENT_MOVED: {
						_wx = event.window.data1;
						_wy = event.window.data2;
						onWindowMove({{event.window.timestamp}, _wx, _wy, _ww, _wh});
						break;
					}
					case SDL_WINDOWEVENT_SIZE_CHANGED: {
						_ww = event.window.data1;
						_wh = event.window.data2;
						recreateTextures();
						onWindowResize({{event.window.timestamp}, _wx, _wy, _ww, _wh});
						break;
					}
					case SDL_WINDOWEVENT_MINIMIZED: {
						onWindowMinimize({{event.window.timestamp}, _wx, _wy, _ww, _wh});
						break;
					}
					case SDL_WINDOWEVENT_MAXIMIZED: {
						onWindowMaximize({{event.window.timestamp}, _wx, _wy, _ww, _wh});
						break;
					}
					case SDL_WINDOWEVENT_RESTORED: {
						onWindowRestore({{event.window.timestamp}, _wx, _wy, _ww, _wh});
						break;
					}
					case SDL_WINDOWEVENT_ENTER: {
						onMouseEnter({{event.window.timestamp}, _mx, _my, 0, 0, down});
						break;
					}
					case SDL_WINDOWEVENT_LEAVE: {
						onMouseLeave({{event.window.timestamp}, _mx, _my, 0, 0, down});
						break;
					}
					case SDL_WINDOWEVENT_FOCUS_GAINED: {
						onWindowFocus({{event.window.timestamp}, _wx, _wy, _ww, _wh});
						break;
					}
					case SDL_WINDOWEVENT_FOCUS_LOST: {
						onWindowBlur({{event.window.timestamp}, _wx, _wy, _ww, _wh});
						break;
					}
					case SDL_WINDOWEVENT_CLOSE: {
						onWindowClose({{event.window.timestamp}, _wx, _wy, _ww, _wh});
						break;
					}
					case SDL_WINDOWEVENT_TAKE_FOCUS: {
						onWindowTakeFocus({{event.window.timestamp}, _wx, _wy, _ww, _wh});
						break;
					}
					case SDL_WINDOWEVENT_HIT_TEST: {
						onWindowHitTest({{event.window.timestamp}, _wx, _wy, _ww, _wh});
						break;
					}
					case SDL_DROPFILE: {
						onFileDrop({{event.drop.timestamp}, event.drop.file});
						SDL_free(event.drop.file);
						break;
					}
				}
			}
		}
	}
	#ifdef ARGON_DYNAMIC_RESIZE
		static int handleResize(void* instance, SDL_Event* event) {
			if(event->type == SDL_WINDOWEVENT && event->window.event == SDL_WINDOWEVENT_RESIZED) {
				Argon* a = (Argon*)instance;
				a->_ww = event->window.data1;
				a->_wh = event->window.data2;
				a->onWindowResize({{event->window.timestamp}, a->_wx, a->_wy, a->_ww, a->_wh});
			}
			return 0;
		}
	#endif

	void recreateTextures() {
		for(int16_t i = textures.size()-1; i >= 0; --i) {
			if(textures[i] != NULL) {
				SDL_DestroyTexture(textures[i]);
				textures[i] = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, _ww, _wh);
			}
			else {textures.erase(textures.begin() + i);}
		}
	}

	inline void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		SDL_SetRenderDrawColor(renderer, r, g, b, a);
		SDL_SetRenderDrawBlendMode(renderer, (a == 255) ? SDL_BLENDMODE_NONE : SDL_BLENDMODE_BLEND);
	}
	inline void setColor(uint32_t c) {
		SDL_SetRenderDrawColor(renderer, (uint8_t)(c>>24), (uint8_t)(c>>16), (uint8_t)(c>>8), (uint8_t)c);
		SDL_SetRenderDrawBlendMode(renderer, ((uint8_t)c) ? SDL_BLENDMODE_NONE : SDL_BLENDMODE_BLEND);
	}
};

#endif