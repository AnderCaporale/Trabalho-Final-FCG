#ifndef COLLISIONS_HPP
#define COLLISIONS_HPP

#define _USE_MATH_DEFINES
#define STB_IMAGE_IMPLEMENTATION
#include <cmath>
#include <cstdio>
#include <cstdlib>

// Headers abaixo são específicos de C++
#include <map>
#include <stack>
#include <string>
#include <vector>
#include <limits>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <bits/stdc++.h>

// Headers da biblioteca GLM: criação de matrizes e vetores.
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#include "structs.hpp"

void checkCollisionWithWalls(glm::vec4& player_position, glm::vec4& move_direction, std::vector<Wall>& walls);

#endif // COLLISIONS_HPP