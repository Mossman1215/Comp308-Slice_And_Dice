//---------------------------------------------------------------------------
//
// Copyright (c) 2015 Taehyun Rhee, Joshua Scott, Ben Allen
//
// This software is provided 'as-is' for assignment of COMP308 in ECS,
// Victoria University of Wellington, without any express or implied warranty. 
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// The contents of this file may not be copied or duplicated in any form
// without the prior permission of its owner.
//
//----------------------------------------------------------------------------

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>
#include "comp308.hpp"
#include "display.hpp"

using namespace std;
using namespace comp308;


// Global variables
// 
GLuint g_winWidth  = 640;
GLuint g_winHeight = 480;
GLuint g_mainWindow = 0;


// Projection values
// 
float g_fovy = 20.0;
float g_znear = 0.1;
float g_zfar = 1000.0;


// Mouse controlled Camera values
//
bool g_mouseDown = false;
vec2 g_mousePos;
float g_yRotation = 0;
float g_xRotation = 0;
float g_yPosition = 0;
float g_zoomFactor = 1.0;

// Mouse controlled drawing values
//
bool g_drawMouse = false;
vec2 cut_draw_1;
vec2 cut_draw_2;


display *g_display = nullptr;

// Sets up where and what the light is
// Called once on start up
// 
void initLight() {
	float direction[]	  = {0.0f, 0.0f, 1.0f, 0.0f};
	float diffintensity[] = {0.7f, 0.7f, 0.7f, 1.0f};
	float ambient[]       = {0.2f, 0.2f, 0.2f, 1.0f};

	glLightfv(GL_LIGHT0, GL_POSITION, direction);
	glLightfv(GL_LIGHT0, GL_DIFFUSE,  diffintensity);
	glLightfv(GL_LIGHT0, GL_AMBIENT,  ambient);	
	
	
	glEnable(GL_LIGHT0);
}


// Sets up where the camera is in the scene
// Called every frame
//use glortho for putting a point in space for a clicked position
// 
void setUpCamera() {
	// Set up the projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(g_fovy, float(g_winWidth) / float(g_winHeight), g_znear, g_zfar);

	// Set up the view part of the model view matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Load camera transforms
	glTranslatef(0, g_yPosition, -5 * g_zoomFactor);
	glRotatef(g_xRotation, 1, 0, 0);
	glRotatef(g_yRotation, 0, 1, 0);
}


// Draw function
//
void draw() {

	// Set up camera every frame
	setUpCamera();

	// Black background
	glClearColor(0.0f,0.0f,0.0f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Enable flags for normal rendering
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);


	// Set the current material (for all objects) to red
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE); 
	glColor3f(1.0f,0.0f,0.0f);


	// Render geometry
	g_display->draw();

	// Disable flags for cleanup (optional)
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_NORMALIZE);
	glDisable(GL_COLOR_MATERIAL);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, g_winWidth, 0, g_winHeight, 0, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glColor3f(1, 1, 1);
	glBegin(GL_LINES);
	glVertex2f(cut_draw_1.x, cut_draw_1.y);
	glVertex2f(cut_draw_2.x, cut_draw_2.y);
	glEnd();

	glutSwapBuffers();

	// Queue the next frame to be drawn straight away
	glutPostRedisplay();
}



vec3 myUnProject(int x, int y) {
	glLoadIdentity();

	GLint viewport[4];
	GLdouble modelView[16];
	GLdouble projection[16];
	GLfloat winX, winY, winZ;
	GLdouble resX, resY, resZ;

	glGetIntegerv(GL_VIEWPORT, viewport);
	glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);


	winX = (float)x;
	winY = (float)viewport[3] - (float)y;
	glReadPixels(x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
	winZ = 1;

	gluUnProject(winX, winY, winZ, modelView, projection, viewport, &resX, &resY, &resZ);

	return vec3(resX, resY, resZ) / (1 /* far distance/camera distance */);
}


// Reshape function
// 
void reshape(int w, int h) {
    if (h == 0) h = 1;

	g_winWidth = w;
	g_winHeight = h;
    
    // Sets the openGL rendering window to match the window size
    glViewport(0, 0, g_winWidth, g_winHeight);  
}

