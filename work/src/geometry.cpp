#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>

#include "comp308.hpp"
#include "geometry.hpp"

using namespace std;
using namespace comp308;

vector<vector<vec3>> allTriangles;

geometry::geometry() {}

geometry::geometry(string filename, vector<vector<vec3>> triangles){
	allTriangles = triangles;
}

void geometry::draw() {
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	//Create a new GLUquadric object; to allow you to draw cylinders
	GLUquadric *quad = gluNewQuadric();
	if (quad == 0) {
		cerr << "Not enough memory to allocate space to draw" << endl;
		exit(EXIT_FAILURE);
	}

	//Render geometry
	render();

	// Clean up
	gluDeleteQuadric(quad);
	glPopMatrix();

}

void geometry::render() {
	glColor3f(0, 1, 1);
	for (vector<vec3> triangle : allTriangles) {
		glBegin(GL_TRIANGLES);
		glNormal3f(0.0, 0.0, 1.0);
		for (vec3 vertex : triangle) {
			glVertex3f(vertex.x, vertex.y, vertex.z);
		}
		glEnd();
	}
}

vector<vector<vec3>> geometry::getTriangles() {
	return allTriangles;
}

void geometry::addToTriangles(vector<vec3> triangle) {
	allTriangles.push_back(triangle);
}

geometry::~geometry(){}