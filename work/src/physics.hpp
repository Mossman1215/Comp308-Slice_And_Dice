/*
 * physics module for comp308 final project
 * 2015 Tomas Cantwell cantwetoma@myvuw.ac.nz
 * using code from Studio freya http://studiofreya.com/3d-math-and-physics/simple-aabb-vs-aabb-collision-detection/
 * and Miguel Casillas http://www.miguelcasillas.com/?p=30
 */
#include "comp308.hpp"
#include <vector>
#include <limits>

struct TAABB
{
  comp308::vec3 m_vecMax;
  comp308::vec3 m_vecMin;
  comp308::vec3 position;
};

class Rigidbody{
public:
	comp308::vec3 update(float);
	void rollBack(float);
        TAABB boundary;
	comp308::vec3 position;
  Rigidbody(comp308::vec3 base,std::vector<comp308::vec3> mesh,double mass,unsigned int numPoints,comp308::vec3 parentForce){
		/*set inertia tensor based on mesh data*/
		position = base;
		this->mass = mass;
		this->mesh = mesh;
		//calculate TAABB parts
		  //find max values for x,y,z
		findMax();
		  //find min values for x,y,z
		findMin();
		boundary.position = base;
		comp308::vec3 momentum = parentForce*(numPoints+0.0/mesh.size());
		addForce(momentum);
	};
	void addForce(comp308::vec3 force);
	void addTorque(comp308::vec4 quat);
private:
	double mass;
	comp308::mat3 inertiaTensor;
	comp308::mat3 inverseInertiaTensor;
	comp308::vec4 rotation;
	comp308::vec3 linearVelocity;//current velocity
	comp308::vec3 angularVelocity;//current angular velocity
	comp308::vec3 force;//sum of all forces
	comp308::vec3 torque;//sum of all torques
	std::vector<comp308::vec3> mesh;
  void drawBoundingBox(bool);
  void findMax(){
    float min = -std::numeric_limits<float>::max();
    comp308::vec3 max(min,min,min);
    for(int i =0; i<mesh.size();i++){
      comp308::vec3 vert = mesh[i];
      if(vert.x > max.x){
	max.x = vert.x;
      }
      if(vert.y > max.y){
	max.y = vert.y;
      }
      if(vert.z > max.z){
	max.z = vert.z;
      }
    }
    boundary.m_vecMax = max;
  }
  void findMin(){
    float max = std::numeric_limits<float>::max();
    comp308::vec3 min(max,max,max);
    for(int i =0; i<mesh.size();i++){
      comp308::vec3 vert = mesh[i];
      if(vert.x < min.x){
	min.x = vert.x;
      }
      if(vert.y < min.y){
	min.y = vert.y;
      }
      if(vert.z < min.z){
	min.z = vert.z;
      }
    }
    boundary.m_vecMin = min;
  }

};
struct Contact{
  Rigidbody *a,*b;//a has vertex b has face
  comp308::vec3 p,//verex of contact
    n,//normal of face
    ea,//edge of A
    eb;//edge of B
  bool vf;//if vertex and face are touching
};
struct Collision{
  Rigidbody *a;
  Rigidbody *b;
};
class Physics{
public:
  void createRigidbody(comp308::vec3 position);
  void update(float);
  std::vector<comp308::vec3> rollBack(float);
  void checkCollisions(float delta);
  void initialiseCollisions();
  bool  AABBtoAABB(const TAABB& tBox1,const TAABB& tBox2);
  void addRigidbody(Rigidbody*);
  Rigidbody* getRigidbody(int position);
  void clear();
  void remove(Rigidbody*);
private:
	float currentTime;
	std::vector<Rigidbody*> objects;
};
