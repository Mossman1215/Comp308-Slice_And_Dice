#pragma once

#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "comp308.hpp"

class geometry
{
public:
	geometry(std::string filename, std::vector<std::vector<comp308::vec3>> triangle);
	void draw();
	void render();
	std::vector<std::vector<comp308::vec3>> getTriangles();
	void addToTriangles(std::vector<comp308::vec3> triangle);
	virtual ~geometry();
};
