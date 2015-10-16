#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>

#include "comp308.hpp"
#include "cut.hpp"
#include "geometry.hpp"
#include "physics.hpp"

using namespace std;
using namespace comp308;

vector<vec3> cutPlane;
vector<vec3> originalPlane;
vec3 normal;
float planeD;

/*
Mapping geometry to rigidbody:

Each geometry holds a rigidbody, everytime a geometry is cut
use the rigidbody of the parent geometry to calculate the rigidbody's
of the child geometry's. Assign the child geometry's the new rigidbody's and pass
the new geometry's off to g_geometry.

Rigidbody rigid = Rigidbody(vec3(0,0,0),g_sphere.getPoints(),1);
*/

cut::cut() {}

/*
For every geometry currently in the world, cut that geometry if it intersects with the plane
and return new geometry resulting from the cut.
*/
vector<geometry> cut::createCut(vector<vec3> plane, vector<geometry> geometrys, Physics *p) {
	cutPlane = plane;
	originalPlane = plane;
	int triCount = 0;
	vector<geometry> allGeometry;
	for (geometry g_geometry : geometrys) {
		vector<geometry> newGeometrys;
		newGeometrys = cutGeometry(g_geometry, p);
		for (geometry newGeometry : newGeometrys) {
			allGeometry.push_back(newGeometry);
			triCount += newGeometry.getTriangles().size();
		}
	}

	cout << "Currently rendering ";
	cout << triCount;
	cout << " triangles" << endl;

	return allGeometry;
}

