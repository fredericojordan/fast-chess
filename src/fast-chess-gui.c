/*This source code copyrighted by Lazy Foo' Productions (2004-2015)
and may not be redistributed without written permission.*/

#include <SDL.h>
#include <stdio.h>
//#include "fast-chess.h"

int TILE_SIDE = 15;

char WHITE[]         = { 0xFF, 0xFF, 0xFF };
char LIGHT_GRAY[]    = { 0xF0, 0xF0, 0xF0 };
char LICHESS_LIGHT[] = {  240,  217,  181 };
char LICHESS_DARK[]  = {  181,  136,   99 };

char windowTitle[] = "Chess Game";

SDL_Window* window = NULL; 			//The window we'll be rendering to
SDL_Surface* screenSurface = NULL; 	//The surface contained by the window

void paintTile(int position, char color[]) {
	int file = position%8;
	int rank = position/8;

	SDL_Rect tile;
	tile.x = file*TILE_SIDE;
	tile.y = (7-rank)*TILE_SIDE;
	tile.w = TILE_SIDE;
	tile.h = TILE_SIDE;

	SDL_FillRect( screenSurface, &tile, SDL_MapRGB( screenSurface->format, color[0], color[1], color[2] ) );
}

void paintTiles(char light_color[], char dark_color[]) {
	SDL_FillRect( screenSurface, NULL, SDL_MapRGB( screenSurface->format, light_color[0], light_color[1], light_color[2] ) );

	int file, rank;
	for (rank=0; rank<8; rank++)
		for (file=0; file<8; file+=2)
			paintTile(rank*8 + file + (rank%2), dark_color);

	SDL_UpdateWindowSurface( window );
}

int main( int argc, char* args[] )
{
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
	}
	else
	{
		window = SDL_CreateWindow( windowTitle, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 8*TILE_SIDE, 8*TILE_SIDE, SDL_WINDOW_SHOWN );

		if( window == NULL ) {
			printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
		} else {
			screenSurface = SDL_GetWindowSurface( window );

//			paintTiles(WHITE, LIGHT_GRAY);
			paintTiles(LICHESS_LIGHT, LICHESS_DARK);

			SDL_Delay( 2000 );
		}
	}

	SDL_DestroyWindow( window );
	SDL_Quit();

	return 0;
}
