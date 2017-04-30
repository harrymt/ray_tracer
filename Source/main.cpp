#include "raytracer.h"
#include "omp.h"

#define D2R(x) x*pi/180

SDL_Surface* screen;
int t;
Triangle* triangles;
size_t num_triangles;

vec3 cameraPos(0, 0, -FOCAL);
const float delta_displacement = 0.1f;
glm::vec3 indirectLight = 0.5f * glm::vec3(1, 1, 1);
glm::vec3 lightPos(0, -0.5, -0.7);
glm::vec3 lightColor = 14.f * glm::vec3(1, 1, 1);
glm::vec3 lightSample[SOFT_SHADOW_SAMPLES];

const float theta = D2R(5);

const mat3 rota(cos(theta),  0, sin(theta),
                0,           1, 0,
                -sin(theta), 0, cos(theta));

const mat3 rotc(cos(-theta),  0, sin(-theta),
                0,            1, 0,
                -sin(-theta), 0, cos(-theta));

mat3 currentRot(1, 0, 0, 0, 1, 0, 0, 0, 1);

void update()
{
    // Compute frame time:
    int t2 = SDL_GetTicks();
    float dt = (float) (t2-t);
    t = t2;
    cout << "Render time: " << dt << " ms.\n";

    Uint8* keystate = SDL_GetKeyState(0);
    if (keystate[SDLK_w])
    {
        cameraPos = cameraPos * glm::inverse(currentRot);
        cameraPos[2] += delta_displacement;
        cameraPos = cameraPos * currentRot;
    }
    if (keystate[SDLK_s])
    {
        cameraPos = cameraPos * glm::inverse(currentRot);
        cameraPos[2] -= delta_displacement;
        cameraPos = cameraPos * currentRot;
    }
    if (keystate[SDLK_d])
    {
        cameraPos = cameraPos * glm::inverse(currentRot);
        cameraPos[0] += delta_displacement;
        cameraPos = cameraPos * currentRot;
    }
    if (keystate[SDLK_a])
    {
        cameraPos = cameraPos * glm::inverse(currentRot);
        cameraPos[0] -= delta_displacement;
        cameraPos = cameraPos * currentRot;
    }
    if (keystate[SDLK_q])
    {
        currentRot = currentRot * rota;
        cameraPos = cameraPos * rota;
    }
    if (keystate[SDLK_e])
    {
        currentRot = currentRot * rotc;
        cameraPos = cameraPos * rotc;
    }
    if (keystate[SDLK_r])
    {
        cameraPos = {0, 0, -FOCAL};
        lightPos = {0, -0.5, -0.7};
        generateLightSample();
        currentRot = mat3(1, 0, 0, 0, 1, 0, 0, 0, 1);
    }


    if (keystate[SDLK_UP])
    {
        lightPos[2] += delta_displacement;
        generateLightSample();
    }
    if (keystate[SDLK_DOWN])
    {
        lightPos[2] -= delta_displacement;
        generateLightSample();
    }
    if (keystate[SDLK_RIGHT])
    {
        lightPos[0] += delta_displacement;
        generateLightSample();
    }
    if (keystate[SDLK_LEFT])
    {
        lightPos[0] -= delta_displacement;
        generateLightSample();
    }

    if (keystate[SDLK_PAGEUP])
    {
        lightPos[1] -= delta_displacement;
        generateLightSample();
    }

    if (keystate[SDLK_PAGEDOWN])
    {
        lightPos[1] += delta_displacement;
        generateLightSample();
    }
}


