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

public:
	cut();
	void createCut(std::vector<comp308::vec3> plane);
	comp308::vec3 findNormal();
	int isInFront(comp308::vec3 vertex);
	void draw();
};