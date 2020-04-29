#include "Project4.h"

Project4::Project4(SDL_Window * pWindow) : Project(pWindow)
{
}

Project4::~Project4()
{
}

void Project4::Display()
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

	// Draw the spline
	DrawSpline();

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	SDL_GL_SwapWindow(mpWindow);
}

void Project4::Initialize()
{
}

void Project4::AddControlPoints()
{
}

void Project4::ProcessEvents(bool & applicationIsRunning)
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

void Project4::DisplayImGui()
{
	// Choose algorithm
	ImGui::Text("Interpolating Splines");

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

void Project4::HandleMouseEvent(SDL_MouseButtonEvent & mouseButtonEvent)
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

void Project4::DrawSpline()
{
	GetControlPointXYValues();

	mSplineCoefficientsX = GenerateSplineCoefficients(mControlPointXValues);
	mSplineCoefficientsY = GenerateSplineCoefficients(mControlPointYValues);
	// Loop through however many points to get x and y values
	std::vector<ControlPoint> graphPoints;
	double numberOfPoints = 300;
	for (double pointIndex = 0; pointIndex <= numberOfPoints; ++pointIndex)
	{
		double t = pointIndex * (mControlPoints.size() - 1) / numberOfPoints;
		// Create a new control point at p(t)
		ControlPoint polynomialPoint;
		polynomialPoint.x = InterpolateValue(t, mSplineCoefficientsX);
		polynomialPoint.y = InterpolateValue(t, mSplineCoefficientsY);
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

std::vector<double> Project4::GenerateSplineCoefficients(const std::vector<double>& values)
{
	// Set the normal coefficients a0, a1, a2, a3



	// Create each row of the linear system for integer t values
	// Get number of points
	const size_t numPoints = values.size();
	const size_t numRows = numPoints + 2;
	const size_t numCols = numRows + 1;

	// Create the linear system for the values
	double** matrix = new double*[numRows];
	for (unsigned int i = 0; i < numCols; ++i)
	{
		matrix[i] = new double[numCols];
	}

	// Create each non derivative row
	for (size_t rowIndex = 0; rowIndex < numPoints; ++rowIndex)
	{
		// Get the t-value for the row
		double tvalue = (double)rowIndex;
		std::vector<double> row;

		// Add to the row, non-truncated power function
		for (size_t coefIndex = 0; coefIndex < 4; ++coefIndex)
		{
			double power = (double)coefIndex;
			double value = 1.0 * pow(tvalue, power);
			row.push_back(value);
		}

		// Then add truncated power function values
		for (size_t coefIndex = 4; coefIndex < numRows; ++coefIndex)
		{
			double c = coefIndex - 3.0;
			double value = 0.0;
			if (tvalue > c)
			{
				value = pow((tvalue - c), 3.0);
			}
			row.push_back(value);
		}

		// Finally add the actual point output value
		row.push_back(values[rowIndex]);

		// Add the row to the matrix
		for (unsigned int colIndex = 0; colIndex < numCols; ++colIndex)
		{
			matrix[rowIndex][colIndex] = row[colIndex];
		}
	}

	// Add derivative conditions

	size_t rowIndex = numRows - 2;
	// First derivative condition f''(0) = 0
	for (unsigned int colIndex = 0; colIndex < numCols; ++colIndex)
	{
		matrix[rowIndex][colIndex] = 0.0;
		if (colIndex == 2)
		{
			matrix[rowIndex][colIndex] = 2.0;
		}
	}

	// Second derivative condition f''(k) = 0
	++rowIndex;
	matrix[rowIndex][0] = 0.0;
	matrix[rowIndex][1] = 0.0;
	matrix[rowIndex][2] = 2.0;
	double maxT = numPoints;
	for (unsigned int colIndex = 3; colIndex < (numCols-1); ++colIndex)
	{
		double value = 6.0;
		double offset = colIndex - 2.0;
		value *= (maxT-offset);
		matrix[rowIndex][colIndex] = value;
		//6*t, 6*(t-1), ... 6*t-(k-1))
	}
	matrix[numRows-1][numCols-1] = 0.0;

	// Reduce the matrix to get coefficient values
	//PrintMatrix(matrix, numRows, numCols);
	ReduceMatrix(matrix, numRows, numCols);
	//PrintMatrix(matrix, numRows, numCols);

	// Solve the matrix, return the last column
	std::vector<double> coefficients;
	for (unsigned int rowIndex = 0; rowIndex < numRows; ++rowIndex)
	{
		double coefValue = matrix[rowIndex][numCols - 1];
		coefficients.push_back(coefValue);
	}

	// Delete the matrix here
	for (unsigned int i = 0; i < numRows; ++i)
	{
		delete[] matrix[i];
	}
	//delete matrix;

	return coefficients;
}

void Project4::GetControlPointXYValues()
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

double Project4::InterpolateValue(const double t, const std::vector<double>& values)
{
	double output = 0.0f;
	// Loop through coefficients and multiply by t value
	double tvalue = 1.0f;

	// Add the values for the non-truncated power function part
	// a0 + a1*t + a2*t^2 + a3*t^3
	output += values[0];
	output += t * values[1];
	if (!(values.size() > 2))
	{
		return 0.0;
	}
	output += t * t * values[2];
	if (!(values.size() > 3))
	{
		return 0.0;
	}
	output += t * t * t * values[3];

	// TODO: CHANGE THIS TO USE PROPER SPLINE FUNCTIOn
	// Add the values for the truncated power functions
	for (size_t coefIndex = 4; coefIndex < values.size(); ++coefIndex)
	{
		double value = 0.0;
		double c = coefIndex - 3;
		if (t > c)
		{
			value = values[coefIndex] * pow((t - c), 3.0);
		}
		output += value;
	}

	return output;
}

void Project4::ReduceMatrix(double** matrix, int numRows, int numCols)
{
	int lead = 0;

	while (lead < numRows)
	{
		double d, m;
		for (int r = 0; r < numRows; ++r)
		{
			d = matrix[lead][lead];
			m = matrix[r][lead] / matrix[lead][lead];

			for (int c = 0; c < numCols; c++)
			{
				if (r == lead)
				{
					matrix[r][c] /= d;
				}
				else
				{
					matrix[r][c] -= matrix[lead][c] * m;
				}
			}
		}
		++lead;
	}
}

void Project4::PrintMatrix(double ** matrix, int numRows, int numCols)
{
	for (int i = 0; i < numRows; ++i)
	{
		for (int j = 0; j < numCols; ++j)
		{
			std::cout << matrix[i][j] << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}
