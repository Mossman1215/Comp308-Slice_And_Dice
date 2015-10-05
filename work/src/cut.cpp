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
float planeD;

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
	
	vector<vec3> frontVertices;
	vector<vec3> backVertices;

	//Separate the vertices.
	for (vec3 vertex : vertices) {
		if (isInFront(vertex) > 0) {
			frontVertices.push_back(vertex);
		}
		else {
			backVertices.push_back(vertex);
		}
	}

	//Calculate the intersection.
	if (frontVertices.size() > 0 && backVertices.size() > 0) {
		if (frontVertices.size() > backVertices.size()) {
			vertices = calculateIntersection(backVertices, frontVertices);
		}
		else {
			vertices = calculateIntersection(frontVertices, backVertices);
		}
	}

	//Add the new vertices to the old ones
	for (vec3 vertex : vertices) {
		frontVertices.push_back(vertex);
		backVertices.push_back(vertex);
	}

	//Actually cut it.
	cutTriangle(frontVertices, backVertices);
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
	planeD = d;
	return ((normal.x*vertex.x) + (normal.y*vertex.y) + (normal.z*vertex.z) + d);
}
/*
Calculates the displacement of the plane (the 'd' in ax + by + cz + d = 0).
*/
float cut::calculateDisplacement(vec3 normal) {
	float d = (normal.x*cutPlane[0].x*-1) - (normal.y*cutPlane[0].y) - (normal.z*cutPlane[0].z);
	return d;
}

/*
Given the two sets of vertices calculates the vectors from from set to the other.
Calculates the lines from these vectors and given vertices.
Calculates the intersection of those lines with the plane and finally
connects the two intersections to form the cut line.
*/
vector<vec3> cut::calculateIntersection(vector<vec3> v1, vector<vec3> v2) {
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

	//The vertices at the intersection point
	vec3 intersectVertex = getLine(v1[0], vector1Unit, t);
	vec3 intersectVertex2 = getLine(v1[0], vector2Unit, t2);

	vector<vec3> vertices;
	vertices.push_back(intersectVertex);
	vertices.push_back(intersectVertex2);

	return vertices;
}

void cut::cutTriangle(vector<vec3> frontVertices, vector<vec3> backVertices) {
	vector<vec3> quad;
	vector<vec3> triangle;

	if (frontVertices.size() > backVertices.size()) {
		quad = frontVertices;
		triangle = backVertices;
	}
	else {
		quad = backVertices;
		triangle = frontVertices;
	}

	//The centroid of the triangle.
	GLfloat centerX = (triangle[0].x + triangle[1].x + triangle[2].x) / 3;
	GLfloat centerY = (triangle[0].y + triangle[1].y + triangle[2].y) / 3;
	GLfloat centerZ = (triangle[0].z + triangle[1].z + triangle[2].z) / 3;

	vec3 centroidTri(centerX, centerY, centerZ);

	//Convert quad to triangles
	vector<vector<vec3>> quadTriangles = quadToTriangle(quad);

	vector<vector<vec3>> triangles;
	triangles.push_back(triangle);
	triangles.push_back(quadTriangles[0]);
	triangles.push_back(quadTriangles[1]);

	//The shortest distance between the triangle centroid and the cutPlane.
	float distance = normal.x*centroidTri.x + normal.y*centroidTri.y + normal.z*centroidTri.z + planeD / (pow((pow(normal.x, 2), pow(normal.y, 2), pow(normal.z, 2)), 0.5));

	//If centroidTri lies on same side as the normal.
	if (distance > 0) {
		//Draw the new triangles.
		draw(triangles, 1);
	}
	else if (distance < 0) {
		draw(triangles, -1);
	}
	else {
		//Draw the original (hasn't been cut yet).
		draw(triangles, 0);
	}
}
/*
 Converts the given quad into two triangles.
 */
vector<vector<vec3>> cut::quadToTriangle(vector<vec3> vertices) {
	vector<vec3> triangle1;
	vector<vec3> triangle2;
	for (int i = 0; i < vertices.size(); i++) {
		if (i == 0) {
			triangle1.push_back(vertices[i]);
			triangle2.push_back(vertices[i]);
		}
		else if (i == 1) {
			triangle2.push_back(vertices[i]);
		}
		else if (i == 2) {
			triangle1.push_back(vertices[i]);
		}
		else if (i == 3) {
			triangle1.push_back(vertices[i]);
			triangle2.push_back(vertices[i]);
		}
	}

	vector<vector<vec3>> triangles;
	triangles.push_back(triangle1);
	triangles.push_back(triangle2);

	return triangles;
}

/*
Draws the newly cut triangle.
triangle translated by normal * direction.
Quad triangles translated by normal * -direction.
*/
void cut::draw(vector<vector<vec3>> triangles, int direction) {
	vec3 translateDirection = normal * direction;

	//Translation magnitude
	double normalMagntde = pow((pow(translateDirection.x, 2) + pow(translateDirection.y, 2) + pow(translateDirection.z, 2)), 0.5);

	//Translation direction
	vec3 translateUnit = translateDirection * (1 / normalMagntde);

	
	glColor3f(0, 1, 1);
	for (int i = 0; i < triangles.size(); i++) {
		glPushMatrix();
		if (i == 0 && direction != 0) {
			glTranslatef(translateUnit.x, translateUnit.y, translateUnit.z);
		}
		else if(direction != 0) {
			glTranslatef(translateUnit.x * -1, translateUnit.y * -1, translateUnit.z * -1);
		}
		glBegin(GL_TRIANGLES);
		glNormal3f(0.0, 0.0, 1.0);
		for (vec3 vertex : triangles[i]) {
			glVertex3f(vertex.x, vertex.y, vertex.z);
		}
		glEnd();
		glPopMatrix();
	}
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
	float t = dot((cutPlane[0] - position), normal) / (dot(direction, normal));
	return t;
}

/*
TODO

Make new geometry everytime a cut a made.

*/