#pragma once

#include "Project.h"

class Project3 : public Project
{
public:
	Project3(SDL_Window* pWindow);
	virtual ~Project3();

	virtual void Display();
	virtual void Initialize();
	virtual void AddControlPoints();

	virtual void ProcessEvents(bool& applicationIsRunning);

private:
	void DisplayImGui();

	void HandleMouseEvent(SDL_MouseButtonEvent& mouseButtonEvent);
	void DrawPolynomial();

	std::vector<double> mControlPointXValues;
	std::vector<double> mControlPointYValues;

	std::vector<double> mNewtonFormCoefficientsX;
	std::vector<double> mNewtonFormCoefficientsY;

	std::vector<double> GenerateNewtonFormCoefficients(const std::vector<double>& values);
	void GetControlPointXYValues();

	double InterpolateXValue(double t);
	double InterpolateYValue(double t);
};