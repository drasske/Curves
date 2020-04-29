#include "Project3.h"

Project3::Project3(SDL_Window * pWindow) : Project(pWindow)
{
}

Project3::~Project3()
{
}

void Project3::Display()
{
	// Project ImGuiWindow
	DisplayImGui();

	// Get mouse position
	Uint32 mouseState;
	int mouseX, mouseY;
	mouseState = SDL_GetMouseState(&mouseX, &mouseY);
	float openGLMouseX = ((float)mouseX / 400.0f) - 1.0f;
	float openGLMouseY = -(((float)mouseY / 300.0f) - 1.0f);
	// Move the point arounds
	if (mSelectedPoint != nullptr)
	{
		mSelectedPoint->y = openGLMouseY;
		mSelectedPoint->x = openGLMouseX;
		// Clamp x and y values
		float xClampValue = 0.98f;
		if (mSelectedPoint->x > xClampValue)
		{
			mSelectedPoint->x = xClampValue;
		}
		else if (mSelectedPoint->x < -xClampValue)
		{
			mSelectedPoint->x = -xClampValue;
		}
		float yClampTop = 0.7f;
		float yClampBottom = -0.97f;
		if (mSelectedPoint->y > yClampTop)
		{
			mSelectedPoint->y = yClampTop;
		}
		else if (mSelectedPoint->y < yClampBottom)
		{
			mSelectedPoint->y = yClampBottom;
		}
	}

	// Set VBO data for user moveable points
	if (!mControlPoints.empty())
	{
		glBindBuffer(GL_ARRAY_BUFFER, mVBO);
		glBufferData(GL_ARRAY_BUFFER, mControlPoints.size() * sizeof(ControlPoint), &mControlPoints[0], GL_STATIC_DRAW);

		// Bind buffers 
		mShader.Bind();
		mShader.SetUniform4f("u_Color", 0.1f, 0.1f, 0.5f, 1.0f);
		glBindVertexArray(mVAO);

		// Draw control points
		glPointSize(10);
		glDrawArrays(GL_POINTS, 0, mControlPoints.size());
	}

	// Draw the polynomial
	DrawPolynomial();

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	SDL_GL_SwapWindow(mpWindow);
}

void Project3::Initialize()
{
}

void Project3::AddControlPoints()
{
}

void Project3::ProcessEvents(bool & applicationIsRunning)
{
	// Process events
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		ImGui_ImplSDL2_ProcessEvent(&event);
		switch (event.type)
		{
		case SDL_QUIT:
			applicationIsRunning = false;
			break;
		case SDL_WINDOWEVENT:
			if (event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(mpWindow))
			{
				applicationIsRunning = false;
			}
			break;
		case SDL_KEYUP:
			if (event.key.keysym.sym == SDLK_ESCAPE)
			{
				applicationIsRunning = false;
			}
			break;
		case SDL_MOUSEBUTTONDOWN:
			HandleMouseEvent(event.button);
			break;
		case SDL_MOUSEBUTTONUP:
			HandleMouseEvent(event.button);
			break;
		}
	}
}

void Project3::DisplayImGui()
{
	// Choose algorithm
	ImGui::Text("Newton Form For Polynomial Interpolation");

	if (ImGui::Button("Reset"))
	{
		// Reset control points
		mControlPoints.clear();
	}

	// Clamp dimension
	ClampDimension(1, 20);

	ImGui::End();

	// Render ImGui
	ImGui::Render();
}

void Project3::HandleMouseEvent(SDL_MouseButtonEvent & mouseButtonEvent)
{
	// Get the current mouse state
	Uint32 mouseState;
	int mouseX, mouseY;
	mouseState = SDL_GetMouseState(&mouseX, &mouseY);
	float openGLMouseX = ((float)mouseX / 400.0f) - 1.0f;
	float openGLMouseY = -(((float)mouseY / 300.0f) - 1.0f);

	if (mouseButtonEvent.button == SDL_BUTTON_LEFT)
	{
		if (mouseButtonEvent.state == SDL_PRESSED)
		{
			// Check if any control points are nearby
			// If not, create a new control point
			bool createNewPoint = true;
			// Don't create more than 20 points
			if (mControlPoints.size() >= 20)
			{
				createNewPoint = false;
			}

			if (openGLMouseY > 0.72f)
			{
				createNewPoint = false;
			}

			// Check mouse distance from every control point
			for (auto& currentPoint : mControlPoints)
			{
				double xRange = 0.03;
				double yRange = 0.03;
				if (openGLMouseX < currentPoint.x + xRange && openGLMouseX > currentPoint.x - xRange && openGLMouseY < currentPoint.y + yRange && openGLMouseY > currentPoint.y - yRange)
				{
					mSelectedPoint = &currentPoint;
					createNewPoint = false;
					break;
				}
			}

			// Create a new point if it should be done
			if (createNewPoint)
			{
				ControlPoint toAdd{ openGLMouseX, openGLMouseY };
				mControlPoints.push_back(toAdd);
				mSelectedPoint = &mControlPoints[mControlPoints.size() - 1];
			}
		}
		// Remove selected point if left is not being held
		else if (mouseButtonEvent.state == SDL_RELEASED)
		{
			mSelectedPoint = nullptr;
		}
	}
}

