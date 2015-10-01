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
From here we have two points in space and we just need to draw a line between them. This is our intersection.

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
vec3 normal;
float planeDisplacement;

cut::cut() {}

void cut::createCut(vector<vec3> plane) {
	/*
	Currently hard coding in the vertices will have to calculate these on the fly in the finished product.
	These are the vertices of the current triangle I'm iterating over.
	*/
	vector<vec3> vertices;
	vec3 v1(-5.0, -5.0, 5.0);
	vec3 v2(-5.0, 5.0, -2.5);
	vec3 v3(5.0, 5.0, -5.0);
	vertices.push_back(v1);
	vertices.push_back(v2);
	vertices.push_back(v3);
	cutPlane = plane;
	
	vector<vec3> polygons1;
	vector<vec3> polygons2;

	//Separate the vertices.
	for (vec3 vertex : vertices) {
		glPushMatrix();
		glTranslatef(vertex.x, vertex.y, vertex.z);
		if (isInFront(vertex) > 0) {
			glColor3f(1, 0, 0);
			polygons1.push_back(vertex);
		}
		else {
			glColor3f(0, 1, 0);
			polygons2.push_back(vertex);
		}
		glutSolidSphere(0.5, 100, 100);
		glPopMatrix();
	}
	
	//Calculate the intersection.
	if (polygons1.size() > 0 && polygons2.size() > 0) {
		if (polygons1.size() > polygons2.size()) {
			calculateIntersection(polygons2, polygons1);
		}
		else {
			calculateIntersection(polygons1, polygons2);
		}
	}
}

/*
Calculates the normal of the plane.
*/
vec3 cut::findNormal() {
	vec3 edge1 = cutPlane[0] - cutPlane[1];
	vec3 edge2 = cutPlane[2] - cutPlane[1];

	vec3 n = cross(edge1, edge2);
	normal = n;
	return n;
}

/*
Returns whether or not the given point is in front or behind the plane.
*/
int cut::isInFront(vec3 vertex) {
	vec3 normal = findNormal();
	float d = calculateDisplacement(normal);
	return ((normal.x*vertex.x) + (normal.y*vertex.y) + (normal.z*vertex.z) + d);
}
/*
Calculates the displacement of the plane (the 'd' in ax + by + cz + d = 0).
*/
float cut::calculateDisplacement(vec3 normal) {
	float d = (normal.x*cutPlane[0].x*-1) - (normal.y*cutPlane[0].y) - (normal.z*cutPlane[0].z);
	planeDisplacement = d;
	return d;
}

/*
Given the two sets of vertices calculates the vectors from from set to the other.
Calculates the lines from these vectors and given vertices.
Calculates the intersection of those lines with the plane and finally
connects the two intersections to form the cut vector.
*/
void cut::calculateIntersection(vector<vec3> v1, vector<vec3> v2) {
	//The two vectors
	vec3 vector1 = v2[0] - v1[0];
	vec3 vector2 = v2[1] - v1[0];

	//The magnitude of these vectors
	double vector1Magntde = pow((pow(vector1.x, 2) + pow(vector1.y, 2) + pow(vector1.z, 2)), 0.5);
	double vector2Magntde = pow((pow(vector2.x, 2) + pow(vector2.y, 2) + pow(vector2.z, 2)), 0.5);

	//The unit vectors
	vec3 vector1Unit = vector1 * (1 / vector1Magntde);
	vec3 vector2Unit = vector2 * (1 / vector2Magntde);

	//The two lines
	vec3 line1 = getLine(v1[0], vector1Unit, 0);
	vec3 line2 = getLine(v1[0], vector2Unit, 0);

	//The scaler t for the intersection.
	float t = getLineDisplacement(v1[0], vector1Unit);
	float t2 = getLineDisplacement(v1[0], vector2Unit);

	//Substituting t in for the equation of the line.
	vec3 line3 = getLine(v1[0], vector1Unit, t);
	vec3 line4 = getLine(v1[0], vector2Unit, t2);

	glColor3f(1, 0, 0);
	glLineWidth(2);
	glBegin(GL_LINES);
	glVertex3f(line1.x, line1.y, line1.z);
	glVertex3f(line3.x, line3.y, line3.z);
	glVertex3f(line2.x, line2.y, line2.z);
	glVertex3f(line4.x, line4.y, line4.z);
	glEnd();
}

/*
Helper method for generating a line given a point, direction and length.
*/
vec3 cut::getLine(vec3 position, vec3 direction, int length) {
	vec3 line(position.x + (direction.x * length), position.y + (direction.y * length), position.z + (direction.z * length));
	return line;
}

/*
Helper method for determining how far along the given line the intersection is.
*/
float cut::getLineDisplacement(vec3 position, vec3 direction) {
	cout << "Plane d: ";
	cout << planeDisplacement << endl;
	float t = (planeDisplacement - (normal.x*position.x) - (normal.y*position.y) - (normal.z*position.z)) / ((normal.x*direction.x) + (normal.y*direction.y) + (normal.z*direction.z));
	cout << "t: ";
	cout << t << endl;
	return t;
}