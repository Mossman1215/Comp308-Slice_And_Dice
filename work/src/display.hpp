#pragma once

#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "comp308.hpp"

class Display
{
public:
	Display();
	void draw();
	void render();
	virtual ~Display();
};

