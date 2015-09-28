#include "physics.hpp"
#include "comp308.hpp"
using namespace comp308;

void Rigidbody::addForce(vec3 force){
	this->force = force;
}
comp308::vec3 Rigidbody::update(float delta){
	//apply forces
	vec3 acceleration = force/mass;
	//change position
	position = position + (acceleration*delta);
	return position;
}
