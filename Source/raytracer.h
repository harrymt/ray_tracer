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
#include <vector>
#include "SDLauxiliary.h"

#define FOCAL 2.0f
#define TRUE_SCREEN_WIDTH 500
#define TRUE_SCREEN_HEIGHT 500
#define SSAA 1
#define SCREEN_WIDTH TRUE_SCREEN_WIDTH * SSAA
#define SCREEN_HEIGHT TRUE_SCREEN_HEIGHT * SSAA
#define FOCAL_LENGTH SCREEN_HEIGHT / FOCAL
#define SHADOW_STR 12.5f
#define BIAS 1e-4
#define SOFT_SHADOW_SAMPLES 20
#define SOFT_SHADOW_MAX_OFFSET 0.02f // was 1 with 20 samples

const float pi = atan(1.0);

using glm::vec2;
using glm::vec3;
using glm::mat3;

using std::cout;
using std::vector;
using std::endl;

class Triangle
{
public:
	glm::vec3 v0;
	glm::vec3 v1;
	glm::vec3 v2;
	//glm::vec3 e1;
	//glm::vec3 e2;
	glm::vec3 normal;
	glm::vec3 color;
	float normal_length;

	inline Triangle(){}
	inline Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 color)
		: v0(v0), v1(v1), v2(v2), color(color)
	{
		ComputeNormal();
	}

	inline void ComputeNormal()
	{
		glm::vec3 e1 = v1 - v0;
		glm::vec3 e2 = v2 - v0;
		normal = glm::normalize(glm::cross(e2, e1));
		normal_length = glm::length(normal);
	}

	inline float area()
	{
		float h = glm::length(v1 - (v0 + v2) / 2.0f);
		float w = glm::length(v2 - v0);
		return h * w / 2.0f;
	}
};

struct Intersection
{
    vec3 position;
    float distance;
    int triangleIndex;
};

struct photon_t
{
	vec3 pos;
	vec3 colour;
	vec3 normal;
	float normal_length;
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
void load(std::string name, glm::vec3 colour, std::vector<Triangle>& triangles);
void scale(std::vector<Triangle>& triangles, float size);
#endif //RAYTRACER_INCLUDE
