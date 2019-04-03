#include "poly.h"
#include <cmath>
#include <vector>
#include <limits>

Poly::Poly(SDL_Renderer* _ren,std::vector<Point> _points) : ren(_ren), points(_points) {
	calcSpecs();
}
Poly::Poly(SDL_Renderer* _ren,int _cx, int _cy, int sides, int r, float sa) : ren(_ren), cx(_cx), cy(_cy) {
	float a = sa;
	for(int i = 0;i < sides; ++i) {
		Point pt;
		pt.x = (float)cx + (r * cos(a));
		pt.y = (float)cy + (r * sin(a));
		points.push_back(pt);
		a += M_PI*2/sides;
	}
	calcSpecs();
}
Poly::~Poly() {
	
}

void Poly::calcSpecs() {
	l = INFINITY;r = -INFINITY;t = INFINITY;b = -INFINITY;
	for(int i = 0;i < points.size(); ++i) {
		if(points[i].x < l) {l = points[i].x;}
		if(points[i].x > r) {r = points[i].x;}
		if(points[i].y < t) {t = points[i].y;}
		if(points[i].y > b) {b = points[i].y;}
		int j = i == points.size() - 1 ? j = 0 : j = i+1;
		Line l = {points[i].x,points[i].y,points[j].x,points[j].y};
		edges.push_back(l);
	}
	h = b - t;
	w = r - l;
	cx = l + w/2;
	cy = t + h/2;
}
bool Poly::pointInPoly(int x, int y) {
  int wn = 0;
  for(auto edge : edges) {
    if(edge.y1 <= y) {
      if(edge.y2 > y) {
      	if(((edge.x2 - edge.x1) * (y - edge.y1) - (x - edge.x1) * (edge.y2 - edge.y1)) > 0) {
          ++wn;
      	}
      }
    }
    else {
      if(edge.y2  <= y) {
        if(((edge.x2 - edge.x1) * (y - edge.y1) - (x - edge.x1) * (edge.y2 - edge.y1)) < 0) {
          --wn;
        }
      }
    }
  }
  return wn != 0;
}
void Poly::stroke() {
	for(auto edge : edges) {
		SDL_RenderDrawLine(ren, edge.x1, edge.y1, edge.x2, edge.y2);
	}
}
void Poly::fill() {
	for(int x = l;x <= r;++x) {
		for(int y = t;y <= b;++y) {
			if(pointInPoly(x,y)) {
				SDL_RenderDrawPoint(ren,x,y);
			}
		}
	}
}


