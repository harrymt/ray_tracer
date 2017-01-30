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

using glm::vec2;
using glm::vec3;
using glm::mat3;

using std::cout;
using std::vector;
using std::endl;

struct Intersection {
    vec3 position;
    float distance;
    int triangleIndex;
};

const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 500;
const float focalLength = SCREEN_HEIGHT / 2;
const vec3 cameraPos(0,0,-2);

extern SDL_Surface* screen;
extern int t;
extern vector<Triangle> triangles;

float Interpolate_f( float start, float end, float step, float max );
void Interpolate( float start, float end, vector<float>& result );
void Interpolate_v(vec3 a, vec3 b, vector<vec3> &result);
int rand_i(int min, int max);
float rand_f();
vec2 convertTo2D(vec3 coords);
bool ClosestIntersection(vec3 start, vec3 dir, const vector<Triangle>& triangles, Intersection& closestIntersection );
void GetRayDirection(int x, int y, vec3 &rayDir);
void printVector(const char* name, vec3 v);
bool triangleIntersection(vec3 point);
void Update();
void Draw();
#endif //RAYTRACER_INCLUDE
