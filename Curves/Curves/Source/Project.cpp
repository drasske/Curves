#include "Project.h"

#include "GLEW/glew.h"

#include "IMGUI/imgui.h"
#include "IMGUI/imgui_impl_sdl.h"
#include "IMGUI/imgui_impl_opengl3.h"

Project::Project(SDL_Window *pWindow) : mShader("./Source/Basic.shader")
{
	mpWindow = pWindow;
	mDimension = 1;
	mPreviousDimension = mDimension;
	mAlgorithm = 0;
	mSelectedPoint = nullptr;

	mShader.Bind();

	// Create vertex array and buffer
	glGenVertexArrays(1, &mVAO);
	glBindVertexArray(mVAO);

	glGenBuffers(1, &mVBO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);

	// Create vertex buffer layout
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_DOUBLE, GL_FALSE, 0, (void*)0);
}

Project::~Project()
{
}

void Project::ProcessEvents(bool& applicationIsRunning)
{
	// Process ImGui events
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		ImGui_ImplSDL2_ProcessEvent(&event);
		if (event.type == SDL_QUIT)
		{
			applicationIsRunning = false;
		}
		if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(mpWindow))
		{
			applicationIsRunning = false;
		}
		if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_ESCAPE)
		{
			applicationIsRunning = false;
		}
	}
}

void Project::StartFrame()
{
	// Start the ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(mpWindow);
	ImGui::NewFrame();

	// Optional show demo window
	//ImGui::ShowDemoWindow();
}

void Project::ClampDimension(int min, int max)
{
	if (mDimension > max)
	{
		mDimension = max;
	}
	else if (mDimension < min)
	{
		mDimension = min;
	}
}

void Project::DrawLine(const float x1, const float y1, const float x2, const float y2)
{
	std::vector<double> line;
	line.push_back(x1);
	line.push_back(y1);
	line.push_back(x2);
	line.push_back(y2);

	glBufferData(GL_ARRAY_BUFFER, line.size() * sizeof(double), &line[0], GL_STATIC_DRAW);

	glDrawArrays(GL_LINES, 0, line.size() / 2);
}

void Project::DrawLine(const std::vector<double>& points)
{
	glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(double), &points[0], GL_STATIC_DRAW);
	glDrawArrays(GL_LINES, 0, points.size() / 2);
}

void Project::DrawLine(const ControlPoint & p1, const ControlPoint & p2)
{
	std::vector<ControlPoint> points{ p1, p2 };
	glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(ControlPoint), &points[0], GL_STATIC_DRAW);
	glDrawArrays(GL_LINES, 0, points.size());
}

ControlPoint operator+(const ControlPoint& p1, const ControlPoint& p2)
{
	return ControlPoint{ p1.x + p2.x, p1.y + p2.y };
}

ControlPoint operator-(const ControlPoint & p1, const ControlPoint & p2)
{
	return ControlPoint{ p1.x - p2.x, p1.y - p2.y };
}

ControlPoint operator*(const ControlPoint & p, double s)
{
	return ControlPoint{ p.x * s, p.y *s };
}

ControlPoint operator*(double s, const ControlPoint & p)
{
	return p*s;
}
