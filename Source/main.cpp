#include <iostream>
#include <glm/glm.hpp>
#include <SDL.h>
#include <stdexcept>
#include "SDLauxiliary.h"
#include "TestModel.h"

using namespace std;
using glm::vec3;
using glm::mat3;

/* ----------------------------------------------------------------------------*/
/* GLOBAL VARIABLES                                                            */

const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 500;
SDL_Surface* screen;
int t;
double colour = 0.0; bool colour_increase = true;

/* ----------------------------------------------------------------------------*/
/* FUNCTIONS                                                                   */

void Update();
void Draw();
float Interpolate_f( float start, float end, float step, float max );
void Interpolate( float start, float end, vector<float>& result );
void Interpolate_v(vec3 a, vec3 b, vector<vec3> &result);

int main( int argc, char* argv[] )
{
	screen = InitializeSDL( SCREEN_WIDTH, SCREEN_HEIGHT );
	t = SDL_GetTicks();	// Set start value for timer.

	while( NoQuitMessageSDL() )
	{
		Update();
		Draw();
	}

	SDL_SaveBMP( screen, "screenshot.bmp" );
	return 0;
}

void Update()
{
	// Compute frame time:
	int t2 = SDL_GetTicks();
	float dt = float(t2-t);
	t = t2;
	cout << "Render time: " << dt << " ms." << endl;
}

float Interpolate_f(float start, float end, float step, float max) {
	return (end - start) * (step / (max - 1)) + start;
}

void Interpolate_v(vec3 a, vec3 b, vector<vec3> &result) {
	float max_size = result.size();
	for(float i = 0; i < max_size; i++) {
		result[i].x = Interpolate_f(a.x, b.x, i, max_size);
		result[i].y = Interpolate_f(a.y, b.y, i, max_size);
		result[i].z = Interpolate_f(a.z, b.z, i, max_size);
	}
}

void Interpolate( float start, float end, vector<float>& result ) {
	float result_size = (float) result.size();

	if(result_size < 2.0) {
		throw std::invalid_argument( "Interpolate size was too small");
	}

	for( float step = 0; step < result_size; step++ ) {
			result[step] = Interpolate_f(start, end, step, result_size);
	}
}

void Draw()
{
	if( SDL_MUSTLOCK(screen) ) {
		SDL_LockSurface(screen);
	}

	// vector<float> result( 12 ); // Create a vector width 10 floats
	// Interpolate( 5, 14, result ); // Fill it with interpolated values
	// for( double i = 0; i < result.size(); i++ )
	// 	cout << result[i] << " "; // Print the result to the terminal

	vector<vec3> result( 4 );
	vec3 a(1,4,9.2);
	vec3 b(4,1,9.8);
	Interpolate_v( a, b, result );
	for( int i=0; i<result.size(); ++i )
	{
		cout << "( "
		<< result[i].x << ", "
		<< result[i].y << ", "
		<< result[i].z << " ) ";
	}

	for( int y=0; y<SCREEN_HEIGHT; ++y )
	{

		for( int x=0; x<SCREEN_WIDTH; ++x )
		{

			vec3 color( colour, 0.0, 0.0 );
			PutPixelSDL( screen, x, y, color );
		}
	}

	if( SDL_MUSTLOCK(screen) )
		SDL_UnlockSurface(screen);

	SDL_UpdateRect( screen, 0, 0, 0, 0 );
}