void Project3::DrawPolynomial()
{
	GetControlPointXYValues();

	mNewtonFormCoefficientsX = GenerateNewtonFormCoefficients(mControlPointXValues);
	mNewtonFormCoefficientsY = GenerateNewtonFormCoefficients(mControlPointYValues);
	// Loop through however many points to get x and y values
	std::vector<ControlPoint> graphPoints;
	double numberOfPoints = 300;
	for (double pointIndex = 0; pointIndex <= numberOfPoints; ++pointIndex)
	{
		double t = pointIndex*(mControlPoints.size()-1) / numberOfPoints;
		// Create a new control point at p(t)
		ControlPoint polynomialPoint;
		polynomialPoint.x = InterpolateXValue(t);
		polynomialPoint.y = InterpolateYValue(t);
		graphPoints.push_back(polynomialPoint);
	}

	// Put into OpenGL
	if (!graphPoints.empty())
	{
		glLineWidth(2);
		mShader.SetUniform4f("u_Color", 0.0f, 0.0f, 0.0f, 1.0f);
		glBufferData(GL_ARRAY_BUFFER, graphPoints.size() * sizeof(ControlPoint), &graphPoints[0], GL_STATIC_DRAW);
		glDrawArrays(GL_LINE_STRIP, 0, graphPoints.size());
	}
}

std::vector<double> Project3::GenerateNewtonFormCoefficients(const std::vector<double>& values)
{
	std::vector<double> dividedDifferenceValues = values;
	
	// Start adding coefficient values
	int numberOfPointsToAdd = mControlPoints.size() - 1;
	int offset = 0;
	double startingIndex = 1.0;
	while (numberOfPointsToAdd > 0)
	{
		// Add the new set of coefficient values
		for (int index = 0; index < numberOfPointsToAdd; ++index)
		{
			// Calculate value of new point
			double dividedDifferenceValue = (dividedDifferenceValues[index + offset + 1] - dividedDifferenceValues[index + offset]);
			dividedDifferenceValue /= (startingIndex);
			dividedDifferenceValues.push_back(dividedDifferenceValue);
			//std::cout << startingIndex + index << " - " << index << std::endl;
		}
		offset += numberOfPointsToAdd + 1;
		++startingIndex;
		--numberOfPointsToAdd;
	}
	//std::cout << std::endl;
	// Trim the divided difference values
	std::vector<double> coefficients;
	int coefficientIndex = 0;
	int numberOfPoints = values.size();
	for (int index = 0; index < numberOfPoints; ++index)
	{
		coefficients.push_back(dividedDifferenceValues[coefficientIndex]);
		coefficientIndex += (numberOfPoints - index);
	}
	return coefficients;
}

void Project3::GetControlPointXYValues()
{
	mControlPointXValues.clear();
	mControlPointYValues.clear();
	// Loop through all the control points
	for (auto& point : mControlPoints)
	{
		mControlPointXValues.push_back(point.x);
		mControlPointYValues.push_back(point.y);
	}
}

double Project3::InterpolateXValue(double t)
{
	double output = 0.0f;
	// Loop through coefficients and multiply by t value
	double tvalue = 1.0f;
	for (size_t coefficientIndex = 0; coefficientIndex < mNewtonFormCoefficientsX.size(); ++coefficientIndex)
	{
		double coefficientValue = mNewtonFormCoefficientsX[coefficientIndex];
		output += coefficientValue*tvalue;
		tvalue *= (t - (double)coefficientIndex);
	}
	return output;
}

double Project3::InterpolateYValue(double t)
{
	double output = 0.0f;
	// Loop through coefficients and multiply by t value
	double tvalue = 1.0f;
	for (size_t coefficientIndex = 0; coefficientIndex < mNewtonFormCoefficientsY.size(); ++coefficientIndex)
	{
		double coefficientValue = mNewtonFormCoefficientsY[coefficientIndex];
		output += coefficientValue * tvalue;
		tvalue *= (t - (double)coefficientIndex);
	}
	return output;
}
