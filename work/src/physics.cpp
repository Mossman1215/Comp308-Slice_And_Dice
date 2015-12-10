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

comp308::vec3 Rigidbody::update(float delta, bool bounding){
	//apply forces
	this->addForce(vec3(0,-9.81*delta,0));
	vec3 acceleration = force/mass;
	//change position
	if(position.y +boundary.m_vecMin.y + (acceleration.y * delta) < 0){
	 	this->addForce(vec3(0,-force.y,0));
		grounded = true;
		position = position + ((force/mass)*delta);
		position.y = -boundary.m_vecMin.y;
		force.x = force.x*0.9;
		force.z = force.z*0.9;
	}
	  if(position.x+boundary.m_vecMax.x>10){
	    this->addForce(vec3(-force.x,0,0));
	    position.x= 10-boundary.m_vecMax.x;
	  }
	  if(position.x-boundary.m_vecMin.x<-10){
	    this->addForce(vec3(-force.x,0,0));
	    position.x = -10+boundary.m_vecMin.x;
	  }
	  if(position.y+boundary.m_vecMax.y>10){
	    this->addForce(vec3(0,-10,0));
	      position.y = 10-boundary.m_vecMax.y;
	  }
  	  if(position.z+boundary.m_vecMax.z>10){
	    this->addForce(vec3(0,0,-force.z));
	    position.z = 10-boundary.m_vecMax.z;
	  }
	  if(position.z-boundary.m_vecMin.z<-10){
	    this->addForce(vec3(0,0,-force.z));
	    position.z = -10 + boundary.m_vecMin.z;
	  }
         
	  position = position + (acceleration*delta);
	  
	force = force*0.99;
	boundary.position = position;
	drawBoundingBox(bounding);
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
	     
           }
      }  
    }
  }
  //resolve collision
  //for each object in collisions list
  //check all vertex & edge & face for separating plane if they fail bounding box check
  //check that vertex or edge ,delta + tolerance, changes the separating plane, meaning that they are no longer disjoint?
  for(unsigned int k=0;k<collisions.size();k++){
	Collision c =  collisions[k];
	vec3 forceA;
	forceA = c.a->position - c.b->position;
	float r = length(forceA);
	if (r>.01||r<-0.1) {
		r = 1;
	}
	if( !c.a->grounded){
	c.a->addForce(forceA*delta);
	}else{
	  c.a->addForce(vec3(forceA.x,0,forceA.z));
	}
	vec3 forceB;
	forceB = c.b->position - c.a->position;
	if(!c.b->grounded){
	 c.b->addForce(forceB*delta);
	}else{
	  c.b->addForce(vec3(forceB.x,0,forceB.z));
	}
  }
  collisions.clear();

  
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
Rigidbody* Physics::getRigidbody(int position){
  return objects[position];
}
void Physics::remove(Rigidbody* rb){
  unsigned int count =0;
  while(count< objects.size()){
    Rigidbody* array =objects[count];
    if(array==rb){
      break;
    }
    count++;
  }
  delete rb;
  objects.erase(objects.begin()+count);
  
}
void Physics::update(float delta, bool bounds){
  for(Rigidbody* rb:objects){
    rb->update(delta, bounds);
  }
}
void Physics::clear(){
  objects.clear();
}
void Rigidbody::drawBoundingBox(bool visible){
  glPushMatrix();
  glTranslatef(boundary.position.x,boundary.position.y,boundary.position.z);
  if(visible){
	glLineWidth(2);
	glColor3f(0,1,0);
	 glBegin(GL_LINE_LOOP);
	 //min z to max z
	  glVertex3f(boundary.m_vecMin.x,boundary.m_vecMin.y,boundary.m_vecMin.z);
	  glVertex3f(boundary.m_vecMin.x,boundary.m_vecMin.y,boundary.m_vecMax.z);
	  //min y to max y 
	  glVertex3f(boundary.m_vecMin.x,boundary.m_vecMax.y,boundary.m_vecMax.z);
	  //max y min z
	  glVertex3f(boundary.m_vecMin.x,boundary.m_vecMax.y,boundary.m_vecMin.z);
	 glEnd();
	 glBegin(GL_LINES);
	  glVertex3f(boundary.m_vecMin.x,boundary.m_vecMin.y,boundary.m_vecMin.z);
	  glVertex3f(boundary.m_vecMax.x,boundary.m_vecMin.y,boundary.m_vecMin.z);
	  glVertex3f(boundary.m_vecMin.x,boundary.m_vecMin.y,boundary.m_vecMax.z);
	  glVertex3f(boundary.m_vecMax.x,boundary.m_vecMin.y,boundary.m_vecMax.z);
	  glVertex3f(boundary.m_vecMin.x,boundary.m_vecMax.y,boundary.m_vecMax.z);
	  glVertex3f(boundary.m_vecMax.x,boundary.m_vecMax.y,boundary.m_vecMax.z);
	  glVertex3f(boundary.m_vecMin.x,boundary.m_vecMax.y,boundary.m_vecMin.z);
	  glVertex3f(boundary.m_vecMax.x,boundary.m_vecMax.y,boundary.m_vecMin.z);
	 glEnd();
	 glBegin(GL_LINE_LOOP);
	 //min z to max z
	  glVertex3f(boundary.m_vecMax.x,boundary.m_vecMin.y,boundary.m_vecMin.z);//vecMin.xyz
	  glVertex3f(boundary.m_vecMax.x,boundary.m_vecMin.y,boundary.m_vecMax.z);//vecMin.xy vecMax.z
	  //min y to max y 
	  glVertex3f(boundary.m_vecMax.x,boundary.m_vecMax.y,boundary.m_vecMax.z);
	  //max y min z
	  glVertex3f(boundary.m_vecMax.x,boundary.m_vecMax.y,boundary.m_vecMin.z);	 
	 glEnd();
  }
  glPopMatrix();
}

int Physics::count(){
  return (int) objects.size();
}
