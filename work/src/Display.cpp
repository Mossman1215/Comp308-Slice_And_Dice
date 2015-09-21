#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>

#include "comp308.hpp"
#include "Display.h"

using namespace std;
using namespace comp308;

Display::Display(){}

void Display::draw() {
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

void Display::render() {
	glColor3f(0, 1, 1);
	glutSolidTeapot(0.2);
}

Display::~Display(){}