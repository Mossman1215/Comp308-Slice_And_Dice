#include "comp308.hpp"
#include <vector>


class Rigidbody{
public:
	comp308::vec3 update(float);
	void rollBack(float);
	Rigidbody(comp308::vec3 base,std::vector<comp308::vec3> mesh,double mass){
		/*set inertia tensor based on mesh data*/
		position = base;
		this->mass = mass;
	};
	void addForce(comp308::vec3 force);
	void addTorque(comp308::vec4 quat);
private:
	double mass;
	comp308::mat3 inertiaTensor;
	comp308::mat3 inverseInertiaTensor;
	comp308::vec3 position;
	comp308::vec4 rotation;
	comp308::vec3 linearVelocity;//current velocity
	comp308::vec3 angularVelocity;//current angular velocity
	comp308::vec3 force;//sum of all forces
	comp308::vec3 torque;//sum of all torques
	std::vector<comp308::vec3> mesh;
};
class Physics{
public:
	void createRigidbody(comp308::vec3 position);
	comp308::vec3 update(float);
	std::vector<comp308::vec3> rollBack(float);
private:
	float currentTime;
	std::vector<Rigidbody> objects;
};
