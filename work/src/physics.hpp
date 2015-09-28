#include "comp308.hpp"
#include <vector>


class Rigidbody{
public:
  void update(unsigned float);
  void rollBack(unsigned float);
  Rigidbody(comp308::vec3 base,vector<vec3> mesh){
    /*set inertia tensor based on mesh data*/
    position = base;
  };
private:
  double mass;
  comp308::mat3 inertiaTensor;
  comp308::mat3 inverseInertiaTensor;
  comp308::vec3 position;
  comp308::mat3 rotation;
  comp308::vec3 linearVelocity;//current velocity
  comp308::vec3 angularVelocity;//current angular velocity
  comp308::vec3 force;//sum of all forces
  comp308::vec3 torque;//sum of all torques
};
class Physics{
public:
  void createRigidbody(comp308::vec3 position);
  vector<vec3> update(unsigned float);
  vector<vec3> rollBack(unsigned float);
private:
  unsigned float currentTime;
  std::vector<Rigidbody> objects;
};
