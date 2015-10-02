#pragma once

#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "comp308.hpp"

class cut {
private:
	comp308::vec3 findNormal();
	int isInFront(comp308::vec3 vertex);
	float calculateDisplacement(comp308::vec3 normal);
	std::vector<comp308::vec3> calculateIntersection(std::vector<comp308::vec3> v1, std::vector<comp308::vec3> v2);
	void cutTriangle(std::vector<comp308::vec3> rightVertices, std::vector<comp308::vec3> leftVertices);
	std::vector<std::vector<comp308::vec3>> quadToTriangle(std::vector<comp308::vec3> vertices);
	void draw(std::vector<std::vector<comp308::vec3>> triangles);
	comp308::vec3 getLine(comp308::vec3 position, comp308::vec3 direction, int length);
	float getLineDisplacement(comp308::vec3 position, comp308::vec3 direction);
public:
	cut();
	void createCut(std::vector<comp308::vec3> plane);
};