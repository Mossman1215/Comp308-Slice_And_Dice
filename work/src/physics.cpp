#include "physics.hpp"
#include "comp308.hpp"
using namespace comp308;
using namespace std;
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
		for(int i =0;i< mesh.size();i++){
		  mesh[i]+=position;
		}
	}else {
		addForce(vec3(0,-force.y,0));
	}
	findMax();
	findMin();
	return position;
}
void Physics::checkCollisions(float delta){
	//check every object with the other objects in the scene and if their bounding boxes collide add them to resolve list
	//for object:objects
  //update vertex positions
  vector<Collision> collisions;
  for(int i =0 ;i<objects.size();i++){
    Rigidbody rb1 = objects[i];
    for(int j = 0; j <objects.size();j++){
      Rigidbody rb2 = objects[j];
      if(j!=i){
           if(AABBtoAABB(rb1.boundary,rb2.boundary)){
	     Collision c;
	     c.a = &rb1;
	     c.b = &rb2;
	     collisions.push_back(c);
	     cout << "collision detected" << endl;
           }
      }  
    }
  }
  //for each object in collisions list
  //check all vertex & edge & face for separating plane if they fail bounding box check
  //check that vertex or edge ,delta + tolerance, changes the separating plane, meaning that they are no longer disjoint?

  
}
void Physics::initialiseCollisions(){
  //generate intial witnesses (separating planes)
}
bool Physics::AABBtoAABB(const TAABB& tBox1, const TAABB& tBox2)
{
  cout <<"tBox1: " << tBox1.m_vecMin <<","<< tBox1.m_vecMax;
  cout <<"tBox2: " << tBox2.m_vecMin <<","<< tBox2.m_vecMax;
  //Check if Box1's max is greater than Box2's min and Box1's min is less than Box2's max
  return(tBox1.m_vecMax.x > tBox2.m_vecMin.x &&
    tBox1.m_vecMin.x < tBox2.m_vecMax.x &&
	 tBox1.m_vecMax.y > tBox2.m_vecMin.y &&
    tBox1.m_vecMin.y < tBox2.m_vecMax.y &&
	 tBox1.m_vecMax.z > tBox2.m_vecMin.z &&
	 tBox1.m_vecMin.z < tBox2.m_vecMax.z);
  //If not, it will return false
}
void Physics::addRigidbody(Rigidbody r){
  objects.push_back(r);
}
