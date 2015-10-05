#include "physics.hpp"
#include "comp308.hpp"
using namespace comp308;
void Rigidbody::addTorque(vec4 quat){
	this->torque = vec3(0,0,0);
}
void Rigidbody::addForce(vec3 force){
	this->force += force;
}
comp308::vec3 Rigidbody::update(float delta){
	//apply forces
	vec3 acceleration = force/mass;
	//change position
	if(position.y + (acceleration.y * delta) > 0){
		position = position + (acceleration*delta);
	}else {
		addForce(vec3(0,-force.y,0));
	}
	return position;
}
