#include "raytracer.h"

/* ----------------------------------------------------------------------------*/
/* GLOBAL VARIABLES                                                            */

//const int SCREEN_WIDTH = 500;
//const int SCREEN_HEIGHT = 500;
SDL_Surface* screen;
int t;

//float focalLength = SCREEN_HEIGHT / 2;
//vec3 cameraPos(0,0,-2);

vector<Triangle> triangles;

/* ----------------------------------------------------------------------------*/
/* FUNCTIONS                                                                   */

void Update()
{
    // Compute frame time:
    int t2 = SDL_GetTicks();
    float dt = float(t2-t);
    t = t2;
    cout << "Render time: " << dt << " ms." << endl;
}


void Draw()
{
    // Set screen to black
    SDL_FillRect( screen, 0, 0 );

    if( SDL_MUSTLOCK(screen) ) {
        SDL_LockSurface(screen);
    }

    for( int y = 0; y < SCREEN_HEIGHT; ++y )
    {
        for(int x = 0; x < SCREEN_WIDTH; ++x )
        {
            vec3 rayDir;
            GetRayDirection(x, y, rayDir);

            Intersection closestIntersection;
            vec3 color(0.0, 0.0, 0.0);

            if(ClosestIntersection(cameraPos, rayDir, triangles, closestIntersection)) {
                color = triangles[closestIntersection.triangleIndex].color;
            }
            PutPixelSDL( screen, x, y, color);
        }
    }

    if( SDL_MUSTLOCK(screen) )
        SDL_UnlockSurface(screen);

    SDL_UpdateRect( screen, 0, 0, 0, 0 );
}

int main( int argc, char* argv[] )
{
    screen = InitializeSDL( SCREEN_WIDTH, SCREEN_HEIGHT );
    t = SDL_GetTicks();    // Set start value for timer.

    // Fill triangles with test model
    LoadTestModel( triangles );

    while( NoQuitMessageSDL() )
    {
        Update();
        Draw();
    }

    SDL_SaveBMP( screen, "screenshot.bmp" );
    return 0;
}
