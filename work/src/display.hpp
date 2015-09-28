#pragma once

#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "comp308.hpp"

class display
{
public:
	display();
	void draw();
	void render();
	virtual ~display();
};

