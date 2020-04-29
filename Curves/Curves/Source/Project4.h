#pragma once

#include "Project.h"

class Project4 : public Project
{
public:
	Project4(SDL_Window* pWindow);
	virtual ~Project4();

	virtual void Display();
	virtual void Initialize();
	virtual void AddControlPoints();

	virtual void ProcessEvents(bool& applicationIsRunning);

private:
	void DisplayImGui();

	void HandleMouseEvent(SDL_MouseButtonEvent& mouseButtonEvent);
	void DrawSpline();

	std::vector<double> mControlPointXValues;
	std::vector<double> mControlPointYValues;

	std::vector<double> mSplineCoefficientsX;
	std::vector<double> mSplineCoefficientsY;

	std::vector<double> GenerateSplineCoefficients(const std::vector<double>& values);
	void GetControlPointXYValues();

	double InterpolateValue(const double t, const std::vector<double>& values);

	void ReduceMatrix(double** matrix, int numRows, int numCols);
	void PrintMatrix(double** matrix, int numRows, int numCols);
};