#ifndef CLOTHSYSTEM_H
#define CLOTHSYSTEM_H

#include <vecmath.h>
#include <vector>
#ifdef _WIN32
#include "GL/freeglut.h"
#else
#include <GL/glut.h>
#endif

#include "particleSystem.h"

class ClothSystem: public ParticleSystem
{
///ADD MORE FUNCTION AND FIELDS HERE
public:
	ClothSystem(int gridSize);

	vector<Vector3f> evalF(vector<Vector3f> state);

	vector<Vector3f> getPosition(vector<Vector3f> state);
	vector<Vector3f> getVelocity(vector<Vector3f> state);

	Vector3f ClothSystem::computeGravity(float mass);
	Vector3f ClothSystem::computeDrag(float dragConstant, Vector3f velocity);
	Vector3f ClothSystem::computeSpring(float restLenght, float springConstant, Vector3f position1, Vector3f position2);

	void draw();

private:

};


#endif
