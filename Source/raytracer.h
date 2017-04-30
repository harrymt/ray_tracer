#ifndef RAYTRACER_INCLUDE
#define RAYTRACER_INCLUDE
#include <iostream>
#include <glm/glm.hpp>
#include "glm/ext.hpp"
#include <SDL.h>
#include <stdexcept>
#include <cstdlib>
#include <ctime>
#include <limits.h>
#include <cmath>
#include "SDLauxiliary.h"
#include "TestModel.h"

#define FOCAL 2.0f
#define TRUE_SCREEN_WIDTH 500
#define TRUE_SCREEN_HEIGHT 500
#define SSAA 3
#define SCREEN_WIDTH TRUE_SCREEN_WIDTH * SSAA
#define SCREEN_HEIGHT TRUE_SCREEN_HEIGHT * SSAA
#define FOCAL_LENGTH SCREEN_HEIGHT / FOCAL
#define SHADOW_STR 12.5f
#define BIAS 1e-4
#define SOFT_SHADOW_SAMPLES 1
#define SOFT_SHADOW_MAX_OFFSET 0.01f // was 1 with 20 samples

const float pi = atan(1.0);

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

struct photon_t
{
	//float x, y, z;
	vec3 pos;
	vec3 colour;
	float lum;
};

vec3 directLight(const Intersection &i, Triangle& closestTriangle, const Triangle* triangles, const size_t num_triangles);
float interpolate_f(float start, float end, float step, float max);
void interpolate(float start, float end, vector<float>& result);
void interpolate_v(vec3 a, vec3 b, vector<vec3> &result);
int rand_i(int min, int max);
float rand_f();
vec2 convertTo2D(vec3 coords);
bool closestIntersection(vec3 start, vec3 dir, const Triangle* triangles, const size_t num_triangles, Intersection& closest);
void getRayDirection(int x, int y, vec3 &rayDir);
void printVector(const char* name, vec3 v);
bool triangleIntersection(vec3& point);
void update();
void draw();
std::vector<photon_t> generateMap();
glm::vec3 gather(vec3& pos, Triangle& triangle);
void generateLightSample();
#endif //RAYTRACER_INCLUDE
