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
#include "shaderLoader.hpp"
#include "geometry.hpp"
#include "physics.hpp"
#include "cut.hpp"
#include <map>

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
float g_zfar = 1000;


// Mouse controlled Camera values
//
bool g_mouseDown = false;
vec2 g_mousePos;
float g_yRotation = 0;
float g_xRotation = 0;
float g_yPosition = 0;
float g_zoomFactor = 4;

// Mouse controlled drawing values
//
bool g_drawMouse = false;
vec3 cut_draw_1;
vec3 cut_draw_2;

// For cutting in three dimensions.
//
vec3 cut_proj_1;
vec3 cut_proj_2;
int g_start_time=0;
float g_delta=0;
vector<geometry> g_geometry;
cut *g_cut = nullptr;
Rigidbody* box;
Rigidbody* box2;
Physics* physics;

// Utility variables
bool g_paused = false;
bool g_slow = false;
bool g_bounds = false;

geometry original;

// SHADERS CODES.
GLuint shader_code = 0;

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

// Build shader program.
void initShader(string vert, string frag, GLuint* address) {
	*address = makeShaderProgram(vert, frag);
}

// Draw function
//
void draw() {
	g_delta = (glutGet(GLUT_ELAPSED_TIME) - g_start_time) /1000.0f;
	g_start_time = glutGet(GLUT_ELAPSED_TIME);
	if(g_paused){
		g_delta = 0;
	} if (g_slow) {
		g_delta *= 0.1;
	}
	// Set up camera every frame
	setUpCamera();

	// Black background
	glClearColor(0.2f,0.2f,0.9f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Enable flags for normal rendering
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);

	// Set the current material (for all objects) to red
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE); 
	
	physics->update(g_delta, g_bounds);
	physics->checkCollisions(g_delta);
	
	
	// Render geometry
	// Enable Drawing texures
	glEnable(GL_TEXTURE_2D);
	// Set the location for binding the texture
	glActiveTexture(GL_TEXTURE0);
	// Bind the texture

	// Use the shader we made
	glUseProgram(shader_code);
	glUniform1f(glGetUniformLocation(shader_code, "radius"), 1.5);
	for (unsigned int i=0;i<g_geometry.size();i++ ) {

	    geometry Geometry = g_geometry[i];     
	    glPushMatrix();	
	    //get position from rigidbody corresponding to this geometry object
		Rigidbody* rigid = Geometry.getRigidbody();
		vec3 pos = rigid->position;
		glTranslatef(pos.x, pos.y, pos.z);
		glColor3f(1.0f, 0.0f, 0.0f);
		Geometry.render();
		glPopMatrix();
	}
	glUseProgram(0);
	
	//glPushMatrix();
	//    vec3 position = box->update(g_delta);
	//glPushMatrix();
	//    vec3 position = box->update(g_delta, g_bounds);
	//    glTranslatef(position.x,position.y,position.z);
	//    glColor3f(1,0,0);
	//    glutSolidCube(1);
	//glPopMatrix();
	//glPushMatrix();
	//    vec3 position2 = box2->update(g_delta, g_bounds);
	//    glTranslatef(position2.x,position2.y,position2.z);
	//    glColor3f(1,0,0);
	//    glutSolidCube(1);
	//glPopMatrix();
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1, 1, 1, 0.5);
	glBegin(GL_QUADS);
	glVertex3f(cut_draw_1.x, cut_draw_1.y, cut_draw_1.z);
	glVertex3f(cut_proj_1.x, cut_proj_1.y, cut_proj_1.z);
	glVertex3f(cut_proj_2.x, cut_proj_2.y, cut_proj_2.z);
	glVertex3f(cut_draw_2.x, cut_draw_2.y, cut_draw_2.z);
	glEnd();

	glBegin(GL_QUADS);
	glVertex3f(10, 0, 10);
	glVertex3f(-10, 0, 10);
	glVertex3f(-10, 0, -10);
	glVertex3f(10, 0, -10);
	glEnd();

	glBegin(GL_QUADS);
	glColor4f(0.4, 1, 0.4, 0.5);
	glVertex3f(10000, -0.01, 10000);
	glVertex3f(-10000, -0.01, 10000);
	glVertex3f(-10000, -0.01, -10000);
	glVertex3f(10000, -0.01, -10000);
	glEnd();

	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

	glColor4f(1, 0, 0, 1);
	glBegin(GL_LINES);
	glVertex3f(cut_draw_1.x, cut_draw_1.y, cut_draw_1.z);
	glVertex3f(cut_draw_2.x, cut_draw_2.y, cut_draw_2.z);
	glVertex3f(cut_proj_1.x, cut_proj_1.y, cut_proj_1.z);
	glVertex3f(cut_proj_2.x, cut_proj_2.y, cut_proj_2.z);
	glEnd();

	// Disable flags for cleanup (optional)
	glDisable(GL_NORMALIZE);
	glDisable(GL_COLOR_MATERIAL);

	glutSwapBuffers();

	// Queue the next frame to be drawn straight away
	glutPostRedisplay();
	g_delta =(float)((glutGet(GLUT_ELAPSED_TIME) - g_start_time)/1000);
}

