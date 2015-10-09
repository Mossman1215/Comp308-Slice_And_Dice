#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>

#include "comp308.hpp"
#include "cut.hpp"
#include "geometry.hpp"

using namespace std;
using namespace comp308;

vector<vec3> cutPlane;
vec3 normal;
float planeD;

cut::cut() {}

/*
For every geometry currently in the world, cut that geometry if it intersects with the plane
and return new geometry resulting from the cut.
*/
vector<geometry> cut::createCut(vector<vec3> plane, vector<geometry> geometrys) {
	cutPlane = plane;
	vec3 normal = findNormal();
	planeD = calculateDisplacement(normal);

	vector<geometry> allGeometry;
	for (geometry g_geometry : geometrys) {
		vector<geometry> newGeometrys;
		newGeometrys = cutGeometry(g_geometry);
		for (geometry newGeometry : newGeometrys) {
			allGeometry.push_back(newGeometry);
		}
	}

	cout << "Currently rendering ";
	cout << allGeometry.size();
	cout << " pieces of geometry" << endl;

	return allGeometry;
}

/*
For the given geometry and for every triangle within that geometry, if it intersects with the plane; cut the triangle and
add the resulting triangles to either one of two geometrys (left of plane or right of plane).
Return those geometrys.
*/
vector<geometry> cut::cutGeometry(geometry g_geometry) {
	geometry geometry1 = geometry();
	geometry geometry2  = geometry();

	for (vector<vec3> triangle : g_geometry.getTriangles()) {
		vector<vec3> vertices = triangle;

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

			//Add the new vertices to the old ones
			for (vec3 vertex : vertices) {
				frontVertices.push_back(vertex);
				backVertices.push_back(vertex);
			}

			vector<vector<vec3>> newTriangles;

			//Actually cut it.
			newTriangles = cutTriangle(frontVertices, backVertices);

			vector<vector<vec3>> triangles1;
			triangles1.push_back(newTriangles[0]);
			vector<vector<vec3>> triangles2;
			triangles2.push_back(newTriangles[1]);
			triangles2.push_back(newTriangles[2]);

			vec3 tri1Centroid = getCentroid(newTriangles[0]);
			vec3 tri2Centroid = getCentroid(newTriangles[1]);

			//Add the new triangles to the corresponding geometry.
			if (isInFront(tri1Centroid) > 0) {
				geometry1.addToTriangles(newTriangles[0]);
			}
			else {
				geometry2.addToTriangles(newTriangles[0]);
			}
			if (isInFront(tri2Centroid) > 0) {
				geometry1.addToTriangles(newTriangles[1]);
				geometry1.addToTriangles(newTriangles[2]);
			}
			else {
				geometry2.addToTriangles(newTriangles[1]);
				geometry2.addToTriangles(newTriangles[2]);
			}
		}

	}

	//If plane didn't intersect this geometry then one of the geometry's will be empty. So discard it.
	vector<geometry> bothGeometrys;

	if (geometry1.getTriangles().size() > 0) {
		bothGeometrys.push_back(geometry1);
	}

	if (geometry2.getTriangles().size() > 0) {
		bothGeometrys.push_back(geometry2);
	}

	if (geometry1.getTriangles().size() == 0 && geometry2.getTriangles().size() == 0) {
		bothGeometrys.push_back(g_geometry);
	}


	return bothGeometrys;
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
	return ((normal.x*vertex.x) + (normal.y*vertex.y) + (normal.z*vertex.z) + planeD);
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

/*
Given a set of vertices that make up a triangle and a quad. Cut the triangle that is made up of
these shapes.
*/
vector<vector<vec3>> cut::cutTriangle(vector<vec3> frontVertices, vector<vec3> backVertices) {
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

	vec3 centroidTri = getCentroid(triangle);

	//Convert quad to triangles
	vector<vector<vec3>> quadTriangles = quadToTriangle(quad);

	vector<vector<vec3>> triangles;
	triangles.push_back(triangle);
	triangles.push_back(quadTriangles[0]);
	triangles.push_back(quadTriangles[1]);

	//The shortest distance between the triangle centroid and the cutPlane.
	float distance = dot(normal, (centroidTri - cutPlane[0]));

	vector<vector<vec3>> newTriangles;

	//If centroidTri lies on same side as the normal.
	if (distance > 0) {
		//Draw the new triangles.
		newTriangles = separateTriangle(triangles, 1);
	}
	else if (distance < 0) {
		newTriangles = separateTriangle(triangles, -1);
	}
	else {
		//Draw the original (hasn't been cut yet).
		newTriangles = separateTriangle(triangles, 0);
	}

	return newTriangles;
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
Separates a single triangle making up the mesh of the geometry
triangle translated by normal * direction.
Quad triangles translated by normal * -direction.
*/
vector<vector<vec3>> cut::separateTriangle(vector<vector<vec3>> triangles, int direction) {
	vector<vector<vec3>> newTriangles = triangles;

	vec3 translateDirection = normal * direction;

	//Translation magnitude
	double normalMagntde = pow((pow(translateDirection.x, 2) + pow(translateDirection.y, 2) + pow(translateDirection.z, 2)), 0.5);

	//Translation direction
	vec3 translateUnit = translateDirection * (1 / normalMagntde);

	for (int i = 0; i < newTriangles.size(); i++) {
		for (vec3 &vertex : newTriangles[i]) {
			if (i == 0 && direction != 0) {
				vertex.x = vertex.x + translateUnit.x;
				vertex.y = vertex.y + translateUnit.y;
				vertex.z = vertex.z + translateUnit.z;
			}
			else if (direction != 0) {
				vertex.x = vertex.x + translateUnit.x * -1;
				vertex.y = vertex.y + translateUnit.y * -1;
				vertex.z = vertex.z + translateUnit.z * -1;
			}
		}
	}

	return newTriangles;
}

/*
Helper method for generating a line given a point, direction and length.
*/
vec3 cut::getLine(vec3 position, vec3 direction, float length) {
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
Helper method for finding the centroid of a shape.
*/
vec3 cut::getCentroid(vector<vec3> shape) {
	GLfloat centerX = (shape[0].x + shape[1].x + shape[2].x) / 3;
	GLfloat centerY = (shape[0].y + shape[1].y + shape[2].y) / 3;
	GLfloat centerZ = (shape[0].z + shape[1].z + shape[2].z) / 3;

	vec3 centroid(centerX, centerY, centerZ);

	return centroid;
}