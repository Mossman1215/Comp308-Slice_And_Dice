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

geometry::geometry(){}

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
	glFlush();
}

geometry::~geometry(){}