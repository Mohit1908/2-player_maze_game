#pragma once

#include<iostream>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
using namespace std;

class  Tile
{
public:

    Tile(int x,int y);

    void init(SDL_Renderer* renderer);

    SDL_Rect destR;
    SDL_Texture* tileTex;
};

Tile::Tile(int x,int y)
{
    destR.x = x;
    destR.y = y;
    destR.w = PLAYER_SIZE;
    destR.h = PLAYER_SIZE;
}

Tile::init(SDL_Renderer* renderer)
{
    string path = "./resources/tile.png";
	const char* path_array = path.c_str();

	SDL_Surface* tmpSurface = IMG_Load(path_array);
	tileTex = SDL_CreateTextureFromSurface(renderer,tmpSurface);
	SDL_FreeSurface(tmpSurface);
}
