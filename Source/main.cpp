#include "raytracer.h"

SDL_Surface* screen;
int t;
vector<Triangle> triangles;
const vec3 cameraPos(0, 0, -2);

void update()
{
    // Compute frame time:
    int t2 = SDL_GetTicks();
    float dt = (float) (t2-t);
    t = t2;
    cout << "Render time: " << dt << " ms.\n";
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

            Intersection closest;
            vec3 color(0.0, 0.0, 0.0);

            if (closestIntersection(cameraPos, rayDir, triangles, closest))
            {
                color = triangles[closest.triangleIndex].color;
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