/*
For the given geometry and for every triangle within that geometry, if it intersects with the plane; cut the triangle and
add the resulting triangles to either one of two geometrys (left of plane or right of plane).
Return those geometrys.
*/
vector<geometry> cut::cutGeometry(geometry g_geometry, Physics *p) {
	geometry geometry1 = geometry();
	geometry geometry2 = geometry();
	int intersects = 0;
	vector<triangle> tempTriangles;
	vector<vertex> cutVertices;
	Rigidbody* parent = g_geometry.getRigidbody();

	//Translate plane back to vertex data
	for (int i = 0; i < 3; i++) {
		cutPlane[i].x = cutPlane[i].x - parent->position.x;
		cutPlane[i].y = cutPlane[i].y - parent->position.y;
		cutPlane[i].z = cutPlane[i].z - parent->position.z;
	}

	vec3 normal = findNormal();
	planeD = calculateDisplacement(normal);

	for (triangle t : g_geometry.getTriangles()) {
		vector<vertex> intersectVertices;

		vector<vertex> frontVertices;
		vector<vertex> backVertices;

		//Separate the vertices.
		for (vertex v : t.v) {
			vec3 point = v.p;
			if (isInFront(point) > 0) {
				frontVertices.push_back(v);
			}
			else {
				backVertices.push_back(v);
			}
		}

		//Calculate the intersection.
		if (frontVertices.size() > 0 && backVertices.size() > 0) {	//If triangle intersects with the plane.
			intersects = 1;
			if (frontVertices.size() > backVertices.size()) {
				intersectVertices = calculateIntersection(backVertices, frontVertices);
			}
			else {
				intersectVertices = calculateIntersection(frontVertices, backVertices);
			}

			//Add the new vertices to the old ones
			for (vertex vertex : intersectVertices) {
				frontVertices.push_back(vertex);
				backVertices.push_back(vertex);
				cutVertices.push_back(vertex);
			}

			vector<triangle> newTriangles;

			//Actually cut it.
			newTriangles = cutTriangle(frontVertices, backVertices);

			vec3 tri1Centroid = getCentroid(newTriangles[0]);	//Centroid of the triangle
			vec3 tri2Centroid = getCentroid(newTriangles[1]);	//Centroid of one of the triangles within the quad

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
		else {
			tempTriangles.push_back(t);	//Store all the triangles within the geometry that haven't been cut so that we can
										//translate them later if the geometry was cut.
		}
	}

	//Translate the uncut triangles of the geometry
	for (triangle t : tempTriangles) {
		if (intersects == 1) {
			vec3 centroid = getCentroid(t);
			float distance = dot(normal, (centroid - cutPlane[0]));
			triangle newTriangle;
			if (distance > 0) {
				newTriangle = separateTriangle(t, 1);
			}
			else {
				newTriangle = separateTriangle(t, -1);
			}
			if (isInFront(centroid) > 0) {
				geometry1.addToTriangles(newTriangle);
			}
			else {
				geometry2.addToTriangles(newTriangle);
			}
		}
		else {
			if (isInFront(getCentroid(t)) > 0) {
				geometry1.addToTriangles(t);
			}
			else {
				geometry2.addToTriangles(t);
			}
		}
	}

	//If geometry was cut
	if (cutVertices.size() > 0){
		//Create a new mesh for the cut area
		vertex centroidPoly = getCentre(cutVertices);
		vector<triangle> mesh = getMesh(cutVertices, centroidPoly);
		vector<triangle> mesh2 = getMesh(cutVertices, centroidPoly);

		//Split the mesh into two
		for (triangle t : mesh) {
			triangle newTriangle;
			newTriangle = separateTriangle(t, 1);
			geometry1.addToTriangles(newTriangle);
		}

		for (triangle t : mesh2) {
			triangle newTriangle;
			newTriangle = separateTriangle(t, -1);
			geometry2.addToTriangles(newTriangle);
		}
	}

	//If plane didn't intersect this geometry then one of the geometry's will be empty. So discard it.
	vector<geometry> bothGeometrys;

	if(geometry1.getTriangles().size() == 0){	//If this geometry is empty, add parent rigidbody to geometry2
		geometry2.setRigidbody(parent);
		bothGeometrys.push_back(geometry2);
	}
	else if (geometry2.getTriangles().size() == 0) {	//If this geometry is empty, add parent rigidbody to geometry1
		geometry1.setRigidbody(parent);
		bothGeometrys.push_back(geometry1);
	}
	else {	//Else add new rigidbody to both as this geometry has been cut
		cout << "Rigidbody position is: " << parent->position << endl;
		cout << "Parent Mesh position is: " << getGeometryCentre(g_geometry.getPoints()) << endl;
		vec3 rigidBase = getGeometryCentre(geometry1.getPoints()) / 200;
		cout << "Geometry1 Mesh position is: " << rigidBase << endl;
		Rigidbody *child = new Rigidbody(rigidBase + parent->position, geometry1.getPoints(), 1, geometry1.getPoints().size(), parent->force);
		p->addRigidbody(child);
		geometry1.setRigidbody(child);
		bothGeometrys.push_back(geometry1);

		rigidBase = getGeometryCentre(geometry2.getPoints()) / 200;
		cout << "Geometry2 Mesh position is: " << rigidBase << endl;
		Rigidbody *child2 = new Rigidbody(rigidBase + parent->position, geometry2.getPoints(), 1, geometry2.getPoints().size(), parent->force);
		p->addRigidbody(child2);
		geometry2.setRigidbody(child2);
		bothGeometrys.push_back(geometry2);
		p->remove(g_geometry.getRigidbody());
	}

	cutPlane = originalPlane;
	cout << "cutting complete" << endl;
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
vector<vertex> cut::calculateIntersection(vector<vertex> v1, vector<vertex> v2) {
	//The two vectors
	vec3 vector1 = v2[0].p - v1[0].p;
	vec3 vector2 = v2[1].p - v1[0].p;

	//The magnitude of these vectors
	double vector1Magntde = pow((pow(vector1.x, 2) + pow(vector1.y, 2) + pow(vector1.z, 2)), 0.5);
	double vector2Magntde = pow((pow(vector2.x, 2) + pow(vector2.y, 2) + pow(vector2.z, 2)), 0.5);

	//The unit vectors
	vec3 vector1Unit = vector1 * (1 / vector1Magntde);
	vec3 vector2Unit = vector2 * (1 / vector2Magntde);

	//The scaler t for the intersection
	float t = getLineDisplacement(v1[0].p, vector1Unit);
	float t2 = getLineDisplacement(v1[0].p, vector2Unit);

	//The new vertices at the intersection point
	vertex intersectVertex1 = findVertex(t / vector1Magntde, v1[0], v2[0]);
	vertex intersectVertex2 = findVertex(t2 / vector2Magntde, v1[0], v2[1]);

	vector<vertex> vertices;
	vertices.push_back(intersectVertex1);
	vertices.push_back(intersectVertex2);

	return vertices;
}

/*
Given a set of vertices that make up a triangle and a quad. Cut the triangle that is made up of
these shapes.
*/
vector<triangle> cut::cutTriangle(vector<vertex> frontVertices, vector<vertex> backVertices) {
	vector<vertex> quad;
	triangle tri;

	if (frontVertices.size() > backVertices.size()) {
		quad = frontVertices;
		tri.v[0] = backVertices[0];
		tri.v[1] = backVertices[1];
		tri.v[2] = backVertices[2];
	}
	else {
		quad = backVertices;
		tri.v[0] = frontVertices[0];
		tri.v[1] = frontVertices[1];
		tri.v[2] = frontVertices[2];
	}

	vec3 centroidTri = getCentroid(tri);

	//Convert quad to triangles
	vector<triangle> quadTriangles = quadToTriangle(quad);

	vector<triangle> triangles;
	triangles.push_back(tri);
	triangles.push_back(quadTriangles[0]);
	triangles.push_back(quadTriangles[1]);

	//The shortest distance between the triangle centroid and the cutPlane.
	float distance = dot(normal, (centroidTri - cutPlane[0]));

	vector<triangle> newTriangles;

	//If centroidTri lies on same side as the normal.
	if (distance > 0) {
		//Draw the new triangles.
		newTriangles = separateTriangles(triangles, 1);
	}
	else if (distance < 0) {
		newTriangles = separateTriangles(triangles, -1);
	}
	else {
		//Draw the original (hasn't been cut yet).
		newTriangles = separateTriangles(triangles, 0);
	}

	return newTriangles;
}
/*
 Converts the given quad into two triangles.
 */
vector<triangle> cut::quadToTriangle(vector<vertex> quad) {
triangle triangle1;
triangle triangle2;
for (int i = 0; i < quad.size(); i++) {
	if (i == 0) {
		triangle1.v[0] = (quad[i]);
		triangle2.v[0] = (quad[i]);
	}
	else if (i == 1) {
		triangle2.v[1] = (quad[i]);
	}
	else if (i == 2) {
		triangle1.v[1] = (quad[i]);
	}
	else if (i == 3) {
		triangle1.v[2] = (quad[i]);
		triangle2.v[2] = (quad[i]);
	}
}

vector<triangle> triangles;
triangles.push_back(triangle1);
triangles.push_back(triangle2);

return triangles;
}

/*
Separates a single triangle making up the mesh of the geometry
triangle translated by normal * direction.
Quad triangles translated by normal * -direction.
*/
vector<triangle> cut::separateTriangles(vector<triangle> triangles, int direction) {
	vector<triangle> newTriangles = triangles;

	vec3 translateDirection = normal * direction;

	//Translation magnitude
	double normalMagntde = pow((pow(translateDirection.x, 2) + pow(translateDirection.y, 2) + pow(translateDirection.z, 2)), 0.5);

	//Translation direction
	vec3 translateUnit = translateDirection * (1 / normalMagntde);

	translateUnit = translateUnit * 0;

	for (int i = 0; i < newTriangles.size(); i++) {
		for (vertex &v : newTriangles[i].v) {
			if (i == 0 && direction != 0) {
				v.p.x = v.p.x + translateUnit.x;
				v.p.y = v.p.y + translateUnit.y;
				v.p.z = v.p.z + translateUnit.z;
			}
			else if (direction != 0) {
				v.p.x = v.p.x + translateUnit.x * -1;
				v.p.y = v.p.y + translateUnit.y * -1;
				v.p.z = v.p.z + translateUnit.z * -1;
			}
		}
	}

	return newTriangles;
}

triangle cut::separateTriangle(triangle t, int direction) {

	triangle newTriangle = t;

	vec3 translateDirection = normal * direction;

	//Translation magnitude
	double normalMagntde = pow((pow(translateDirection.x, 2) + pow(translateDirection.y, 2) + pow(translateDirection.z, 2)), 0.5);
	//Translation direction
	vec3 translateUnit = translateDirection * (1 / normalMagntde);

	translateUnit = translateUnit * 0;

	for (vertex &v : newTriangle.v) {
		v.p.x = v.p.x + translateUnit.x;
		v.p.y = v.p.y + translateUnit.y;
		v.p.z = v.p.z + translateUnit.z;
	}

	return newTriangle;
}

/*
Given some percentage. linearly interpolate along the edge in order to
calculate the new vertex values.
Add those values to the lists and finally give the vertex the corresponding index's
in order to find said values.
*/
vertex cut::findVertex(float percentage, vertex from, vertex to) {	//TODO check this
	vec3 normal = ((to.n - from.n) * percentage) + from.n;
	vec2 uv = ((to.t - from.t) * percentage) + from.t;
	vec3 point = ((to.p - from.p) * percentage) + from.p;

	vertex newVertex;
	newVertex.n = normal;
	newVertex.t = uv;
	newVertex.p = point;

	return newVertex;
}

/*
Creates a vector of triangles representing the mesh formed from the cut area.
*/
vector<triangle> cut::getMesh(vector<vertex> vertices, vertex centre){
	vector<triangle> mesh;

	int count = 0;
	for (int i = 0; i < vertices.size() - 1; i++) {
		if(count == 1) {
			count = 0;
			continue;
		}
		triangle t;
		t.v[0] = centre;
		t.v[1] = vertices[i];
		t.v[2] = vertices[i + 1];
		mesh.push_back(t);
		count++;
	}

	return mesh;
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
Helper method for finding the centroid of a triangle.
*/
vec3 cut::getCentroid(triangle t) {
	GLfloat centerX = (t.v[0].p.x + t.v[1].p.x + t.v[2].p.x) / 3;
	GLfloat centerY = (t.v[0].p.y + t.v[1].p.y + t.v[2].p.y) / 3;
	GLfloat centerZ = (t.v[0].p.z + t.v[1].p.z + t.v[2].p.z) / 3;
	
	vec3 centroid(centerX, centerY, centerZ);

	return centroid;
}

/*
Helper method for getting the centroid vertex of a convex polygon.
*/
vertex cut::getCentre(vector<vertex> polygon) {
	GLfloat centreNX = 0;
	GLfloat centreNY = 0;
	GLfloat centreNZ = 0;
	
	GLfloat centreTX = 0;
	GLfloat centreTY = 0;
	
	GLfloat centrePX = 0;
	GLfloat centrePY = 0;
	GLfloat centrePZ = 0;

	for (vertex v : polygon) {
		centreNX = centreNX + v.n.x;
		centreNY = centreNY + v.n.y;
		centreNZ = centreNZ + v.n.z;

		centreTX = centreTX + v.t.x;
		centreTY = centreTY + v.t.y;

		centrePX = centrePX + v.p.x;
		centrePY = centrePY + v.p.y;
		centrePZ = centrePZ + v.p.z;
	}

	int size = polygon.size();

	vec3 centreN(centreNX / size, centreNY / size, centreNZ / size);
	vec2 centreT(centreTX / size, centreTY / size);
	vec3 centreP(centrePX / size, centrePY / size, centrePZ / size);

	vertex centreVertex;
	centreVertex.n = centreN;
	centreVertex.t = centreT;
	centreVertex.p = centreP;

	return centreVertex;
}

vec3 cut::getGeometryCentre(vector<vec3> points) {
	GLfloat centreX = 0;
	GLfloat centreY = 0;
	GLfloat centreZ = 0;

	for (vec3 v : points) {
		centreX = centreX + v.x;
		centreY = centreY + v.y;
		centreZ = centreZ + v.z;
	}

	vec3 centroid(centreX / points.size(), centreY / points.size(), centreZ / points.size());

	return centroid;
}
