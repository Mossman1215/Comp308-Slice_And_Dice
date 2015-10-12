#pragma once

#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "comp308.hpp"

struct vertex {
	int p = 0; // index for point in m_points
	int t = 0; // index for uv in m_uvs
	int n = 0; // index for normal in m_normals
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

	std::vector<std::vector<comp308::vec3>> allTriangles;	//The triangles that make up this geometry.
	//std::vector<triangle> allTriangles;
	comp308::vec3 m_color;
public:
	geometry();
	//geometry(std::string filename);
	geometry(std::vector<std::vector<comp308::vec3>> triangle);
	void readOBJ(std::string filename);
	std::vector<triangle> createDisplayListPoly(std::string filename);
	void draw();
	void render();
	std::vector<std::vector<comp308::vec3>> getTriangles();
	void addToTriangles(std::vector<comp308::vec3> triangle);
	virtual ~geometry();
};