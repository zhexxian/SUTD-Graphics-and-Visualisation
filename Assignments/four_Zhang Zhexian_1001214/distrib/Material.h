#ifndef MATERIAL_H
#define MATERIAL_H

#include <cassert>
#include <vecmath.h>

#include "Ray.h"
#include "Hit.h"
#include "texture.hpp"

class Material
{
public:
	
	Material( const Vector3f& d_color ,const Vector3f& s_color=Vector3f::ZERO, float s=0):
	diffuseColor( d_color),specularColor(s_color), shininess(s)
	{

	}

	virtual ~Material()
	{

	}

	virtual Vector3f getDiffuseColor() const 
	{ 
		return  diffuseColor;
	}
    
///TODO:
///Implement Shade function that uses ambient, diffuse, specular and texture
	Vector3f Shade( const Ray& ray, const Hit& hit,
					const Vector3f& dirToLight, const Vector3f& lightColor ) {

		// Create texture color variable and default it to diffuse color (when there is no texture mapping)
		Vector3f textureColor = diffuseColor;
		// If the material has valid texture indicated by texture.valid(), then simply use the texture color instead of diffuseColor
		if (texture.valid()) {
			textureColor = texture(hit.texCoord[0], hit.texCoord[1]);
		}
						
		Vector3f normal = hit.getNormal();
		// The diffuse intensity depends on dirToLight and normal
		float kd = Vector3f::dot(dirToLight, normal);
		if (kd < 0) {
			kd = 0;
		}
		
		// diffuse factor is related to lightColor
		Vector3f diffuseFactor = kd * Vector3f(textureColor[0]*lightColor[0], textureColor[1]*lightColor[1], textureColor[2]*lightColor[2]);

		// The specular intensity depends on four quantities: shininess, rayDirection, dirToLight, and the normal. 
		Vector3f rayDirection = ray.getDirection();

		// Reflection: R = – L + 2 (L · N) N
		//Vector3f reflectedDirection = rayDirection - 2.0f * Vector3f::dot(rayDirection, normal) * rayDirection.normalized();
		Vector3f reflectedDirection = rayDirection - 2.0f * Vector3f::dot(rayDirection, normal) * normal.normalized();


		// Compute the specular shading intensity (base on condition, if specularShading is greater or less than 0)
		float specularShading = Vector3f::dot(dirToLight, reflectedDirection);
		if (specularShading > 0) {
			specularShading = pow(specularShading, shininess);
		} else {
			specularShading = 0.0f;
		}
		// specularColor = specularShading * specularColor * lightColor
		Vector3f specularFactor = specularShading * Vector3f(specularColor[0]*lightColor[0], specularColor[1]*lightColor[1], specularColor[2]*lightColor[2]);
		
		// Combining all color components
		return diffuseFactor + specularFactor;		

	}
	
	void loadTexture(const char * filename){
		texture.load(filename);
	}
	
	
protected:
	Texture texture;
	Vector3f diffuseColor;
	Vector3f specularColor;
	float shininess;
	
};



#endif // MATERIAL_H
