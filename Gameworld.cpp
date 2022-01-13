#include "stdafx.h"
#include "Gameworld.h"

Gameworld::Gameworld(Status* status)
{
	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow("Drone", 50, 50,
		1280, 720, SDL_WINDOW_RESIZABLE);
	if (!window)
	{
		status->msg->insert(std::string("Could not create window: ")+ SDL_GetError());
	}

	this->status = status;

	this->renderer = SDL_CreateRenderer(this->window, -1, SDL_RENDERER_SOFTWARE|SDL_RENDERER_PRESENTVSYNC);
	if (!renderer)
		status->msg->insert(std::string("Render Failure! ") + SDL_GetError());
	rect = new SDL_Rect();
	TTF_Init();
	fontMessage = TTF_OpenFont("C:/Windows/Fonts/msyhbd.ttc", MESSAGE_FONTSIZE);
	if (!fontMessage)
		status->msg->insert(std::string("TTF Open Font Failure!") + TTF_GetError());
	TTF_SetFontStyle(fontMessage, TTF_STYLE_BOLD);
	SDL_Surface* surface = new SDL_Surface();
	looper();
}

Gameworld::~Gameworld()
{
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void Gameworld::looper()
{
	while (!status->isExit)
	{
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		eventProcess();

		messageRender();
		//
		SDL_RenderPresent(renderer);
		FPSLimiter();
	}
}

void Gameworld::eventProcess()
{
	while (SDL_PollEvent(&event) == 1)
	{
		if (event.type == SDL_QUIT)
			status->isExit = true;
	}
}
	
void Gameworld::FPSLimiter()
{
	if (SDL_GetTicks() - fpsTimer < fps)
	{
		SDL_Delay(fps - SDL_GetTicks() + fpsTimer);
	}
	fpsTimer = SDL_GetTicks();
}

void Gameworld::messageRender()
{
	int x = 400; int y = 400;
	for (int i = 0; i < status->msg->size(); i++)
	{
		const char* msgChar = status->msg->at(i).c_str();
		surfaceMessage = TTF_RenderText_Blended(fontMessage, msgChar, white);
		TTF_SizeText(fontMessage, msgChar, &rect->w, &rect->h);
		rect->x = x - rect->w / 2;
		rect->y = y - rect->h / 2;
		textureMessage = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
		SDL_RenderCopy(renderer, textureMessage, NULL, rect);
		
		SDL_FreeSurface(surfaceMessage);
		SDL_DestroyTexture(textureMessage);

		y += rect->h;
	}
}
