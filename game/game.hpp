#pragma once

#include<iostream>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include "Map.hpp"
#include "bullet.hpp"
#include "player.hpp"
using namespace std;

class Game
{
public:
	Game();

	void init(char* title,int xpos,int ypos,int width,int height);			//Initialize the game window for first time..
	void handleEventsforServer();	
	void handleEventsforClient();	
	void eventsFromClient(int16_t array[]);
	void eventsFromServer(int16_t array[]);

	void update();
	void render();				//Change the screen being rendered and updates it.
	void clean();				//Once the game is over destroys the screen and renderer

	bool running() {return isRunning;}
	bool isRunning = false;
	int key_pressed = 0;		//number of key pressed.

	short send_event[7];	
	SDL_Window *window = nullptr;
	SDL_Renderer *renderer = nullptr;

	SDL_Texture* Message;
	SDL_Rect Message_rect;

	TTF_Font* Font; 

	player *player1 = nullptr;
	player *player2 = nullptr;
	Map* maze = nullptr;
	deque<bullet*> all_bullets;
};

Game::Game()
{
	
}

void Game::init(char* title,int xpos,int ypos,int width,int height)
{
	if(SDL_Init(SDL_INIT_EVERYTHING)==0)
	{
		cout<<"The game has been initialised\n";
	}
	window = SDL_CreateWindow(title,xpos,ypos,width,height+SCORE_HEIGHT,0);

	isRunning = true;
	if(window==0)
	{
		isRunning = false;
		cout<<"Window has not been created yet";
	}
	else
	{
		cout << "Window created"<<endl;
	}

	renderer = SDL_CreateRenderer(window,-1,0);
	if(renderer==0)
	{
		cout<<"Renderer has not been initialised";
		isRunning = false;
	}
	else
	{
		cout << "Renderer initialised" <<endl;
	}
	SDL_SetRenderDrawColor(renderer,128,128,128,255);

	//for text rendering.....
	if(TTF_Init()==-1) 
	{
		cout<<"Error in loading ttf";
	}
	else
	{
		cout << "TTF loaded succesfully";
	}
	Font = TTF_OpenFont("./resources/font.ttf", 30);
	SDL_Color Black = {0, 0, 0,255};
	
	string boundary;
	for(int i=0;i<(SCREEN_WIDTH-3);i++)
	{
		boundary = boundary + "-";
	}
	
	SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Font, boundary.c_str(), Black);
	Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
	SDL_FreeSurface(surfaceMessage);

	Message_rect.x = 0;  
	Message_rect.y = SCREEN_HEIGHT;
	SDL_QueryTexture(Message,nullptr,nullptr,&Message_rect.w,&Message_rect.h);

	maze = new Map(renderer);

	//for correct initialization of location of players.

	//srand(time(0));												//for random and separate initialization.
	int spawn_x = rand()%(SCREEN_WIDTH/(TILE_SIZE*2)-1);
	int spawn_y = rand()%(SCREEN_HEIGHT/(TILE_SIZE*2)-1);
	
	pair<int,int> min_location = maze->init_pos(spawn_x,spawn_y,1);
	player1 = new player(min_location.first,min_location.second,10,SCREEN_HEIGHT+20);

	spawn_x = rand()%(SCREEN_WIDTH/(TILE_SIZE*2)-1) + SCREEN_WIDTH/(TILE_SIZE*2);
	spawn_y = rand()%(SCREEN_HEIGHT/(TILE_SIZE*2)-1) + SCREEN_HEIGHT/(TILE_SIZE*2);

	pair<int,int> max_location = maze->init_pos(spawn_x,spawn_y,-1);
	player2 = new player(max_location.first,max_location.second,SCREEN_WIDTH-260,SCREEN_HEIGHT+20);
	
	string path1 = "resources/ghost1.png"; 
	player1->init(renderer,path1);

	string path2 = "resources/ghost2.png";
	player2->init(renderer,path2);	
	
	//initialization of vent.
	spawn_x = rand()%(SCREEN_WIDTH/(TILE_SIZE*2)-1);
	spawn_y = rand()%(SCREEN_HEIGHT/(TILE_SIZE*2)-1) + SCREEN_HEIGHT/(TILE_SIZE*2);

	pair<int,int> first_vent_pos,second_vent_pos;
	first_vent_pos = maze->init_pos(spawn_x,spawn_y,1);
	maze->map[first_vent_pos.first/(TILE_SIZE)][first_vent_pos.second/TILE_SIZE] = -1;

	maze->first_vent.x = first_vent_pos.first;
	maze->first_vent.y = first_vent_pos.second;

	spawn_x = rand()%(SCREEN_WIDTH/(TILE_SIZE*2)-1) + SCREEN_WIDTH/(TILE_SIZE*2);
	spawn_y = rand()%(SCREEN_HEIGHT/(TILE_SIZE*2)-1);

	second_vent_pos = maze->init_pos(spawn_x,spawn_y,-1);
	maze->map[second_vent_pos.first/TILE_SIZE][second_vent_pos.second/TILE_SIZE] = -1;
	
	maze->second_vent.x = second_vent_pos.first;
	maze->second_vent.y = second_vent_pos.second;	
}

