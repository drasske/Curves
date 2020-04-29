#pragma once

#include <iostream>
#include <vector>
#include <algorithm>

#include "GLEW/glew.h"
#include "SDL/SDL.h"

#include "IMGUI/imgui.h"
#include "IMGUI/imgui_impl_sdl.h"
#include "IMGUI/imgui_impl_opengl3.h"
#include "IMGUI/imgui_internal.h"

#include "Shader.h"
#include "Utility.h"

// TODO: Add mouse position as a member	

struct ControlPoint
{
	double x;
	double y;
};

// Control point addition
ControlPoint operator+(const ControlPoint& p1, const ControlPoint& p2);
ControlPoint operator-(const ControlPoint& p1, const ControlPoint& p2);
// Control point scalar multiplcation
ControlPoint operator*(const ControlPoint& p, double s);
ControlPoint operator*(double s, const ControlPoint& p);

class Project
{
public:
	Project(SDL_Window* pWindow);
	virtual ~Project();

	virtual void StartFrame();
	virtual void Display() = 0;
	virtual void Initialize() = 0;
	virtual void ProcessEvents(bool& applicationIsRunning);

	virtual void AddControlPoints() = 0;

protected:
	SDL_Window* mpWindow;
	
	std::vector<ControlPoint> mControlPoints;
	int mDimension;
	int mPreviousDimension;
	int mAlgorithm;
	ControlPoint* mSelectedPoint;
	
	Shader mShader;
	GLuint mVAO;
	GLuint mVBO;

	void ClampDimension(int max, int min);
	void DrawLine(const float x1, const float y1, const float x2, const float y2);
	void DrawLine(const std::vector<double>& points);
	void DrawLine(const ControlPoint& p1, const ControlPoint& p2);
};