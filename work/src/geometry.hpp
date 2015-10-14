#pragma once

#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "comp308.hpp"

struct vertex {
	comp308::vec3 p; 
	comp308::vec2 t;
	comp308::vec3 n;
};

struct triangle {
	vertex v[3]; //requires 3 verticies
};

class geometry
{
private:
	// Feilds for storing raw obj information
	std::string m_filename;
	std::vector<comp308::vec3> m_points;	// Point list
	std::vector<comp308::vec2> m_uvs;		// Texture Coordinate list
	std::vector<comp308::vec3> m_normals;	// Normal list
	std::vector<triangle> m_triangles;		// Triangle/Face list

	comp308::vec3 m_color;

	// IDs for the display list to render
	GLuint m_displayListPoly = 0; // DisplayList for Polygon
public:
	geometry();
	geometry(std::string filename);
	//geometry(std::vector<std::vector<comp308::vec3>> triangle);
	void readOBJ(std::string filename);
	void createNormals();
	//std::vector<triangle> createDisplayListPoly();
	void createDisplayListPoly();
	void draw();
	void render();
	std::vector<triangle> getTriangles();
	void addToTriangles(triangle triangle);
	virtual ~geometry();
        std::vector<comp308::vec3> getPoints();
};
