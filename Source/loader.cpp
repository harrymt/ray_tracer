#include "raytracer.h"
#include <fstream>

void load(std::string name, glm::vec3 colour, std::vector<Triangle>& triangles)
{
	std::ifstream file(name);
	char buffer[128];
	std::vector<glm::vec3> vertices;
	if (file.is_open())
	{
		int size = triangles.size();
		std::cout << "Loading: " << name << std::endl;
		while (!file.eof())
		{
			switch (file.peek())
			{
			case 'v':
				file.get();
				float x, y, z;
				file >> x >> y >> z;
				vertices.push_back(vec3(x, y, z));
				break;
			case 'f':
			{
				file.get();
				int a, b, c;
				file >> a >> b >> c;
				--a; --b; --c;
				Triangle triangle(vertices[a], vertices[b], vertices[c], colour);
				triangles.push_back(triangle);
				break;
			}
			case '\n':
				file.get();
				break;
			case '#':
			default:
				file.getline(buffer, 128);
				break;
			}
		}
		std::cout << "Loaded " << triangles.size() - size << " new triangles" << std::endl;
		file.close();
	}
	else
	{
		std::cout << "Cannot open " << name << ", aborting" << std::endl;
	}
}

void scale(std::vector<Triangle>& triangles, float size)
{
	// ----------------------------------------------
	// Scale to the volume [-1,1]^3

	for (Triangle& triangle : triangles)
	{
		triangle.v0 *= 2 / size;
		triangle.v1 *= 2 / size;
		triangle.v2 *= 2 / size;

		triangle.v0 -= vec3(1, 1, 1);
		triangle.v1 -= vec3(1, 1, 1);
		triangle.v2 -= vec3(1, 1, 1);

		triangle.v0.x *= -1;
		triangle.v1.x *= -1;
		triangle.v2.x *= -1;

		triangle.v0.y *= -1;
		triangle.v1.y *= -1;
		triangle.v2.y *= -1;

		triangle.ComputeNormal();
	}
}