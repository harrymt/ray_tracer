#include "raytracer.h"

#define NUM_PHOTONS 250000
#define SEARCH_RADIUS 0.4

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
		float lum = 5.f;// / NUM_PHOTONS;
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
				photon.pos = pos;

				Triangle& triangle = triangles[closest.triangleIndex];
				dir = triangle.normal;
				dir = glm::rotateX(dir, glm::linearRand(-0.99f, 0.99f) * 3.14159f / 2.0f);
				dir = glm::rotateY(dir, glm::linearRand(-0.99f, 0.99f) * 3.14159f / 2.0f);
				dir = glm::rotateZ(dir, glm::linearRand(-0.99f, 0.99f) * 3.14159f / 2.0f);
				dir = glm::normalize(dir);
				
				colour.r *= triangle.color.r;
				colour.g *= triangle.color.g;
				colour.b *= triangle.color.b;
				photon.colour = colour;
				photon.lum = 1.0f;

				if (!first) photons.push_back(photon);
				first = false;

				pos.x += dir.x*0.00015;
				pos.y += dir.y*0.00015;
				pos.z += dir.z*0.00015;

				lum -= photon.lum;
			}
			// The photon has left the world and is of no use
			else break;
		}
	}
	
	return photons;
}

glm::vec3 gather(vec3& pos, Triangle& triangle)
{
	// Here is a pos with a certain illumination and a certain colour
	// We need to find the N nearest photons to it, average out their colour
	// (potentially as a distance weighted mean?) and then perform an illumination
	// weighted average with the directLight. That is to say, indirect light should make
	// more difference the dimmer the direct light is

	std::vector<photon_t*> nearest;

	for (photon_t& photon : photons)
	{
		float dist = glm::distance(photon.pos, pos);
		if (dist < SEARCH_RADIUS)
		{
			// additional constraint, we will ensure we only act on the plane normal to our intersection

			float dot = glm::dot(triangle.v0 - photon.pos, triangle.normal);
			//if (-std::numeric_limits<float>::min()*10.0f <= dot && dot <= std::numeric_limits<float>::min()*10.0f)
			if (dot == 0.0f)
			{
				nearest.push_back(&photon);
			}
		}
	}

	vec3 gather_colour(0.0f, 0.0f, 0.0f);
	float sum_dist = 0.0f;
	for (photon_t* photon : nearest)
	{
		float dist = glm::distance(photon->pos, pos);
		gather_colour += glm::clamp(photon->colour, vec3(0.f, 0.f, 0.f), vec3(1.f, 1.f, 1.f));// *1.0f / sqrt(dist);
		sum_dist += 1.0f;// / sqrt(dist);
	}
	gather_colour /= 2.4f*sum_dist;
	
	return glm::clamp(gather_colour, vec3(0.f, 0.f, 0.f), vec3(1.f, 1.f, 1.f));
}