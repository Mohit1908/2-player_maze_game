#pragma once
const int SCREEN_WIDTH = 1050;
const int SCREEN_HEIGHT = 750;
const int SCORE_HEIGHT = 70;
const int PLAYER_SIZE = 25;
const int TILE_SIZE = 50;
const int BULLET_SIZE = 15;
const int COIN_SIZE = 25;
const int NORTH = 0;
const int EAST = 1;
const int SOUTH = 2;
const int WEST = 3;
const int MAX_PLAYERS = 2;
const int BUF_MAX = 256;
const int FRAME_TIME = 16666;
const int TAB_SIZE = 10;
const int INVISIBLE_TIME = 300;     //5 seconds.
int TIME = 0;
int FPS = 60;
int frameDelay = 1000/FPS;
int frameStart;
int frameTime,difference;
 