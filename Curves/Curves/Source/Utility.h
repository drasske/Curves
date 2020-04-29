#pragma once
#include "SDL/SDL.h"

void InitChoose();

float Choose(int d, int i);

void InitImGui(SDL_Window* pWindow, SDL_GLContext context);

bool DisplayImGuiMenu(int& projectNumber);

void OpenGLClear();