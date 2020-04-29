#include "Project1.h"

#include "IMGUI/imgui.h"
#include "IMGUI/imgui_impl_sdl.h"
#include "IMGUI/imgui_impl_opengl3.h"

#include "GLEW/glew.h"

Project1::Project1(SDL_Window* pWindow) : Project(pWindow)
{
	// Set y axis data
	const double xPosition = -0.8f;
	mYaxis.push_back(xPosition);
	mYaxis.push_back(0.9f);
	mYaxis.push_back(xPosition);
	mYaxis.push_back(-0.9f);

	// Set x axis data
	const double yPosition = 0.0f;
	mXaxis.push_back(-0.8f);
	mXaxis.push_back(yPosition);
	mXaxis.push_back(0.8f);
	mXaxis.push_back(yPosition);
}

Project1::~Project1()
{
}

void Project1::Display()
{
	// Project ImGui Window
	{

		ImGui::RadioButton("BB Form", &mAlgorithm, 0); ImGui::SameLine();
		ImGui::RadioButton("NLI", &mAlgorithm, 1); ImGui::SameLine();

		ImGui::PushItemWidth(100);
		ImGui::InputInt("Degree", &mDimension, 1);
		ImGui::PopItemWidth();

		if (ImGui::Button("Reset"))
		{
			// Reset control points
			mControlPoints.clear();

			// Add control points
			AddControlPoints();
		}

		// Clamp dimension
		ClampDimension(1, 20);

		ImGui::End();
	}

	// Render ImGui
	ImGui::Render();

	// Update control points if dimension has changed
	if (mDimension != mPreviousDimension)
	{
		// Reset control points
		mControlPoints.clear();

		// Add control points
		AddControlPoints();

		mPreviousDimension = mDimension;
	}

	// Get mouse position
	Uint32 mouseState;
	int mouseX, mouseY;
	mouseState = SDL_GetMouseState(&mouseX, &mouseY);
	float openGLMouseX = ((float)mouseX / 400.0f) - 1.0f;
	float openGLMouseY = -(((float)mouseY / 300.0f) - 1.0f);

	// Check mouse position against the control points to get selected point
	if (mSelectedPoint == nullptr)
	{
		for (int index = 0; index < mDimension + 1; ++index)
		{
			ControlPoint &currentPoint = mControlPoints[index];
			float xrange = 0.03f;
			float yrange = 0.07f;
			if (openGLMouseX < currentPoint.x + xrange && openGLMouseX > currentPoint.x - xrange && openGLMouseY < currentPoint.y + yrange && openGLMouseY > currentPoint.y - yrange)
			{
				mSelectedPoint = &currentPoint;
				break;
			}
		}
	}

	// Update selected point position
	if (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT) && mSelectedPoint != nullptr)
	{
		mSelectedPoint->y = openGLMouseY;
		// Clamp the y value
		if (mSelectedPoint->y > 0.9f)
		{
			mSelectedPoint->y = 0.9f;
		}
		if (mSelectedPoint->y < -0.9f)
		{
			mSelectedPoint->y = -0.9f;
		}
	}
	else
	{
		mSelectedPoint = nullptr;
	}


	// Set VBO data for user moveable points
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, mControlPoints.size() * sizeof(ControlPoint), &mControlPoints[0], GL_STATIC_DRAW);

	// Bind buffers 
	mShader.Bind();
	mShader.SetUniform4f("u_Color", 0.1f, 0.1f, 0.5f, 1.0f);
	glBindVertexArray(mVAO);

	// Draw dimension points
	glPointSize(10);
	glDrawArrays(GL_POINTS, 0, mControlPoints.size());

	// Draw the axis
	glLineWidth(2);
	mShader.SetUniform4f("u_Color", 0.3f, 0.3f, 0.3f, 1.0f);
	DrawLine(mYaxis);

	// Draw hash marks
	for (int numTicks = 0; numTicks < 7; ++numTicks)
	{
		// Calculate new y-values
		float yValue = (numTicks * 0.3f) - 0.9f;
		DrawLine(-0.85f, yValue, -0.75f, yValue);
	}

	DrawLine(mXaxis);

	// Draw the actual graph
	DrawGraph();

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	SDL_GL_SwapWindow(mpWindow);
}

