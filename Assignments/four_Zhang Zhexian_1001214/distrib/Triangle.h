#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Object3D.h"
#include <vecmath.h>
#include <cmath>
#include <iostream>

using namespace std;
///TODO: implement this class.
///Add more fields as necessary,
///but do not remove hasTex, normals or texCoords
///they are filled in by other components
class Triangle: public Object3D
{
public:
	
	Triangle(){
		this->a = Vector3f(0.0f, 0.0f, 0.0f);
		this->b = Vector3f(0.0f, 0.0f, 0.0f);
		this->c = Vector3f(0.0f, 0.0f, 0.0f);
        this->hasTex = false;
	}
	
    ///@param a b c are three vertex positions of the triangle
	Triangle( const Vector3f& a, const Vector3f& b, const Vector3f& c, Material* m):Object3D(m){
		this->a = a;
		this->b = b;
		this->c = c;
        this->hasTex = false;
	}
	

	virtual bool intersect( const Ray& ray,  Hit& hit , float tmin){
		Vector3f rayOrigin = ray.getOrigin();
		Vector3f rayDirection = ray.getDirection();

		//Use Cramer's rule to resolve for beta, gamma, and t
		Matrix3f A = Matrix3f(
			a.x()-b.x(), a.x()-c.x(), rayDirection.x(),
			a.y()-b.y(), a.y()-c.y(), rayDirection.y(),
			a.z()-b.z(), a.z()-c.z(), rayDirection.z()

		);
		Matrix3f beta_c = Matrix3f(
			a.x()-rayOrigin.x(), a.x()-c.x(), rayDirection.x(),
			a.y()-rayOrigin.y(), a.y()-c.y(), rayDirection.y(),
			a.z()-rayOrigin.z(), a.z()-c.z(), rayDirection.z()
		);
		Matrix3f gamma_c = Matrix3f(
			a.x()-b.x(), a.x()-rayOrigin.x(), rayDirection.x(),
			a.y()-b.y(), a.y()-rayOrigin.y(), rayDirection.y(),
			a.z()-b.z(), a.z()-rayOrigin.z(), rayDirection.z()
		);
		Matrix3f t_c = Matrix3f(
			a.x()-b.x(), a.x()-c.x(), a.x()-rayOrigin.x(),
			a.y()-b.y(), a.y()-c.y(), a.y()-rayOrigin.y(),
			a.z()-b.z(), a.z()-c.z(), a.z()-rayOrigin.z()
		);

		float determinantA = A.determinant();
		float beta = beta_c.determinant() / determinantA;
		float gamma = gamma_c.determinant() / determinantA;
		float t = t_c.determinant() / determinantA;

		//alpha = 1 - beta - gamma
		float alpha = 1.0f - beta - gamma;

		//Condition for intersection:
		//beta + gamma <= 1 & beta >= 0 & gamma >= 0, and t > tmin 
		if(beta + gamma <= 1){
			if(beta >= 0 && gamma >=0){
				if(t >= tmin && t < hit.getT()){
					//Interpolated normals
					Vector3f normal = alpha * normals[0] + beta * normals[1] + gamma * normals[2];
					hit.set(t, material, normal.normalized());
					//Interpolated texture coordinates
					hit.setTexCoord(alpha * texCoords[0] + beta * texCoords[1] + gamma * texCoords[2]);
					return true;
				}
			}
		}
		
		return false;
	}

	bool hasTex;
	Vector3f normals[3];
	Vector2f texCoords[3];
protected:
	Vector3f a;
	Vector3f b;
	Vector3f c;

};

#endif //TRIANGLE_H