void Game::handleEventsforServer()
{
	send_event[0] = 1;
	send_event[3] = -1;		//stores xpos of bullet if generated.
	send_event[4] = -1;		//stores ypos of bullet if generated
	send_event[5] = -1;		//stores direction of bullet if generated.
	send_event[6] = player1->health; //for termination of client.	

	SDL_Event event;
	SDL_PollEvent(&event);
	int xmove = 0;
	int ymove = 0;
	int dis = (player1->distance);

	if(event.type==SDL_QUIT) isRunning = false;
	
	if(event.type == SDL_KEYDOWN && event.key.repeat==0)	
    {
		key_pressed++;
		switch (event.key.keysym.sym)
    	{
			case SDLK_ESCAPE:	isRunning = false;
        	case SDLK_LEFT: 	player1->playerDir = 3;player1->lastDir=player1->playerDir;break;
        	case SDLK_RIGHT: 	player1->playerDir = 1;player1->lastDir=player1->playerDir;break;
        	case SDLK_UP:    	player1->playerDir = 0;player1->lastDir=player1->playerDir;break;
        	case SDLK_DOWN:  	player1->playerDir = 2;player1->lastDir=player1->playerDir;break;
			case SDLK_v:		if(event.key.repeat==0)
								{player1->destR = player1->teleport(player1->destR,maze);} break;
			case SDLK_s: 		if((player1->coins)>0)	
								{bullet* newbullet = new bullet(player1->destR.x,player1->destR.y,player1->lastDir);
			 					newbullet->init(renderer);
								all_bullets.push_back(newbullet);
								player1->coins--;
								send_event[3] = newbullet->destR.x;
								send_event[4] = newbullet->destR.y;
								send_event[5] = newbullet->direction;}
								break;
		}
	}
	
	if(event.type==SDL_KEYUP && event.key.repeat==0)
	{
		key_pressed--;
		if(key_pressed==0) player1->playerDir = -1;		//basically no movement.
	}
	
	switch(player1->playerDir)
	{
		case -1:break;
		case 0:ymove = -1*dis;break;
		case 1:xmove = dis;break;
		case 2:ymove = dis;break;
		case 3:xmove = -1*dis;break;		
	}
	player1->destR = player1->valid_move(player1->destR,xmove,ymove,maze->map);

	send_event[1] = player1->destR.x;
	send_event[2] = player1->destR.y;
}

