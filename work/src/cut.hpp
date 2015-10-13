#pragma once

#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "comp308.hpp"
#include "geometry.hpp"

class cut {
private:
	std::vector<geometry> cutGeometry(geometry geometry);
	comp308::vec3 findNormal();
	int isInFront(comp308::vec3 vertex);
	float calculateDisplacement(comp308::vec3 normal);
	std::vector<vertex> calculateIntersection(std::vector<vertex> v1, std::vector<vertex> v2);
	std::vector<triangle> cutTriangle(std::vector<vertex> frontVertices, std::vector<vertex> backVertices);
	std::vector<triangle> quadToTriangle(std::vector<vertex> vertices);
	std::vector<triangle> separateTriangles(std::vector<triangle> triangles, int direction);
	void separateTriangle(triangle t, int direction);
	//std::vector<comp308::vec3> separateTriangle(std::vector<comp308::vec3> triangle, int direction);
	vertex findVertex(float percentage, vertex from, vertex to);
	comp308::vec3 getLine(comp308::vec3 position, comp308::vec3 direction, float length);
	float getLineDisplacement(comp308::vec3 position, comp308::vec3 direction);
	comp308::vec3 getCentroid(triangle t);
public:
	cut();
	std::vector<geometry> createCut(std::vector<comp308::vec3> plane, std::vector<geometry> geometry);
};
