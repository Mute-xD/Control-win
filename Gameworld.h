#pragma once
#include"stdafx.h"
#include"Status.h"

#define MESSAGE_FONTSIZE 20


class Gameworld
{
public:
	Gameworld(Status* status);
	~Gameworld();

private:
	SDL_Window* window = nullptr;
	Status* status = nullptr;
	UINT32 fps = 60;
	UINT32 fpsTimer = 0;
	SDL_Event event;
	SDL_Renderer* renderer = nullptr;
	TTF_Font* fontMessage = nullptr;
	SDL_Texture* textureMessage = nullptr;
	SDL_Surface* surfaceMessage = nullptr;

	SDL_Color white = { 255, 255, 255 };
	SDL_Color black = { 0, 0, 0 };
	SDL_Rect* rect = nullptr;

	void looper();
	void eventProcess();
	void FPSLimiter();
	void messageRender();
};

