#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <vecmath.h>
#include "Object3D.h"
///TODO implement this class
///So that the intersect function first transforms the ray
///Add more fields as necessary
class Transform: public Object3D
{
public: 
  Transform(){}
 Transform( const Matrix4f& m, Object3D* obj ):o(obj){
	 //Transform will store a pointer to an Object3D
	 this->m = m;
	 this->o = obj;
    
  }
  ~Transform(){
  }
  virtual bool intersect( const Ray& r , Hit& h , float tmin){
	  //New origin: originOS = M-1 * originWS 
	  Vector4f newOrigin = m.inverse()*Vector4f(r.getOrigin(), 1.0f);
	  //New direction: directionOS = M-1 * directionWS 
	  Vector4f newDirection = m.inverse()*Vector4f(r.getDirection(), 0.0f);
	  Ray newRay = Ray(newOrigin.xyz(), newDirection.xyz());
	  return o->intersect( newRay , h , tmin);
  }

 protected:
  Object3D* o; //un-transformed object	
  Matrix4f m;
};

#endif //TRANSFORM_H