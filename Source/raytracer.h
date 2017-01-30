#ifndef RAYTRACER_INCLUDE
#define RAYTRACER_INCLUDE
#include <iostream>
#include <glm/glm.hpp>
#include <SDL.h>
#include <stdexcept>
#include <cstdlib>
#include <ctime>
#include <limits.h>
#include "SDLauxiliary.h"
#include "TestModel.h"

#define SCREEN_WIDTH 500
#define SCREEN_HEIGHT 500
#define FOCAL_LENGTH SCREEN_HEIGHT / 2.0f

using glm::vec2;
using glm::vec3;
using glm::mat3;

using std::cout;
using std::vector;
using std::endl;

struct Intersection
{
    vec3 position;
    float distance;
    int triangleIndex;
};

float interpolate_f(float start, float end, float step, float max);
void interpolate(float start, float end, vector<float>& result);
void interpolate_v(vec3 a, vec3 b, vector<vec3> &result);
int rand_i(int min, int max);
float rand_f();
vec2 convertTo2D(vec3 coords);
bool closestIntersection(vec3 start, vec3 dir, const vector<Triangle>& triangles, Intersection& closest);
void getRayDirection(int x, int y, vec3 &rayDir);
void printVector(const char* name, vec3 v);
bool triangleIntersection(vec3& point);
void update();
void draw();
#endif //RAYTRACER_INCLUDE