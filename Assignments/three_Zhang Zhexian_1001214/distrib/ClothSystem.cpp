#include "ClothSystem.h"
#include <iostream>

extern bool breezeOn;
extern bool showWire;


//TODO: Initialize here
ClothSystem::ClothSystem(int gridSize)
{
	vector<Vector3f> state;
	// gridSize = 3 means it is made of 3 * 3 particles
	m_numParticles = gridSize * gridSize;
	// fill in code for initializing the state based on the number of particles
	for (int i = 1; i < gridSize+1; i++) {
		for(int j = 1; j < gridSize+1; j++){
			// for this system, we care about the position and the velocity

			// set position i and j to form a uniform grid
			state.push_back(Vector3f((i)*0.5, (j)*0.5, 0));
			// set all initial velocity to zero
			state.push_back(Vector3f(0, 0, 0));
		}
	}
	this->setState(state);
}


vector<Vector3f> ClothSystem::getPosition(vector<Vector3f> state) {
	vector<Vector3f> position;
	for (int i = 0; i < state.size(); i=i+2) {
		// the option is to store a big Vector3f array of size 2n where 
		// positions are stored at even indices and velocities at odd indices. 
		position.push_back(state[i]);
	}
	return position;
};
vector<Vector3f> ClothSystem::getVelocity(vector<Vector3f> state) {
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
Vector3f ClothSystem::computeGravity(float mass){
	Vector3f f = Vector3f(0, -9.81, 0) * mass;
	return f;
}
		
// viscous drag force
Vector3f ClothSystem::computeDrag(float dragConstant, Vector3f velocity){
	Vector3f f = -1.0 * dragConstant * velocity;
	return f;
}	
		
// spring force
Vector3f ClothSystem::computeSpring(float restLenght, float springConstant, Vector3f position1, Vector3f position2){
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
vector<Vector3f> ClothSystem::evalF(vector<Vector3f> state)
{

	vector<Vector3f> f;
	vector<Vector3f> position = getPosition(state);
	vector<Vector3f> velocity = getVelocity(state);
	Vector3f force;
	int index;

	float mass = 0.5;
	float dragConstant = 0.25;
	float restLength_structural = 0.5;
	float restLength_shear = 0.5*pow(2.0, 0.5);
	float restLength_flex = 1.0;
	float springConstant = 100.5;

	float gridSize = pow(m_numParticles, 0.5);

	// YOUR CODE HERE
	for (int i = 1; i < gridSize+1; i++) {
		for (int j = 1; j < gridSize+1; j++) {
			index = i+((j-1)*gridSize)-1;
			if(i==gridSize && j==1){
				// top right corner particle's velocity and acceleration are zero
				f.push_back(Vector3f(0,0,0));
				f.push_back(Vector3f(0,0,0));
			}
			else if(i==gridSize && j==gridSize){
				// top left corner particle's velocity and acceleration are zero
				f.push_back(Vector3f(0,0,0));
				f.push_back(Vector3f(0,0,0));
			}
			else{
				// velocity
				f.push_back(velocity[index]);

				// basic forces
				force = computeGravity(mass);
				force = force + computeDrag(dragConstant, velocity[index]);

				if(breezeOn){
					force = force + Vector3f(0.5, 2.5, 1.3);
					cout << "breeze" <<endl;
				}

				// spring forces
				// structural springs
				if(i!=1){
					force = force + computeSpring(restLength_structural, springConstant, position[index], position[index-1]);
				}
				if(j!=1){
					force = force + computeSpring(restLength_structural, springConstant, position[index], position[index-gridSize]);
				}
				if(i!=gridSize){
					force = force + computeSpring(restLength_structural, springConstant, position[index], position[index+1]);
				}
				if(j!=gridSize){
					force = force + computeSpring(restLength_structural, springConstant, position[index], position[index+gridSize]);
				}
				

				// shear springs
				if(i!=1 && j!=gridSize){
					force = force + computeSpring(restLength_shear, springConstant, position[index], position[index+gridSize-1]);
				}
				if(i!=1 && j!=1){
					force = force + computeSpring(restLength_shear, springConstant, position[index], position[index-gridSize-1]);
				}
				if(i!=gridSize && j!=gridSize){
					force = force + computeSpring(restLength_shear, springConstant, position[index], position[index+gridSize+1]);
				}
				if(i!=gridSize && j!=1){
					force = force + computeSpring(restLength_shear, springConstant, position[index], position[index-gridSize+1]);
				}
				

				// flex springs
				if(i>2){
					force = force + computeSpring(restLength_flex, springConstant, position[index], position[index-2]);
				}
				if(i<gridSize-2){
					force = force + computeSpring(restLength_flex, springConstant, position[index], position[index+2]);
				}
				if(j>2){
					force = force + computeSpring(restLength_flex, springConstant, position[index], position[index-gridSize*2]);
				}
				if(j<gridSize-2){
					force = force + computeSpring(restLength_flex, springConstant, position[index], position[index+gridSize*2]);
				}

				// acceleration = force / mass
				f.push_back(force/mass);
			}
		}	
	}

	return f;
	
	//return vector<Vector3f>();
}

///TODO: render the system (ie draw the particles)
void ClothSystem::draw()
{
	vector<Vector3f> position = this->getPosition(this->getState());
	float gridSize = pow(m_numParticles, 0.5);
	int index;
	for (int i = 1; i < gridSize+1; i++) {
		for (int j = 1; j < gridSize+1; j++) {
			index = i+((j-1)*gridSize)-1;

			if(showWire) {
			
				Vector3f pos = position[index];//  position of particle i. YOUR CODE HERE
				Vector3f pos_up = position[index+1];
				Vector3f pos_down = position[index-1];
				Vector3f pos_left = position[index-gridSize];
				Vector3f pos_right = position[index+gridSize];

				glPushMatrix();
				glTranslatef(pos[0], pos[1], pos[2] );
				glutSolidSphere(0.075f,10.0f,10.0f);
				glPopMatrix();
			
			
				glLineWidth(10.0);
				glClearColor(0.8, 0.8, 0.8, 0.0);
				glBegin(GL_LINES);
				if(i!=1){
					glVertex3f(pos[0], pos[1], pos[2]);
					glVertex3f(pos_down[0], pos_down[1], pos_down[2]);
				}
				if(j!=1){
					glVertex3f(pos[0], pos[1], pos[2]);
					glVertex3f(pos_left[0], pos_left[1], pos_left[2]);
				}
				if(i!=gridSize){
					glVertex3f(pos[0], pos[1], pos[2]);
					glVertex3f(pos_up[0], pos_up[1], pos_up[2]);
				}
				if(j!=gridSize){
					glVertex3f(pos[0], pos[1], pos[2]);
					glVertex3f(pos_right[0], pos_right[1], pos_right[2]);
				}
				glEnd();
				glFlush();

			}
			
			if(i!=1 && j!=gridSize){
				Vector3f p2 = position[index];
				Vector3f p1 = position[index-1];
				Vector3f p3 = position[index-1+gridSize];
				Vector3f p4 = position[index+gridSize];

				Vector3f normal1 = Vector3f::cross(p3-p1,p2-p1).normalized();
				Vector3f normal2 = Vector3f::cross(p2-p3,p1-p3).normalized();
				Vector3f normal3 = Vector3f::cross(p1-p2,p3-p2).normalized();
				Vector3f normal4 = Vector3f::cross(p3-p2,p4-p2).normalized();
				Vector3f normal5 = Vector3f::cross(p4-p3,p2-p3).normalized();
				Vector3f normal6 = Vector3f::cross(p2-p4,p3-p4).normalized();

				glBegin(GL_TRIANGLES);
				glNormal3f(normal1.x(), normal1.y(), normal1.z());
				glVertex3f(p1.x(), p1.y(), p1.z());
				glNormal3f(normal2.x(), normal2.y(), normal2.z());
				glVertex3f(p3.x(), p3.y(), p3.z());
				glNormal3f(normal3.x(), normal3.y(), normal3.z());
				glVertex3f(p2.x(), p2.y(), p2.z());
				glEnd();
				glBegin(GL_TRIANGLES);
				glNormal3f(normal4.x(), normal4.y(), normal4.z());
				glVertex3f(p2.x(), p2.y(), p2.z());
				glNormal3f(normal5.x(), normal5.y(), normal5.z());
				glVertex3f(p3.x(), p3.y(), p3.z());
				glNormal3f(normal6.x(), normal6.y(), normal6.z());
				glVertex3f(p4.x(), p4.y(), p4.z());
				glEnd();

			}
		}
	}
}