void draw()
{
	int num_complete = 0;
	int num_required = TRUE_SCREEN_HEIGHT * TRUE_SCREEN_WIDTH;
	int percent_complete = 0;
	std::cout << "percent complete: 0%";
#pragma omp parallel for schedule(dynamic,16)
    for (int y = 0; y < SCREEN_HEIGHT; y += SSAA)
    {
        for (int x = 0; x < SCREEN_WIDTH; x += SSAA)
        {
            vec3 colour(0.0, 0.0, 0.0);
            for (int i = 0; i < SSAA; ++i)
            {
                for (int j = 0; j < SSAA; ++j)
                {
                    vec3 rayDir;
                    getRayDirection(x + i, y + j, rayDir);
                    rayDir = rayDir * currentRot;
                    Intersection closest;
                    vec3 partial_colour(0.0, 0.0, 0.0);

                    if (closestIntersection(cameraPos, rayDir, triangles, num_triangles, closest))
                    {
						vec3 direct = directLight(closest, triangles[closest.triangleIndex], triangles, num_triangles);
						//if (direct == vec3(0.f, 0.f, 0.f))
						//{
						//	partial_colour = gather(closest.position, direct, colour);
						//	//cout << partial_colour.r << " " << partial_colour.g << " " << partial_colour.b << std::endl;
						//}
						//else 
						partial_colour = direct * indirectLight * triangles[closest.triangleIndex].color + gather(closest.position, triangles[closest.triangleIndex].normal);
                    }

                    colour += partial_colour;
                }
            }
            colour /= (SSAA * SSAA);

            PutPixelSDL(screen, x / SSAA, y / SSAA, colour);
			// Just for visual cues
			num_complete++;
			if (num_complete > (percent_complete * num_required / 100))
			{
				SDL_UpdateRect(screen, 0, 0, 0, 0);
				percent_complete++;
				if (percent_complete > 10) std::cout << "\b";
				std::cout << "\b\b" << percent_complete << "%";
				if (percent_complete == 100) std::cout << std::endl;
			}
        }
    }

    if (SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);
    //SDL_UpdateRect(screen, 0, 0, 0, 0);
}

void generateLightSample() {
	srand(time(0));
    lightSample[0] = lightPos;
    #pragma omp parallel for
    for (int i = 1; i < SOFT_SHADOW_SAMPLES; ++i)
    {
        float x = glm::linearRand(-SOFT_SHADOW_MAX_OFFSET, SOFT_SHADOW_MAX_OFFSET);
        float y = glm::linearRand(-SOFT_SHADOW_MAX_OFFSET, SOFT_SHADOW_MAX_OFFSET);
        float z = glm::linearRand(-SOFT_SHADOW_MAX_OFFSET, SOFT_SHADOW_MAX_OFFSET);
        lightSample[i] = glm::vec3(lightPos[0] + x, lightPos[1] + y, lightPos[2] + z);
    }
}

int main()
{
    screen = InitializeSDL(TRUE_SCREEN_WIDTH, TRUE_SCREEN_HEIGHT);
    t = SDL_GetTicks();
	/*We should use arrays, makes it a lot faster!*/
	vector<Triangle> triangles_;

    // Fill triangles with test model
    LoadTestModel(triangles_);

	num_triangles = triangles_.size();
	triangles = new Triangle[num_triangles];

	for (size_t i = 0; i < num_triangles; ++i)
	{
		triangles[i] = triangles_[i];
	}

    // Generate random light samples
    generateLightSample();

	//omp_set_num_threads(6);

	std::vector<photon_t> photons = generateMap();
	std::cout << photons.size() << std::endl;

    //do
    {
		SDL_FillRect(screen, 0, 0);
        update();
        draw();
		/*if (SDL_MUSTLOCK(screen)) SDL_LockSurface(screen);
		for (auto photon : photons)
		{
			vec2 point = convertTo2D(vec3(photon.x-cameraPos.x, photon.y-cameraPos.y, photon.z-cameraPos.z));
			//std::cout << point.x << " " << point.y << std::endl;
			PutPixelSDL(screen, point.x, point.y, photon.colour*photon.lum);
		}
		if (SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);//*/
		SDL_UpdateRect(screen, 0, 0, 0, 0);
		// For windows?
		SDL_SaveBMP(screen, "screenshot.bmp");
	} while (NoQuitMessageSDL());

	delete[] triangles;

    SDL_SaveBMP(screen, "screenshot.bmp");
    return 0;
}