void Game::eventsFromClient(int16_t array[])
{
	if(array[1] == 1)
	{
		player2->destR.x = array[2];
		player2->destR.y = array[3];
		cerr << "A";
		if(array[4]!=-1)				//has a bullet been shot...
		{
			bullet* newbullet = new bullet(int(array[4]),int(array[5]),int(array[6]));
			newbullet->init(renderer);
			all_bullets.push_back(newbullet);
			player2->coins--;	
		}
		cerr << "B";
		if(array[7]==0)
		{
			//isRunning = false;
		}
	}
}
void Game::handleEventsforClient()
{
	send_event[0] = 1;
	send_event[3] = -1;		//stores xpos of bullet if generated.
	send_event[4] = -1;		//stores ypos of bullet if generated
	send_event[5] = -1;		//stores direction of bullet if generated.
	send_event[6] = player2->health;	//needed for termination of server

	SDL_Event event;
	SDL_PollEvent(&event);
	int xmove = 0;
	int ymove = 0;
	int dis = (player2->distance);

	if(event.type==SDL_QUIT) isRunning = false;

	if(event.type == SDL_KEYDOWN && event.key.repeat==0)	
    {
		key_pressed++;
		switch (event.key.keysym.sym)
    	{
			case SDLK_ESCAPE:	isRunning = false;
        	case SDLK_LEFT: 	player2->playerDir = 3;player2->lastDir=player2->playerDir;break;
        	case SDLK_RIGHT: 	player2->playerDir = 1;player2->lastDir=player2->playerDir;break;
        	case SDLK_UP:    	player2->playerDir = 0;player2->lastDir=player2->playerDir;break;
        	case SDLK_DOWN:  	player2->playerDir = 2;player2->lastDir=player2->playerDir;break;
			case SDLK_v:		if(event.key.repeat==0)
								{player2->destR = player2->teleport(player2->destR,maze);} break;
			case SDLK_s: 		if((player2->coins)>0)	
								{bullet* newbullet = new bullet(player2->destR.x,player2->destR.y,player1->lastDir);
			 					newbullet->init(renderer);
								all_bullets.push_back(newbullet);
								player2->coins--;
								send_event[3] = newbullet->destR.x;
								send_event[4] = newbullet->destR.y;
								send_event[5] = newbullet->direction;}
								break;
		}
	}
	
	if(event.type==SDL_KEYUP && event.key.repeat==0)
	{
		key_pressed--;
		if(key_pressed==0) player2->playerDir = -1;		//basically no movement.
	}
	
	switch(player2->playerDir)
	{
		case -1:break;
		case 0:ymove = -1*dis;break;
		case 1:xmove = dis;break;
		case 2:ymove = dis;break;
		case 3:xmove = -1*dis;break;		
	}
	player2->destR = player2->valid_move(player2->destR,xmove,ymove,maze->map);

	send_event[1] = player2->destR.x;
	send_event[2] = player2->destR.y;
}

void Game::eventsFromServer(int16_t array[])
{
	if(array[1] == 1)
	{
		player1->destR.x = array[2];
		player1->destR.y = array[3];

		if(array[4]!=-1)
		{
			bullet* newbullet = new bullet(int(array[4]),int(array[5]),int(array[6]));
			newbullet->init(renderer);
			all_bullets.push_back(newbullet);
			player1->coins--;	
		}

		if(array[7]==0)				//client has been shot
		{
			//isRunning = false;
		}
	}
}

void Game::update()
{
	player1->touch(maze,2);			//2 for coins
	player2->touch(maze,2);

	int i=0;
	while(i<(int)all_bullets.size())
	{
		if(all_bullets[i]->move(maze->map,player1,player2) == false)
		{
			auto it = all_bullets.begin();
			(*it)->clear();
			all_bullets.pop_front();
			continue;
		}
		i++;
	}

	if(player1->health==0)
	{
		isRunning = false;
	}
	else if(player2->health==0)
	{
		isRunning = false;
	}

	player1->ForScore(Font,renderer,"PLAYER1 BULLETS : ");
	player2->ForScore(Font,renderer,"PLAYER2 BULLETS : ");
}
void Game::render()
{
	SDL_RenderClear(renderer);
	maze->DrawMap(renderer);
	SDL_RenderCopy(renderer,player1->playerTex,NULL,&(player1->destR));
	SDL_RenderCopy(renderer,player2->playerTex,NULL,&(player2->destR));
	SDL_RenderCopy(renderer, Message, NULL, &Message_rect);

	//for text in scoreboard....
	SDL_RenderCopy(renderer,player1->scoreTex,NULL,&(player1->scoreR));
	SDL_RenderCopy(renderer,player2->scoreTex,NULL,&(player2->scoreR));

	int i=0;
	int total_bullets = all_bullets.size();
	while(i<total_bullets)
	{
		SDL_RenderCopy(renderer,all_bullets[i]->bulletTex,NULL,&(all_bullets[i]->destR));
		i++;
	}

	SDL_RenderPresent(renderer);	
}

void Game::clean()
{
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();

	window = nullptr;
	renderer = nullptr;
	Message = nullptr;
	Font = nullptr; 

	player1->clean();
	player1 = nullptr;
	player2->clean();
	player2 = nullptr;
	maze->clean();
	maze = nullptr;
	
	cout<<"GAME OVER"<<endl; 
}