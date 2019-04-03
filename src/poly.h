#include <vector>
#include <SDL2/SDL.h>
struct Point {
	float x;
	float y;
};
struct Line {
	float x1;
	float y1;
	float x2;
	float y2;
};

class Poly {
private:
	SDL_Renderer* ren;
public:
	int cx;
	int cy;
	float w;
	float h;
	float t;
	float b;
	float l;
	float r;
	std::vector<Point> points;
	std::vector<Line> edges;
	Poly(SDL_Renderer* _ren,std::vector<Point> _points);
	Poly(SDL_Renderer* _ren,int _cx, int _cy, int sides, int r, float sa = 0.0);
	void calcSpecs();
	~Poly();
	bool pointInPoly(int x, int y);
	void stroke();
	void fill();
};