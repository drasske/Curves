#pragma once

#include "Project.h"

class Project2 : public Project
{
public:
	Project2(SDL_Window* pWindow);
	virtual ~Project2();

	virtual void Display();
	virtual void Initialize();
	virtual void AddControlPoints();

	virtual void ProcessEvents(bool& applicationIsRunning);

private:
	std::vector<ControlPoint> mStagePoints;

	void HandleMouseEvent(SDL_MouseButtonEvent& mouseButtonEvent);
	void DrawCurve();
	void DrawBBForm();
	double BernsteinValue(double t, int index);
	void DrawNLIForm();
	//ControlPoint NLIValue(float t, int startIndex, int endIndex);
	void GenerateNLIStagePoints(double t);
	void DrawMidpointForm();
	void GenerateMidpoints(std::vector<ControlPoint>& points);
	void DrawPolyline();
	void DrawShells(double t);
};