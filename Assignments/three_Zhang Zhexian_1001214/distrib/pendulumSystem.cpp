#include <iostream>
#include "pendulumSystem.h"

PendulumSystem::PendulumSystem(int numParticles):ParticleSystem(numParticles)
{
	m_numParticles = numParticles;
	vector<Vector3f> state;
	// fill in code for initializing the state based on the number of particles
	for (int i = 0; i < m_numParticles; i++) {
		
		// for this system, we care about the position and the velocity

		// set position depends on the index of the particle, so that all particles form a diagonal line with equal intervals
		state.push_back(Vector3f(i+0.5, i+0.5, 0));
		// set all initial velocity to zero
		state.push_back(Vector3f(0, 0, 0));
	}
	this->setState(state);
}

vector<Vector3f> PendulumSystem::getPosition(vector<Vector3f> state) {
	vector<Vector3f> position;
	for (int i = 0; i < state.size(); i=i+2) {
		// the option is to store a big Vector3f array of size 2n where 
		// positions are stored at even indices and velocities at odd indices. 
		position.push_back(state[i]);
	}
	return position;
};
vector<Vector3f> PendulumSystem::getVelocity(vector<Vector3f> state) {
	vector<Vector3f> velocity;
	for (int i = 0; i < state.size(); i=i+2) {
		// the option is to store a big Vector3f array of size 2n where 
		// positions are stored at even indices and velocities at odd indices. 
		velocity.push_back(state[i+1]);
	}
	return velocity;
};

// implement the gravity force, viscous drag force, and spring force

// gravity force
Vector3f PendulumSystem::computeGravity(float mass){
	Vector3f f = Vector3f(0, -9.81, 0) * mass;
	return f;
}
		
// viscous drag force
Vector3f PendulumSystem::computeDrag(float dragConstant, Vector3f velocity){
	Vector3f f = -1.0 * dragConstant * velocity;
	return f;
}	
		
// spring force
Vector3f PendulumSystem::computeSpring(float restLenght, float springConstant, Vector3f position1, Vector3f position2){
	// spring force depends on the number of particles
	Vector3f f;
	if(this->m_numParticles == 0){
		f = Vector3f(0, 0, 0);
	}
	else {
		Vector3f distanceVec = Vector3f(position1.x()-position2.x(), position1.y()-position2.y(), position1.z()-position2.z());
		float distance = (distanceVec).abs();
		f = -1.0 * springConstant * (distance - restLenght) * (distanceVec.normalized());	
	}
	return f;
}	

// TODO: implement evalF
// for a given state, evaluate f(X,t)

vector<Vector3f> PendulumSystem::evalF(vector<Vector3f> state)
{
	vector<Vector3f> f;
	vector<Vector3f> position = getPosition(state);
	vector<Vector3f> velocity = getVelocity(state);
	Vector3f force;

	float mass = 0.5;
	float dragConstant = 5.25;
	float restLenght = 0.6;
	float springConstant = 100.5;

	// the first two entries (initial velocity and accelerations) in f are zero vectors
	f.push_back(Vector3f(0,0,0));
	f.push_back(Vector3f(0,0,0));

	// YOUR CODE HERE
	for (int i = 1; i < m_numParticles; i++) {
		// velocity
		f.push_back(velocity[i]);

		// acceleration = force / mass
		
		force = computeGravity(mass);
		force = force + computeDrag(dragConstant, velocity[i]);

		// there is more than one spring force on each particle (except first and last particle)

		force = force + computeSpring(restLenght, springConstant, position[i], position[i-1]);

		if(i<m_numParticles-2){
			force = force + computeSpring(restLenght, springConstant, position[i], position[i+1])
				+ computeSpring(restLenght, springConstant, position[i], position[i+2]);
		}
		else if(i<m_numParticles-1){
			force = force + computeSpring(restLenght, springConstant, position[i], position[i+1]);
		}
		else{}
		//cout << "force[0]: " << force.x() << " force[1]: " << force.y() << " force[2]: " << force.z() << endl;

		 
		f.push_back(force/mass);
	}

	return f;
}

// render the system (ie draw the particles)
void PendulumSystem::draw()
{
	vector<Vector3f> position = this->getPosition(this->getState());
	for (int i = 0; i < m_numParticles; i++) {
		Vector3f pos = position[i];//  position of particle i. YOUR CODE HERE
		glPushMatrix();
		glTranslatef(pos[0], pos[1], pos[2] );
		glutSolidSphere(0.075f,10.0f,10.0f);
		glPopMatrix();
	}
}