// Unprojection system for creating cutting planes.
vec3 myUnProject(int x, int y, int z) {
	glLoadIdentity();

	GLint viewport[4];
	GLdouble modelView[16];
	GLdouble projection[16];
	GLfloat winX, winY, winZ;
	GLdouble resX, resY, resZ;
	setUpCamera();

	glGetIntegerv(GL_VIEWPORT, viewport);
	glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);


	winX = (float)x;
	winY = (float)viewport[3] - (float)y;
	winZ = z;

	gluUnProject(winX, winY, winZ, modelView, projection, viewport, &resX, &resY, &resZ);

	return vec3(resX, resY, resZ);
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

void reset() {
	g_geometry.clear();
	physics->clear();
	geometry new_g = original;
	new_g.setRigidBody();
	g_geometry.push_back(new_g);
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
	if(key ==' '){
		g_paused= !g_paused;
	}
     switch(key){
     case 'j':
             box2->addForce(vec3(-1,0,0));
               break;
       case 'l':
               box2->addForce(vec3(1,0,0));
             break;
       case 'i':
              box2->addForce(vec3(0,1,0));
              break;
       case 'k':
               box2->addForce(vec3(0,-1,0));
               break;
       case 'n':
               box2->addForce(vec3(0,0,-1));
              break;
       case 'm':
               box2->addForce(vec3(0,0,1));
			   break;
	   case '[':
		   g_slow = !g_slow;
		   break;
	   case 'b':
		   g_bounds = !g_bounds;
		   break;
	   case 'r':
		   reset();
		   break;
       }

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
				cut_draw_1 = myUnProject(x, y, 0);
				cut_proj_1 = myUnProject(x, y, 1);
				cut_draw_2 = myUnProject(x, y, 0);
				cut_proj_2 = myUnProject(x, y, 1);
			}
			if (!g_drawMouse) {
				cut_draw_2 = myUnProject(x, y, 0);
				cut_proj_2 = myUnProject(x, y, 1);

				//Create cut upon mouse release.
				vector<vec3> plane;
				plane.push_back(cut_proj_1);
				plane.push_back(cut_proj_2);
				plane.push_back(cut_draw_2);
				vector<geometry> allGeometry;
				allGeometry = g_cut->createCut(plane, g_geometry, physics);
				g_geometry = allGeometry;
				//for all geometry objects
				//preserve momentum
				//clear rigid body list
				//add all the new rigidbodies post cut
     				
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
		cut_draw_2 = myUnProject(x, y, 0);
		cut_proj_2 = myUnProject(x, y, 1);
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
	g_mainWindow = glutCreateWindow("COMP308 Final Project");


	// Initilise GLEW
	// must be done after creating GL context (glutCreateWindow in this case)
	GLenum err = glewInit();
	if (GLEW_OK != err) { // Problem: glewInit failed, something is seriously wrong.
		cerr << "Error: " << glewGetErrorString(err) << endl;
		abort(); // Unrecoverable error
	}

	cout << "Using OpenGL " << glGetString(GL_VERSION) << endl;
	cout << "Using GLEW " << glewGetString(GLEW_VERSION) << endl;

	//Create our physics and geometry
	physics = new Physics();
	// Finally create our geometry
	geometry g_sphere = geometry("../work/res/assets/sphere.obj", physics);
	original = g_sphere;
	g_geometry.push_back(g_sphere);

	g_cut = new cut();
	vector<vec3> vertex;
	vertex.push_back(vec3(-.5, .5, -.5));
	vertex.push_back(vec3(-.5, -.5, -.5));
	vertex.push_back(vec3(.5, .5, .5));
	vertex.push_back(vec3(.5, -.5, .5));
	/*box = new Rigidbody(vec3(0, 10, 0), vertex, 1, vertex.size(), vec3(0, 0, 0));
	box2 = new Rigidbody(vec3(.5, 20, .5), vertex, 1, vertex.size(), vec3(0, 0, 0));
	physics->addRigidbody(box);
	physics->addRigidbody(box2);*/

	initShader("../work/res/shaders/melon_shader.vert", "../work/res/shaders/melon_shader.frag", &shader_code);

	// Register functions for callback
	glutDisplayFunc(draw);
	glutReshapeFunc(reshape);
	
	glutKeyboardFunc(keyboardCallback);
	glutSpecialFunc(specialCallback);

	glutMouseFunc(mouseCallback);
	glutMotionFunc(mouseMotionCallback);

	cut_draw_1 = vec3(-1, -1, 0);
	cut_draw_2 = vec3(-1, -1, 0);


	// Create a light on the camera
	initLight();
	// Loop required by OpenGL
	// This will not return until we tell OpenGL to finish
	glutMainLoop();

	// Don't forget to delete all pointers that we made
	delete box;
	delete box2;
	delete physics;
	delete g_cut;
	return 0;
}
