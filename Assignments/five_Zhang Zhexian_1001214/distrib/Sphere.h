#ifndef SPHERE_H
#define SPHERE_H

#include "Object3D.h"
#include <vecmath.h>
#include <cmath>

#include <iostream>
using namespace std;
///TODO:
///Implement functions and add more fields as necessary
class Sphere: public Object3D
{
public:
	Sphere(){ 
		//unit ball at the center
		center = Vector3f(0.0, 0.0, 0.0);
		radius = 1.0;
	}

	Sphere( Vector3f center , float radius , Material* material ):Object3D(material){
		//material private variable already taken care of by Object3D class
		this->center = center;
		this->radius = radius;
	}
	

	~Sphere(){}

	virtual bool intersect( const Ray& r , Hit& h , float tmin){
		//Ray-sphere intersection

		//Translate the sphere to make the centre at (0,0,0)
		Vector3f origin = r.getOrigin() - this->center;
		//at^2 + bt + c = 0 
		float a = Vector3f::dot(r.getDirection(), r.getDirection());
		float b = 2*Vector3f::dot(origin, (r.getDirection()));
		float c = Vector3f::dot(origin, origin) - pow(this->radius, 2.0f);
		float d = pow(b,2.0f)-4.0f*a*c;
		//check determinant
		if(d<0){
			return false;
		}
		float t1 = (sqrt(d)-b)/(2.0f*a);
		float t2 = (-sqrt(d)-b)/(2.0f*a);
		//check tmin
		if(t1<tmin && t2<tmin){
			return false;
		}
		//take the smaller root
		float t;
		if(t1 >= tmin && t2 >= tmin){
			if(t1 >= t2){
				t=t2;
			}
			else t=t1;
		}else if (t2 >= tmin){
			t=t2;
		}else if (t1 >= tmin){
			t=t1;
		}else{
			return false;
		}
		//sphere normal
		Vector3f q = r.pointAtParameter(t)-this->center;
		Vector3f n = q.normalized(); 
		//update Hit
		float currentT = h.getT();
		if(t < currentT){
			h.set(t, this->material, n);
			return true;
		}
		return false;
	}

protected:
	Vector3f center;
	float radius;
};


#endif

