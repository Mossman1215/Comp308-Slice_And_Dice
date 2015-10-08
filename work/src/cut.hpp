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
	comp308::vec3 findNormal();
	int isInFront(comp308::vec3 vertex);
	float calculateDisplacement(comp308::vec3 normal);
	std::vector<comp308::vec3> calculateIntersection(std::vector<comp308::vec3> v1, std::vector<comp308::vec3> v2);
	std::vector<std::vector<comp308::vec3>> cutTriangle(std::vector<comp308::vec3> frontVertices, std::vector<comp308::vec3> backVertices);
	std::vector<std::vector<comp308::vec3>> quadToTriangle(std::vector<comp308::vec3> vertices);
	std::vector<std::vector<comp308::vec3>> separateTriangle(std::vector<std::vector<comp308::vec3>> triangles, int direction);
	comp308::vec3 getLine(comp308::vec3 position, comp308::vec3 direction, int length);
	float getLineDisplacement(comp308::vec3 position, comp308::vec3 direction);
public:
	cut();
	std::vector<geometry> createCut(std::vector<comp308::vec3> plane, std::vector<geometry> geometry);
};