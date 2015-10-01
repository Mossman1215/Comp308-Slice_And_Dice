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
	void calculateIntersection(std::vector<comp308::vec3> v1, std::vector<comp308::vec3> v2);
	comp308::vec3 getLine(comp308::vec3 position, comp308::vec3 direction, int length);
	float getLineDisplacement(comp308::vec3 position, comp308::vec3 direction);
public:
	cut();
	void createCut(std::vector<comp308::vec3> plane);
};