void Project1::Initialize()
{
	AddControlPoints();
}

void Project1::AddControlPoints()
{
	for (int i = 0; i < mDimension + 1; ++i)
	{
		// Get the proper intervals
		// -1.0 - 1.0 for dimension 2 - 20
		// i = i + 1 / dimension
		ControlPoint toAdd;
		toAdd.x = -0.8f + (1.6f*(double)i / (double)mDimension);
		toAdd.y = 0.3f;
		mControlPoints.push_back(toAdd);
	}
}

void Project1::DrawGraph()
{
	switch (mAlgorithm)
	{
	case 0:
		DrawBBForm();
		break;
	case 1:
		DrawNLIForm();
		break;
	}
}

void Project1::DrawBBForm()
{
	double numberOfPoints = 200;
	double scale = 1.6 * 1.0 / numberOfPoints;
	// Add however many points to the graph
	std::vector<double> graphPoints;
	for (double currentPoint = 0; currentPoint < numberOfPoints; ++currentPoint)
	{
		double xValue = (currentPoint * scale) - 0.8;
		graphPoints.push_back(xValue);
		// Do the actual point calculation here
		double t = currentPoint / numberOfPoints;
		double value = BernsteinValue(t);
		// Get value of function and scale to window size
		// Convert value to OpenGL value
		//std::cout << value << std::endl;
		//std::cout << "OpenGL Value: " << (value * 0.3f) << std::endl;
		graphPoints.push_back(value*0.3f);
	}

	// Put into OpenGL
	glBufferData(GL_ARRAY_BUFFER, graphPoints.size() * sizeof(double), &graphPoints[0], GL_STATIC_DRAW);
	glDrawArrays(GL_LINE_STRIP, 0, graphPoints.size() / 2);
}

double Project1::BernsteinValue(double t)
{
	double output = 0.0;
	double scale = 10.0 / 3.0;
	int numIndices = mControlPoints.size();
	//int dimension = numIndices - 1;
	for (int index = 0; index < numIndices; ++index)
	{
		double coefficient = Choose(mDimension, index);
		double controlPointValue = mControlPoints[index].y * scale;
		output += controlPointValue * coefficient * pow((1.0 - t), (double)(mDimension - index)) * pow(t, (double)index);
	}
	return output;
}

void Project1::DrawNLIForm()
{
	double numberOfPoints = 200;
	double scale = 1.6 * 1.0 / numberOfPoints;
	// Add however many points to the graph
	std::vector<double> graphPoints;
	for (double currentPoint = 0; currentPoint < numberOfPoints; ++currentPoint)
	{
		double xValue = (currentPoint * scale) - 0.8f;
		graphPoints.push_back(xValue);
		// Do the actual point calculation here
		double t = currentPoint / numberOfPoints;
		double yValue = NLIValue(t);
		// Add the point to the list of graph points
		graphPoints.push_back(yValue);
	}

	// Put into OpenGL
	glBufferData(GL_ARRAY_BUFFER, graphPoints.size() * sizeof(double), &graphPoints[0], GL_STATIC_DRAW);
	glDrawArrays(GL_LINE_STRIP, 0, graphPoints.size() / 2);
}

double Project1::NLIValue(double t)
{
	int numPoints = mControlPoints.size();
	int offset = 0;
	int arrayIndex = 0;
	int arraySize = (numPoints * (numPoints + 1)) / 2;
	double* coefficients = new double[arraySize];

	// Populate initial array
	for (int i = 0; i < numPoints; ++i)
	{
		coefficients[i] = mControlPoints[i].y;
		++arrayIndex;
	}

	// Do the interpolation
	while (numPoints > 1)
	{
		for (int i = 0; i < numPoints - 1; ++i)
		{
			coefficients[arrayIndex] = (1.0 - t)*coefficients[i + offset] + t * coefficients[i + 1 + offset];
			++arrayIndex;
		}
		offset += numPoints;
		--numPoints;
	}

	double output = coefficients[arrayIndex - 1];
	delete[] coefficients;
	//std::cout << "Output: " << output << std::endl;
	return output;
}
