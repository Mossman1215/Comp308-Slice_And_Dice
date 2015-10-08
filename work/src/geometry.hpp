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
private:
	std::vector<std::vector<comp308::vec3>> allTriangles;	//The tiangles that make up this geometry.
	comp308::vec3 m_color;
public:
	geometry(std::string filename, std::vector<std::vector<comp308::vec3>> triangle);
	geometry();
	void draw();
	void render();
	std::vector<std::vector<comp308::vec3>> getTriangles();
	void addToTriangles(std::vector<comp308::vec3> triangle);
	virtual ~geometry();
};