// Keyboard callback
// Called once per button state change
//
void keyboardCallback(unsigned char key, int x, int y) {
	if (key == GLUT_KEY_RIGHT || key == 'a') {
		g_yRotation += 5;
	} else if (key == GLUT_KEY_LEFT || key == 'd') {
		g_yRotation -= 5;
	}else if (key == GLUT_KEY_UP || key == 'w') {
		g_yPosition += 0.03;
	}else if (key == GLUT_KEY_DOWN || key == 's') {
		g_yPosition -= 0.03;
	}
	cout << key << " " << g_yRotation << endl;
}


// Special Keyboard callback
// Called once per button state change
//
void specialCallback(int key, int x, int y) {

}


// Mouse Button Callback function
// (x,y) :: (0,0) is top left and (g_winWidth, g_winHeight) is bottom right
// state :: 0 is down, 1 is up
// 
// Called once per button state change
// 
void mouseCallback(int button, int state, int x, int y) {
	//cout << "Mouse Callback :: button=" << button << ", state=" << state << ", (" << x << "," << y << ")" << endl;
	switch(button){

		case 0: //left mouse button
			g_drawMouse = (state==0);
			if (g_drawMouse) {
				cut_draw_1 = vec2(x, g_winHeight - y);
				cut_draw_2 = vec2(x, g_winHeight - y);
			}
			if (!g_drawMouse) {
				cut_draw_2 = vec2(x, g_winHeight - y);
			}
			break;

		case 1:
			g_zoomFactor = 1;
			g_yPosition = 0;
			g_yRotation = 0;
			g_xRotation = 0;
			break;

		case 2:
			g_mouseDown = (state == 0);
			g_mousePos = vec2(x, y);
			break;

		case 3: //scroll foward/up
			g_zoomFactor /= 1.1;
			break;

		case 4: //scroll back/down
			g_zoomFactor *= 1.1;
			break;
	}
}


// Mouse Motion Callback function
// Called once per frame if the mouse has moved and
// at least one mouse button has an active state
void mouseMotionCallback(int x, int y) {
	//cout << "Mouse Motion Callback :: (" << x << "," << y << ")" << endl;
	if (g_drawMouse) {
		cut_draw_2 = vec2(x, g_winHeight - y);
	}
	if (g_mouseDown) {
		vec2 dif = vec2(x, y) - g_mousePos;
		g_mousePos = vec2(x, y);
		g_yRotation += 0.3 * dif.x;
		g_xRotation += 0.3 * dif.y;
	}
}



//Main program
// 
int main(int argc, char **argv) {

	// Initialise GL, GLU and GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);

	// Initialise window size and create window
	glutInitWindowSize(g_winWidth, g_winHeight);
	g_mainWindow = glutCreateWindow("COMP308 Assignment 2");


	// Initilise GLEW
	// must be done after creating GL context (glutCreateWindow in this case)
	GLenum err = glewInit();
	if (GLEW_OK != err) { // Problem: glewInit failed, something is seriously wrong.
		cerr << "Error: " << glewGetErrorString(err) << endl;
		abort(); // Unrecoverable error
	}

	cout << "Using OpenGL " << glGetString(GL_VERSION) << endl;
	cout << "Using GLEW " << glewGetString(GLEW_VERSION) << endl;


	// Register functions for callback
	glutDisplayFunc(draw);
	glutReshapeFunc(reshape);
	
	glutKeyboardFunc(keyboardCallback);
	glutSpecialFunc(specialCallback);

	glutMouseFunc(mouseCallback);
	glutMotionFunc(mouseMotionCallback);

	cut_draw_1 = vec2(0, 0);
	cut_draw_2 = vec2(500, 500);


	// Create a light on the camera
	initLight();

	// Finally create our geometry
	g_display = new display();

	// Loop required by OpenGL
	// This will not return until we tell OpenGL to finish
	glutMainLoop();

	// Don't forget to delete all pointers that we made
	return 0;
}