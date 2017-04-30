#include "raytracer.h"

#define NUM_PHOTONS 100000
#define N 20
#define SEARCH_RADIUS 0.2

extern glm::vec3 lightPos;
extern Triangle* triangles;
extern size_t num_triangles;

// Two stages
// Shoot;
//   fire out photons from light sources (n photons from a I light source each has power I/n)
//   every time they itersect with a surface they will get partially deposited and then continue
//   it will be reemitted in a random direction, since we will assume total diffusity. Uniform cosine distribution.
//   We will limit to 8 bounces?
// Gather;
//   perform normal raytracing to find illuminations

// We'll make the photon map a vector for the time being, just to get everything working nicely
std::vector<photon_t> photons;

std::vector<photon_t> generateMap()
{
	for (int n = 0; n != NUM_PHOTONS; n++)
	{
		// Need to make a photon
		glm::vec3 dir = glm::linearRand<float>(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
		dir = glm::normalize(dir);
		glm::vec3 pos = lightPos;
		glm::vec3 colour = glm::vec3(1, 1, 1);
		float lum = 3.f;// / NUM_PHOTONS;
		Intersection closest;
		bool first = true;
		while (lum > 0)
		{
			if (closestIntersection(pos, dir, triangles, num_triangles, closest))
			{
				// take some of the intensity
				// move along
				pos = closest.position;
				photon_t photon;
				photon.x = pos.x;
				photon.y = pos.y;
				photon.z = pos.z;

				Triangle& triangle = triangles[closest.triangleIndex];
				dir = triangle.normal;
				dir = glm::rotateX(dir, glm::linearRand(-1.0f, 1.0f) * 3.14159f / 2.0f);
				dir = glm::rotateY(dir, glm::linearRand(-1.0f, 1.0f) * 3.14159f / 2.0f);
				dir = glm::rotateZ(dir, glm::linearRand(-1.0f, 1.0f) * 3.14159f / 2.0f);
				dir = glm::normalize(dir);
				
				colour.r *= triangle.color.r;
				colour.g *= triangle.color.g;
				colour.b *= triangle.color.b;
				photon.colour = colour;
				photon.lum = 1.0;

				if (!first) photons.push_back(photon);
				first = false;

				photon.x += dir.x*0.015;
				photon.y += dir.y*0.015;
				photon.z += dir.z*0.015;

				lum -= photon.lum;
			}
			// The photon has left the world and is of no use
			else break;
		}
	}
	
	return photons;
}

glm::vec3 gather(vec3 pos, vec3 normal)
{
	// Here is a pos with a certain illumination and a certain colour
	// We need to find the N nearest photons to it, average out their colour
	// (potentially as a distance weighted mean?) and then perform an illumination
	// weighted average with the directLight. That is to say, indirect light should make
	// more difference the dimmer the direct light is

	std::vector<photon_t*> nearest;
	// really really bad n^2 knn algorithm, we'll fix this
	/*for (int i = 0; i < N; i++)
	{
		float min_dist = std::numeric_limits<float>::infinity();
		photon_t* min_photon;
		for (photon_t& photon : photons)
		{
			float dist = glm::distance(vec3(photon.x, photon.y, photon.z), pos);
			if (dist < min_dist)
			{
				bool skip = false;
				for (photon_t* photon_ : nearest) if (photon_ == &photon)
				{
					skip = true;
					break;
				}
				if (skip) continue;
				min_photon = &photon;
				min_dist = dist;
			}
		}
		nearest.push_back(min_photon);
	}*/

	for (photon_t& photon : photons)
	{
		float dist = glm::distance(vec3(photon.x, photon.y, photon.z), pos);
		if (dist < SEARCH_RADIUS)
		{
			// additional constraint, we will ensure we only act on the plane normal to our intersection
			float dot = glm::dot(pos - vec3(photon.x, photon.y, photon.z), normal);
			if (0.0f <= dot && dot <= std::numeric_limits<float>::min())
			{
				nearest.push_back(&photon);
			}
		}
	}

	vec3 gather_colour(0.0f, 0.0f, 0.0f);
	float sum_dist = 0.0f;
	for (photon_t* photon : nearest)
	{
		float dist = glm::distance(vec3(photon->x, photon->y, photon->z), pos);
		gather_colour += photon->colour *1.0f;// / sqrt(dist);
		sum_dist += 1.0f;// / sqrt(dist);
	}
	gather_colour /= 2.4f*sum_dist;
	return gather_colour;
}