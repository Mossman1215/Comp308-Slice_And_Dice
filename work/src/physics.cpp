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
	addForce(vec3(0,-9.81*delta,0));
	vec3 acceleration = force/mass;
	//change position
	if(position.y + (acceleration.y * delta) > 0){
		position = position + (acceleration*delta);
		for(int i =0;i< mesh.size();i++){
		  mesh[i]+=position;
		}
	}else {
		addForce(vec3(0,-force.y,0));
		position = position + ((force/mass)*delta);
		force = force*0.9;
	}
	force = force*0.99;
	boundary.position = position;
	return position;
}
void Physics::checkCollisions(float delta){
	//check every object with the other objects in the scene and if their bounding boxes collide add them to resolve list
	//for object:objects
  //update vertex positions
  vector<Collision> collisions;
  for(int i =0 ;i<objects.size();i++){
    Rigidbody* rb1 = objects[i];
    for(int j = 0; j <objects.size();j++){
      Rigidbody* rb2 = objects[j];
      if(j!=i){
          if(AABBtoAABB(rb1->boundary,rb2->boundary)){
	     Collision c;
	     c.a = rb1;
	     c.b = rb2;
	     collisions.push_back(c);
	     cout << "collision detected rb1.pos:" << rb1->boundary.position << " rb2.pos "<< rb2->boundary.position<< endl;
           }
      }  
    }
  }
  //resolve collisions
  for(unsigned int k=0;k<collisions.size();k++){
	Collision c =  collisions[k];
	vec3 forceA;
	forceA = c.a->position - c.b->position;
	cout <<"forceA"<<forceA<< endl;
	forceA = normalize(forceA);
	float r = length(forceA);
	c.a->addForce(forceA/(r*r));
	vec3 forceB;
	forceB = c.b->position - c.a->position;
	forceB = normalize(forceB);
	c.b->addForce(forceB/(r*r));
	cout <<"forceB"<<forceB<< endl;
	
  }
  collisions.clear();
  //for each object in collisions list
  //check all vertex & edge & face for separating plane if they fail bounding box check
  //check that vertex or edge ,delta + tolerance, changes the separating plane, meaning that they are no longer disjoint?

  
}
void Physics::initialiseCollisions(){
  //generate intial witnesses (separating planes)
}
bool Physics::AABBtoAABB(const TAABB& tBox1,const TAABB& tBox2){
	float x1 = tBox1.position.x;
	float y1 = tBox1.position.y;
	float z1 = tBox1.position.z;
	float x2 = tBox2.position.x;
	float y2 = tBox2.position.y;
	float z2 = tBox2.position.z;
	return(x1+tBox1.m_vecMax.x > x2+tBox2.m_vecMin.x &&
	           x1+tBox1.m_vecMin.x < x2+tBox2.m_vecMax.x &&
	       y1+tBox1.m_vecMax.y > y2+tBox2.m_vecMin.y &&
	           y1+tBox1.m_vecMin.y < y2+tBox2.m_vecMax.y &&
	       z1+tBox1.m_vecMax.z > z2+tBox2.m_vecMin.z &&
	       z1+tBox1.m_vecMin.z < z2+tBox2.m_vecMax.z);
}

void Physics::addRigidbody(Rigidbody* r){
  objects.push_back(r);
}
