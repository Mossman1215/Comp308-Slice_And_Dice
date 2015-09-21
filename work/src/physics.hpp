#include "comp308.hpp"
#include <vector>

class Rigidbody{
public:
  void update(unsigned float);
  void rollBack(unsigned float);
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
  Rigidbody createRigidbody();
  void update(unsigned float);
  void rollBack(unsigned float);
private:
  unsigned float currentTime;
  std::vector<Rigidbody> objects;
};
