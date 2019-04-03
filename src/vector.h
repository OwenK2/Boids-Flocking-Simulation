
#ifndef VECTOR_H
#define VECTOR_H
class Vector {
public:
	float x = 0;
	float y = 0;
	float z = 0;
	Vector();
	Vector(Vector& v);
	Vector(float _a);
	Vector(float _x, float _y);
	Vector(float _x, float _y, float _z);	
	bool operator==(Vector a);	
	bool operator==(Vector* a);
	bool operator!=(Vector a);	
	bool operator!=(Vector* a);
	void operator=(Vector a);	
	void operator=(Vector* a);
	void operator+=(Vector a);
	void operator+=(Vector* a);
	void operator+=(int a);
	void operator+=(float a);
	void operator-=(Vector a);
	void operator-=(Vector* a);
	void operator-=(int a);
	void operator-=(float a);
	void operator*=(Vector a);
	void operator*=(Vector* a);
	void operator*=(int a);
	void operator*=(float a);
	void operator/=(Vector a);
	void operator/=(Vector* a);
	void operator/=(int a);
	void operator/=(float a);
	void normalize();
	void log();
	void limit(int l);
	void limit(float l);
	void setMag(int m);
	void setMag(float m);
	float magSq();
	float mag();
	~Vector();
};
#endif