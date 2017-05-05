#include "RayTracer.h"
#include "Camera.h"
#include "Ray.h"
#include "Hit.h"
#include "Group.h"
#include "Material.h"
#include "Light.h"

#define EPSILON 0.001

//IMPLEMENT THESE FUNCTIONS
//These function definitions are mere suggestions. Change them as you like.


//reflection direction
Vector3f mirrorDirection( const Vector3f& normal, const Vector3f& incoming)
{
	return (incoming - 2*(Vector3f::dot(incoming, normal))*(normal)).normalized(); //R = V – 2 (V · N) N 
}

//refraction direction
bool transmittedDirection( const Vector3f& normal, const Vector3f& incoming, 
        float index_n, float index_nt, 
        Vector3f& transmitted)
{
	float ratioOfRefraction = index_n/index_nt; //index_n and index_nt are refraction indices of the current object and the object the ray is going into
	float nDotI = Vector3f::dot(normal,incoming);
	float squareRoot_term = 1-ratioOfRefraction *ratioOfRefraction *(1-nDotI*nDotI);
	if (squareRoot_term < 0){
		return false; //total reflection, no refraction
	}
	else{
		transmitted = ratioOfRefraction *(incoming - normal*nDotI) - normal*sqrt(squareRoot_term); //refracted direction
		transmitted.normalize();
		return true;
	}
}


RayTracer::RayTracer( SceneParser * scene, int max_bounces, bool hasShadow //add shadow argument
	):m_scene(scene)
{
  g=scene->getGroup();
  m_maxBounces = max_bounces;
  shadow = hasShadow;
}

RayTracer::~RayTracer()
{
}

Vector3f RayTracer::traceRay( Ray& ray, float tmin, int bounces,
        float refr_index, Hit& hit ) const
{
	Vector3f color;

	
	if(g->intersect(ray, hit, tmin)) //check if ray intersects group
	{
		//ambient
		color = (m_scene->getAmbientLight())*(hit.getMaterial()->getDiffuseColor()); 


		for(int i=0; i<m_scene->getNumLights(); i++){

			//shadow
			Light* light = m_scene->getLight(i);
			Vector3f directionToLight;
			float distanceToLight;
			Vector3f lightColor;
			light->getIllumination(ray.pointAtParameter(hit.getT()), directionToLight, lightColor, distanceToLight);
			Vector3f hitPoint = ray.pointAtParameter(hit.getT());
			
			if(shadow){
				Ray shadowRay = Ray(hitPoint, directionToLight);
				Hit shadowHit = Hit(distanceToLight, NULL, NULL);					
				bool groupIntersect = g->intersect(shadowRay, shadowHit, EPSILON);
				//diffuse (if no intersection/no shadow)
				if (shadowHit.getT() >= distanceToLight){
					color += hit.getMaterial()->Shade(ray, hit, directionToLight, lightColor);       
				}
			}
			else{
				//diffuse (if shadow is disabled)
				color += hit.getMaterial()->Shade(ray, hit, directionToLight, lightColor);
			}
		}

		//recursion
		if (bounces>m_maxBounces-1){
			return color;
		}

		Vector3f reflectionColor;
		Vector3f refractionColor;
		float R;
		if(bounces > 0){

			//reflection
			Vector3f reflectionDirection  = mirrorDirection(hit.getNormal().normalized(),ray.getDirection().normalized());

			Vector3f intersectionPoint = ray.pointAtParameter(hit.getT());
			Ray reflectionRay = Ray(intersectionPoint,reflectionDirection); //reflectionRay originates from the intersection point
			Hit reflectionHit = Hit(FLT_MAX, NULL, Vector3f(0.0f, 0.0f, 0.0f));
			bool reflectionIntersection = g->intersect( reflectionRay, reflectionHit, 0.0f );

			reflectionColor = traceRay(reflectionRay, EPSILON, refr_index, bounces - 1, reflectionHit)*hit.getMaterial()->getSpecularColor();


			//refraction
			//if(hit.getMaterial()->getRefractionIndex() > 0) //Cast refraction rays for transparent materials (if Material::refractionIndex>0)
			//{
				Vector3f normal =  hit.getNormal().normalized();
				float index_nt = hit.getMaterial()->getRefractionIndex(); //index_nt is the refraction index of the object the ray is going into
				if (Vector3f::dot(ray.getDirection(),normal) > 0)
				{
					normal *= -1.0f; //if the ray is currently inside the object, the normal will be positive
					index_nt = 1.0f;	
				}
				Vector3f refractionDirection;
				bool refraction = transmittedDirection(normal, ray.getDirection(), refr_index, index_nt, refractionDirection);//check if not total reflection

				if(refraction){
					Ray refractionRay = Ray(intersectionPoint,refractionDirection);
					Hit refractionHit = Hit();

					refractionColor = traceRay(refractionRay, EPSILON, index_nt, bounces - 1, refractionHit)*hit.getMaterial()->getSpecularColor();

					//blending of reflection and refraction (Fresnel’s equation)
					float c = 0.0f;
					if(refr_index <= index_nt){
						c = abs(Vector3f::dot(ray.getDirection(),normal));
					}
					else{
						c = abs(Vector3f::dot(refractionDirection,normal));
					}
					float R_0 = pow(((index_nt - refr_index)/(index_nt + refr_index)),2);
					R = R_0 + (1-R_0)*pow((1-c),5);
				}
			//}
		}
		//weighting of reflection and refraction
		color += R*reflectionColor + (1-R)*refractionColor; 
	}
	else {
		color = m_scene->getBackgroundColor(ray.getDirection());
	}
	return color;
}
