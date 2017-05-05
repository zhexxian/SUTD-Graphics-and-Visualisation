#include "simpleSystem.h"
#include <iostream>

using namespace std;

SimpleSystem::SimpleSystem()
{
	Vector3f startPos = (1, 1, 1);
	vector<Vector3f> state;
	
	state.push_back(startPos);
	this->setState(state);
}

// TODO: implement evalF
// for a given state, evaluate f(X,t)
vector<Vector3f> SimpleSystem::evalF(vector<Vector3f> state)
{
	vector<Vector3f> f;
	
	//X = (x, y, z)
	//f(X,t) = (-y, x, 0)

	// YOUR CODE HERE
	for(int i=0;i<state.size();i++){
	  Vector3f derivatives = Vector3f(-state[i].y(),state[i].x(),0);
	  f.push_back(derivatives);
	}

	return f;
}

// render the system (ie draw the particles)
void SimpleSystem::draw()
{

	//add for loop to make the sphere move
	for(int i=0; i<this->getState().size(); i++){
		Vector3f pos = this->m_vVecState[i];//YOUR PARTICLE POSITION
		glPushMatrix();
		glTranslatef(pos[0], pos[1], pos[2]);
		//cout << "translating:" << m_vVecState[i][0] << endl;
		glutSolidSphere(0.075f,10.0f,10.0f);
		glPopMatrix();
	}
}