#ifndef PLANE_H
#define PLANE_H

#include "Object3D.h"
#include <vecmath.h>
#include <cmath>
using namespace std;
///TODO: Implement Plane representing an infinite plane
///choose your representation , add more fields and fill in the functions
class Plane: public Object3D
{
public:
	Plane(){
	}
	Plane( const Vector3f& normal , float d , Material* m):Object3D(m){
		//material private variable already taken care of by Object3D class
		this->normal = normal;
		this->d = -d;
	}
	~Plane(){}
	virtual bool intersect( const Ray& r , Hit& h , float tmin){
				
		//Plane is defined by a point and a normal
		//Explicit: R0 + t*Rd
		//Implicit: n*P + D = 0

		//To check intersection, t = -(D + n*R0) / n*Rd
		Vector3f R0 = r.getOrigin();
		Vector3f Rd = r.getDirection();
		float t = -1.0f*(d+Vector3f::dot(normal,R0)) / Vector3f::dot(normal, Rd);
		//Checking validity of t
		if(t<tmin || t>=h.getT()){
			return false;
		}
		h.set(t, this->material, this->normal.normalized());
		return true;
	}

protected:
	Vector3f normal;
	float d;

};
#endif //PLANE_H
