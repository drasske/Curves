#include "Project2.h"

Project2::Project2(SDL_Window * pWindow) : Project(pWindow)
{
}

Project2::~Project2()
{
}

void Project2::Display()
{
	// Project ImGui Window
	
	// Choose algorithm
	ImGui::RadioButton("BB Form", &mAlgorithm, 0); ImGui::SameLine();
	ImGui::RadioButton("NLI", &mAlgorithm, 1); ImGui::SameLine();
	ImGui::RadioButton("Midpoint", &mAlgorithm, 2); ImGui::SameLine();

	// Show polyline option
	static bool showPolyline = false;
	ImGui::Checkbox("Polyline", &showPolyline); ImGui::SameLine();
			
	// Enable shell checkbox if NLI is selected
	static bool showShells = false;

	if(mAlgorithm != 1)
	{
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		showShells = false;
	}
		
	ImGui::Checkbox("Shells", &showShells); ImGui::SameLine();
		
	if (mAlgorithm != 1)
	{
		ImGui::PopStyleVar();
		ImGui::PopItemFlag();
	}

	// T value slider
	ImGui::PushItemWidth(100);
	static float tValue = 0.5f;
	ImGui::SliderFloat("T", &tValue, 0.0f, 1.0f);
	ImGui::PopItemWidth();

	if (ImGui::Button("Reset"))
	{
		// Reset control points
		mControlPoints.clear();
		tValue = 0.5f;
	}

	// Clamp dimension
	ClampDimension(1, 20);

	ImGui::End();
	
	// Render ImGui
	ImGui::Render();

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

	// Draw the curve
	DrawCurve();

	// Draw polyline
	if (showPolyline)
	{
		DrawPolyline();
	}

	// Draw shells
	if (showShells)
	{
		DrawShells(tValue);
		// Draw stage points
		if (!mStagePoints.empty())
		{
			glBufferData(GL_ARRAY_BUFFER, mStagePoints.size() * sizeof(ControlPoint), &mStagePoints[0], GL_STATIC_DRAW);
			glPointSize(5);
			glDrawArrays(GL_POINTS, 0, mStagePoints.size());
		}
	}

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	SDL_GL_SwapWindow(mpWindow);
}

void Project2::Initialize()
{
}

void Project2::AddControlPoints()
{

}

void Project2::ProcessEvents(bool & applicationIsRunning)
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

void Project2::HandleMouseEvent(SDL_MouseButtonEvent & mouseButtonEvent)
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
				float xRange = 0.03f;
				float yRange = 0.03f;
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

void Project2::DrawCurve()
{
	switch (mAlgorithm)
	{
	case 0:
		DrawBBForm();
		break;
	case 1:
		DrawNLIForm();
		break;
	case 2:
		DrawMidpointForm();
		break;
	}
}

void Project2::DrawBBForm()
{
	double numberOfPoints = 128;
	// Add however many points to the graph
	std::vector<double> graphPoints;
	for (double currentPoint = 0; currentPoint <= numberOfPoints; ++currentPoint)
	{
		// Do the actual point calculation here
		double t = currentPoint / numberOfPoints;
	
		double totalValX = 0;
		double totalValY = 0;
		// Compute the BB function value for each t value
		for (size_t index = 0; index < mControlPoints.size(); ++index)
		{
			double value = BernsteinValue(t, index);
			ControlPoint* pControlPoint = &mControlPoints[index];
			totalValX += pControlPoint->x * value;
			totalValY += pControlPoint->y * value;
		}

		// Add the point to the graph
		graphPoints.push_back(totalValX);
		graphPoints.push_back(totalValY);
	}

	// Put into OpenGL
	if (!graphPoints.empty())
	{
		glLineWidth(2);
		mShader.SetUniform4f("u_Color", 0.0f, 0.0f, 0.0f, 1.0f);
		glBufferData(GL_ARRAY_BUFFER, graphPoints.size() * sizeof(double), &graphPoints[0], GL_STATIC_DRAW);
		glDrawArrays(GL_LINE_STRIP, 0, graphPoints.size() / 2);
	}
}

double Project2::BernsteinValue(double t, int index)
{
	double output = 0.0f;
	int numberOfControlPoints = mControlPoints.size() - 1;
	double coefficient = Choose(numberOfControlPoints, index);
	output = coefficient * pow((1.0 - t), (double)(numberOfControlPoints - index)) * pow(t, (double)index);
	return output;
}

