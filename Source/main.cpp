#include <iostream>
#include <glm/glm.hpp>
#include <SDL.h>
#include <stdexcept>
#include <cstdlib>
#include <ctime>
#include "SDLauxiliary.h"
#include "TestModel.h"

using namespace std;
using glm::vec2;
using glm::vec3;
using glm::mat3;

/* ----------------------------------------------------------------------------*/
/* GLOBAL VARIABLES                                                            */

const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 500;
SDL_Surface* screen;
int t;

vec3 topLeft(1,0,0);     // red
vec3 topRight(0,0,1);    // blue
vec3 bottomRight(0,1,0); // green
vec3 bottomLeft(1,1,0);  // yellow

vector<vec3> leftSide( SCREEN_HEIGHT );
vector<vec3> rightSide( SCREEN_HEIGHT );

vector<vec3> colours(SCREEN_HEIGHT);

bool stars_increasing = false;
vector<vec3> stars( 1000 );


/* ----------------------------------------------------------------------------*/
/* FUNCTIONS                                                                   */

void Update();
void Draw();
float Interpolate_f( float start, float end, float step, float max );
void Interpolate( float start, float end, vector<float>& result );
void Interpolate_v(vec3 a, vec3 b, vector<vec3> &result);
void updateStars();
int rand_i(int min, int max);
float rand_f();
glm::vec2 convertTo2D(vec3 coords);

int main( int argc, char* argv[] )
{
	screen = InitializeSDL( SCREEN_WIDTH, SCREEN_HEIGHT );
	t = SDL_GetTicks();	// Set start value for timer.

	// Seed for random numbers
	srand (static_cast <unsigned> (time(0)));

	Interpolate_v( topLeft, bottomLeft, leftSide );
	Interpolate_v( topRight, bottomRight, rightSide );

	updateStars();

	while( NoQuitMessageSDL() )
	{
		Update();
		Draw();
	}

	SDL_SaveBMP( screen, "screenshot.bmp" );
	return 0;
}

float rand_f(float min, float max) {
	return min + float(rand()) / float(RAND_MAX/(max - min));
}

void updateStars() {
	for(size_t i = 0; i < stars.size(); i++) {
		float x = rand_f(-1.0, 1.0);
		float y = rand_f(-1.0, 1.0);
		float z = rand_f(0.0, 1.0);
		stars[i] = vec3(x, y, z);
	}
}

void Update()
{
	// Compute frame time:
	int t2 = SDL_GetTicks();
	float dt = float(t2-t);
	t = t2;
	cout << "Render time: " << dt << " ms." << endl;
}

// HOW TO RUN
//	for( float step = 0; step < result_size; step++ ) {
//			result[step] = Interpolate_f(start, end, step, result_size);
//	}
float Interpolate_f(float start, float end, float step, float max) {
	return (end - start) * (step / (max - 1)) + start;
}


// HOW TO RUN
// vector<vec3> result( 4 );
// 	vec3 a(1,4,9.2);
// 	vec3 b(4,1,9.8);
// 	Interpolate_v( a, b, result );
// 	for( int i=0; i<result.size(); ++i )
// 	{
// 		cout << "( "
// 		<< result[i].x << ", "
// 		<< result[i].y << ", "
// 		<< result[i].z << " ) ";
// 	}
void Interpolate_v(vec3 a, vec3 b, vector<vec3> &result) {
	float max_size = result.size();
	for(float i = 0; i < max_size; i++) {
		result[i].x = Interpolate_f(a.x, b.x, i, max_size);
		result[i].y = Interpolate_f(a.y, b.y, i, max_size);
		result[i].z = Interpolate_f(a.z, b.z, i, max_size);
	}
}

// HOW TO RUN:
// vector<float> result( 12 ); // Create a vector width 10 floats
// Interpolate( 5, 14, result ); // Fill it with interpolated values
// for( double i = 0; i < result.size(); i++ )
// 	cout << result[i] << " "; // Print the result to the terminal
void Interpolate( float start, float end, vector<float>& result ) {
	float result_size = (float) result.size();

	if(result_size < 2.0) {
		throw std::invalid_argument( "Interpolate size was too small");
	}

	for( float step = 0; step < result_size; step++ ) {
			result[step] = Interpolate_f(start, end, step, result_size);
	}
}

vec2 convertTo2D(vec3 coords) {
		float f = SCREEN_HEIGHT / 2;
		float u = f * coords.x / coords.z + SCREEN_WIDTH / 2;
		float v = f * coords.y / coords.z + SCREEN_HEIGHT / 2;
		return vec2(u, v);
}

void Draw()
{
	// Set screen to black
	SDL_FillRect( screen, 0, 0 );

	if( SDL_MUSTLOCK(screen) ) {
		SDL_LockSurface(screen);
	}

	// // TODO: future, add #includes and parralise loops like e.g.
	// // #pragma omp parallel for with Open MP
	// for( int y=0; y < SCREEN_HEIGHT; ++y )
	// {
	// 	Interpolate_v(leftSide[y], rightSide[y], colours);
	//
	// 	for(int x = 0; x < SCREEN_WIDTH; ++x )
	// 	{
	// 		PutPixelSDL( screen, x, y, colours[x] );
	// 	}
	// }

	for( size_t s = 0; s < stars.size(); ++s ) {
		// Move the stars
		double velocity = 0.001;
		if(stars[s].z < 1) { stars[s].z += velocity; } else { stars[s].z = 0.0; }

		// Code for projecting and drawing each star
		vec3 color = 0.2f * vec3(1,1,1) / (stars[s].z * stars[s].z);
		vec2 star = convertTo2D(stars[s]);
		PutPixelSDL( screen, star.x, star.y, color );
	}

	if( SDL_MUSTLOCK(screen) )
		SDL_UnlockSurface(screen);

	SDL_UpdateRect( screen, 0, 0, 0, 0 );
}
