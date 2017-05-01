#include "raytracer.h"

#define NUM_PHOTONS 25000//0
#define SEARCH_RADIUS 0.4
#define SEARCH_RADIUS_SQUARED SEARCH_RADIUS * SEARCH_RADIUS

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
		Intersection closest;
		bool first = true;
		for (int bounces = 5; bounces > 0; --bounces)
		{
			if (closestIntersection(pos, dir, triangles, num_triangles, closest))
			{
				// take some of the intensity
				// move along
				pos = closest.position;
				photon_t photon;
				photon.pos = pos;

				Triangle& triangle = triangles[closest.triangleIndex];

				dir = triangle.normal;
				photon.normal = dir;
				photon.normal_length = glm::length(photon.normal);
				dir = glm::rotateX(dir, glm::linearRand(-0.99f, 0.99f) * 3.14159f / 2.0f);
				dir = glm::rotateY(dir, glm::linearRand(-0.99f, 0.99f) * 3.14159f / 2.0f);
				dir = glm::rotateZ(dir, glm::linearRand(-0.99f, 0.99f) * 3.14159f / 2.0f);
				dir = glm::normalize(dir);
				
				colour.r *= triangle.color.r;
				colour.g *= triangle.color.g;
				colour.b *= triangle.color.b;
				photon.colour = glm::clamp(colour, vec3(0.f, 0.f, 0.f), vec3(1.f, 1.f, 1.f));

				if (!first) photons.push_back(photon);
				first = false;

				pos.x += dir.x*0.00015;
				pos.y += dir.y*0.00015;
				pos.z += dir.z*0.00015;
			}
			// The photon has left the world and is of no use
			else break;
		}
	}
	
	return photons;
}

glm::vec3 gather(vec3& pos, Triangle& triangle)
{
	photon_t* photons_ = photons.data();
	vec3 gather_colour(0.0f, 0.0f, 0.0f);
	int n = 0;
	for (int i = 0; i < photons.size(); ++i)
	{
		photon_t& photon = photons_[i];
		float dist_sq = glm::distance2(photon.pos, pos); // tiny optimisation, avoid square root!
		if (dist_sq < SEARCH_RADIUS_SQUARED)
		{
			// additional constraint, we will ensure we only act on the plane normal to our intersection
			//float dot = glm::dot(triangle.v0 - photon.pos, triangle.normal);
			//if (dot == 0.0f)

			// That was great, but not for complex geometry like bunnies
			// Instead we will only collect within a certain angle
			float alpha = glm::dot(triangle.normal , photon.normal) / (triangle.normal_length * photon.normal_length);
			if (alpha > 0.25)
			{
				gather_colour += photon.colour;
				++n;
			}
		}
	}
	gather_colour /= (2.4f*n);
	
	return glm::clamp(gather_colour, vec3(0.f, 0.f, 0.f), vec3(1.f, 1.f, 1.f)) * 1.5f;
}