void Project2::DrawNLIForm()
{
	// CURVE[0, d](T) = (1 - t)CURVE[0, d-1](t) + tCURVE[1, d](t);
	// Base case CURVE[0, 1](t) = (1-t)P0 + tP1
	double numberOfPoints = 128;
	double numberOfControlPoints = mControlPoints.size();
	// Add however many points to the graph
	std::vector<ControlPoint> graphPoints;
	for (double currentPoint = 0; currentPoint <= numberOfPoints; ++currentPoint)
	{
		// Do the actual point calculation here
		double t = currentPoint / numberOfPoints;

		GenerateNLIStagePoints(t);
		// Compute the NLI function value for each t value
		if (numberOfControlPoints > 0)
		{
			graphPoints.push_back(mStagePoints[mStagePoints.size() - 1]);
		}
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

// Generate the NLI Point based on the control points
void Project2::GenerateNLIStagePoints(double t)
{
	// Empty points
	mStagePoints.clear();

	// Add initial points
	mStagePoints = mControlPoints;

	// Start adding staged points
	int numberOfPointsToAdd = mControlPoints.size() - 1;
	int offset = 0;
	while (numberOfPointsToAdd > 0)
	{
		// Add the new set of stage points
		for (int index = 0; index < numberOfPointsToAdd; ++index)
		{
			mStagePoints.push_back((1.0 - t)*mStagePoints[index + offset] + t * (mStagePoints[index + offset + 1]));
		}
		offset += numberOfPointsToAdd + 1;
		--numberOfPointsToAdd;
	}
}

void Project2::DrawMidpointForm()
{
	// Set a k value
	int numberOfSubdivisons = 5;
	int k = numberOfSubdivisons;

	// Keep track of how many points are approximated
	int numberOfControlPoints = mControlPoints.size();
	// Add initial control points to the graph
	std::vector<ControlPoint> graphPoints = mControlPoints;
	int numberOfPoints = graphPoints.size();
	GenerateMidpoints(graphPoints);
	if (!graphPoints.empty())
	{
		// Get initial right and left halves to recurse on
		std::vector<ControlPoint> leftMidpoints(graphPoints.begin(), graphPoints.end() - graphPoints.size() / 2);
		std::vector<ControlPoint> rightMidpoints(graphPoints.end() - graphPoints.size() / 2 - 1, graphPoints.end());

		// Loop through number of subdivisions
		for (int currentSubdivision = 0; currentSubdivision < k; ++currentSubdivision)
		{
			// Split the current points into two groups
			// Will then have number of points * 2 - 1
			GenerateMidpoints(leftMidpoints);
			GenerateMidpoints(rightMidpoints);
		}

		// Combine points into one list
		graphPoints.clear();
		for (auto& point : leftMidpoints)
		{
			graphPoints.push_back(point);
		}
		for (auto& point : rightMidpoints)
		{
			graphPoints.push_back(point);
		}
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

void Project2::GenerateMidpoints(std::vector<ControlPoint>& points)
{
	// Start adding staged points
	double t = 0.5f;
	int numberOfPointsToAdd = points.size() - 1;
	int offset = 0;
	std::vector<ControlPoint> tempPoints = points;
	while (numberOfPointsToAdd > 0)
	{
		// Add the new set of stage points
		for (int index = 0; index < numberOfPointsToAdd; ++index)
		{
			tempPoints.push_back((1.0 - t)*tempPoints[index + offset] + t * (tempPoints[index + offset + 1]));
		}
		offset += numberOfPointsToAdd + 1;
		--numberOfPointsToAdd;
	}

	// Trim the intermediate points
	int pointSize = points.size();
	points.clear();
	// Push the first half
	int prevIndex = 0;
	for (int i = 0; i < pointSize; ++i)
	{
		points.push_back(tempPoints[prevIndex]);
		prevIndex += (pointSize - i);
	}
	// Push the second half
	--prevIndex;
	for (int i = pointSize - 1; i > 0; --i)
	{
		prevIndex -= (pointSize - i);
		points.push_back(tempPoints[prevIndex]);
	}
}

void Project2::DrawPolyline()
{
	if (!mControlPoints.empty())
	{
		glLineWidth(2);
		mShader.SetUniform4f("u_Color", 0.5f, 0.5f, 0.5f, 1.0f);
		glBufferData(GL_ARRAY_BUFFER, mControlPoints.size() * sizeof(ControlPoint), &mControlPoints[0], GL_STATIC_DRAW);
		glDrawArrays(GL_LINE_STRIP, 0, mControlPoints.size());
	}
}

void Project2::DrawShells(double t)
{
	if (mStagePoints.size() > 1)
	{
		GenerateNLIStagePoints(t);
		glLineWidth(2);
		mShader.SetUniform4f("u_Color", 0.7f, 0.0f, 0.0f, 1.0f);
		// Ignore the control points
		int numberLinesToDraw = (mControlPoints.size() - 2);
		int offset = mControlPoints.size();
		int totalLinesDrawn = 0;

		// Draw lines reducing the number of lines to draw each loop
		while (numberLinesToDraw > 0)
		{
			// Skip index if at cutoff
			for (int index = 0; index < numberLinesToDraw; ++index)
			{
				DrawLine(mStagePoints[offset + index], mStagePoints[offset + index + 1]);
				++totalLinesDrawn;
			}
			offset += numberLinesToDraw + 1;
			--numberLinesToDraw;
		}
	}
}
