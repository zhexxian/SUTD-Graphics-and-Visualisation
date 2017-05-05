
#ifndef CAMERA_H
#define CAMERA_H

#include "Ray.h"
#include <vecmath.h>
#include <float.h>
#include <cmath>


class Camera
{
public:
	//generate rays for each screen-space coordinate
	virtual Ray generateRay( const Vector2f& point ) = 0 ; 
	
	virtual float getTMin() const = 0 ; 
	virtual ~Camera(){}
protected:
	Vector3f center; 
	Vector3f direction;
	Vector3f up;
	Vector3f horizontal;

};

///TODO: Implement Perspective camera
///Fill in functions and add more fields if necessary
class PerspectiveCamera: public Camera
{
public:
	PerspectiveCamera(const Vector3f& center, const Vector3f& direction,const Vector3f& up , float angle){
		this->center = center;
		this->angle = angle;
		this->w = direction;
		this->u = Vector3f::cross(w, up);
		this->v = Vector3f::cross(u, w);
		this->direction = direction.normalized();
		this->up = up.normalized();
		this->horizontal = Vector3f::cross(this->direction, this->up).normalized();
		this->up = Vector3f::cross(this->horizontal, this->direction).normalized();
	}

	virtual Ray generateRay( const Vector2f& point){
		//d = 1/(tan(alpha/2))
		float d = 1.0f/tan(this->angle/2.0f);
		//r = p-e = (x*u, d*w)
		//Vector3f r = Vector3f(Vector3f::dot(point.x(),u),Vector3f::dot(point.y(),v), Vector3f::dot(d, w));
		//Vector3f rayDirection = r.normalized();
		Vector3f rayDirection = (point.x() * this->horizontal + point.y() * this->up + d * this->direction).normalized();
		return Ray(this->center,rayDirection);
	}

	virtual float getTMin() const { 
		return 0.0f;
	}

private:
	Vector3f u;
	Vector3f v;
	Vector3f w;
	Vector3f center;
	float angle;
};

#endif //CAMERA_H

		