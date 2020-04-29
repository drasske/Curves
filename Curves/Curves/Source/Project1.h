#pragma once

#include "Project.h"

class Project1 : public Project
{
public:
	Project1(SDL_Window* pWindow);
	virtual ~Project1();

	virtual void Display();
	virtual void Initialize();
	virtual void AddControlPoints();

private:
	std::vector<double> mYaxis;
	std::vector<double> mXaxis;

	void DrawGraph();
	void DrawBBForm();
	double BernsteinValue(double t);
	void DrawNLIForm();
	double NLIValue(double t);

};