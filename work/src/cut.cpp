/*
Algorithm so far:

If you can find the interseciton of a line and a plane in space, then the algorithm is as follows:

Find the vertices of the triangle you are cutting, of the 3 vertices 
2 will be in front on the plane and one will be behind the plane or vice versa.
Find the equations of the lines representing the two edges that go from the one point on one side
to the two points on the other side.

From here, the simplest way to find the intersection of the polygon and the plane
is to find the intersection of the two edges with the plane.
The two calculated points from the intersection lie on the intersection line between the polygon and the plane.
From here we have two points in space and we just need to draw a line between them. This is out intersection.

Do this for every polygon. If all 3 points lie on the same side of the plane then the ploygon does not
intersect with the plane and we shouldn't run the algorithm.
*/
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>

#include "comp308.hpp"
#include "cut.hpp"

using namespace std;
using namespace comp308;

vector<vec3> cutPlane;

cut::cut() {}

void cut::createCut(vector<vec3> plane) {
	/*
	Currently hard coding in the vertices will have to calculate these on the fly in the finished product.
	These are the vertices of the current triangle I'm iterating over.
	*/
	vector<vec3> vertices;
	vec3 v1(-5.0, -5.0, 0.0);
	vec3 v2(-5.0, 5.0, 0.0);
	vec3 v3(5.0, 5.0, 0.0);
	vertices.push_back(v1);
	vertices.push_back(v2);
	vertices.push_back(v3);
	cutPlane = plane;
	
	for (vec3 vertex : vertices) {
		glPushMatrix();
		glTranslatef(vertex.x, vertex.y, vertex.z);
		if (isInFront(vertex) > 0) {
			glColor3f(1, 0, 0);
		}
		else {
			glColor3f(0, 1, 0);
		}
		glutSolidSphere(0.5, 100, 100);
		glPopMatrix();
	}
}

/*
Need to calculate the normal of the plane in order to find the equation of it. Normal is done through the cross product.
*/
vec3 cut::findNormal() {
	//glLineWidth(2);
	//glColor3f(1, 0, 0);
	//glBegin(GL_LINES);
	//glVertex3f(cutPlane[1].x, cutPlane[1].y, cutPlane[1].z);
	//glVertex3f(cutPlane[2].x, cutPlane[2].y, cutPlane[2].z);
	//glEnd();

	vec3 edge1 = cutPlane[0] - cutPlane[1];
	vec3 edge2 = cutPlane[2] - cutPlane[1];

	vec3 n = cross(edge1, edge2);
//	vec3 n2 = n * 5;

	//glLineWidth(2);
	//glColor3f(1, 0, 0);
	//glBegin(GL_LINES);
	//glVertex3f(n.x, n.y, n.z);
	//glVertex3f(n2.x, n2.y, n2.z);
	//glEnd();

	return n;
}

/*
Need to find whether the given vertex is behind or in front of plane.

equation of plane is: Ax+By+Cz+D=0 plug in a point not lying on the plane for x,y and z and the result should 
be positive or negative respective of what side it lies on. Hopefully...
*/
int cut::isInFront(vec3 vertex) {
	vec3 normal = findNormal();
	return ((normal.x*vertex.x) + (normal.y*vertex.y) + (normal.z*vertex.z));
}

void cut::draw() {
	findNormal();
}