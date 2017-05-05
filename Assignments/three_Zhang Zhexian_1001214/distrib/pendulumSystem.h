
#ifndef PENDULUMSYSTEM_H
#define PENDULUMSYSTEM_H

#include <vecmath.h>
#include <vector>
#ifdef _WIN32
#include "GL/freeglut.h"
#else
#include <GL/glut.h>
#endif

#include "particleSystem.h"

class PendulumSystem: public ParticleSystem
{
public:
	PendulumSystem(int numParticles);
	
	vector<Vector3f> evalF(vector<Vector3f> state);
	vector<Vector3f> getPosition(vector<Vector3f> state);
	vector<Vector3f> getVelocity(vector<Vector3f> state);

	Vector3f PendulumSystem::computeGravity(float mass);
	Vector3f PendulumSystem::computeDrag(float dragConstant, Vector3f velocity);
	Vector3f PendulumSystem::computeSpring(float restLenght, float springConstant, Vector3f position1, Vector3f position2);
	
	void draw();
	
};

#endif
