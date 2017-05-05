#include "TimeStepper.hpp"

///TODO: implement Explicit Euler time integrator here
void ForwardEuler::takeStep(ParticleSystem* particleSystem, float stepSize)
{
	vector<Vector3f> state = particleSystem->getState();
	vector<Vector3f> derivatives = particleSystem->evalF(state);
	vector<Vector3f> newState;
	for(int i=0;i<state.size();i++){
		Vector3f newState_i = state[i]+stepSize*derivatives[i];
		newState.push_back(newState_i);
	}

	particleSystem->setState(newState);
}

///TODO: implement Trapzoidal rule here
void Trapzoidal::takeStep(ParticleSystem* particleSystem, float stepSize)
{
	vector<Vector3f> step1state = particleSystem->getState();
	vector<Vector3f> step1derivatives = particleSystem->evalF(step1state);
	vector<Vector3f> step2state;
	for(int i=0;i<step1state.size();i++){
		Vector3f step2state_i = step1state[i]+stepSize*step1derivatives[i];
		step2state.push_back(step2state_i);
	}
	vector<Vector3f> step2derivatives = particleSystem->evalF(step2state);

	vector<Vector3f> newState;
	for(int i=0;i<step1state.size();i++){
		Vector3f newState_i = step1state[i]+(0.5*stepSize)*(step1derivatives[i]+step2derivatives[i]);
		newState.push_back(newState_i);
	}

	particleSystem->setState(newState);
}
