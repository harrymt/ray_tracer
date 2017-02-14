#include "raytracer.h"

#define D2R(x) x*pi/180

SDL_Surface* screen;
int t;
vector<Triangle> triangles;
vec3 cameraPos(0, 0, -FOCAL);
const float delta_displacement = 0.1f;
glm::vec3 indirectLight = 0.5f * glm::vec3(1, 1, 1);
glm::vec3 lightPos(0, -0.5, -0.7);
glm::vec3 lightColor = 14.f * glm::vec3(1, 1, 1);

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
        currentRot = mat3(1, 0, 0, 0, 1, 0, 0, 0, 1);
    }


    if (keystate[SDLK_UP])
    {
        lightPos[2] += delta_displacement;
    }
    if (keystate[SDLK_DOWN])
    {
        lightPos[2] -= delta_displacement;
    }
    if (keystate[SDLK_RIGHT])
    {
        lightPos[0] += delta_displacement;
    }
    if (keystate[SDLK_LEFT])
    {
        lightPos[0] -= delta_displacement;
    }

    if (keystate[SDLK_PAGEUP])
    {
        lightPos[1] -= delta_displacement;
    }

    if (keystate[SDLK_PAGEDOWN])
    {
        lightPos[1] += delta_displacement;
    }
}


void draw()
{
    // Set screen to black
    if (SDL_MUSTLOCK(screen)) SDL_LockSurface(screen);

    for (int y = 0; y < SCREEN_HEIGHT; ++y)
    {
        for (int x = 0; x < SCREEN_WIDTH; ++x)
        {
            vec3 rayDir;
            getRayDirection(x, y, rayDir);
            rayDir = rayDir * currentRot;


            Intersection closest;
            vec3 color(0.0, 0.0, 0.0);

            if (closestIntersection(cameraPos, rayDir, triangles, closest))
            {
                color = directLight(closest, triangles[closest.triangleIndex]) * triangles[closest.triangleIndex].color;
            }

            PutPixelSDL(screen, x, y, color);
        }
    }

    if (SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);
    SDL_UpdateRect(screen, 0, 0, 0, 0);
}

int main()
{
    screen = InitializeSDL(SCREEN_WIDTH, SCREEN_HEIGHT);
    t = SDL_GetTicks();

    // Fill triangles with test model
    LoadTestModel(triangles);

    while (NoQuitMessageSDL())
    {
        update();
        draw();
    }

    SDL_SaveBMP(screen, "screenshot.bmp");
    return 0;
}
