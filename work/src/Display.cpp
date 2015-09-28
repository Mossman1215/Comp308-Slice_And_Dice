#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>

#include "comp308.hpp"
#include "display.hpp"

using namespace std;
using namespace comp308;

display::display(){}

void display::draw() {
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

void display::render() {
	glColor3f(0, 1, 1);
	//glutSolidTeapot(0.2);

	glBegin(GL_TRIANGLES);
	glNormal3f(0.0, 0.0, 1.0);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-5.0, -5.0, 0.0);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-5.0, 5.0, 0.0);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(5.0, 5.0, 0.0);
	glEnd();
	glFlush();
}

display::~display(){}