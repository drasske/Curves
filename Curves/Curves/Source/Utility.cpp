#include "Utility.h"

#include <map>

#include "GLEW/glew.h"

#include "IMGUI/imgui.h"
#include "IMGUI/imgui_impl_sdl.h"
#include "IMGUI/imgui_impl_opengl3.h"

void InitChoose()
{
	for (int i = 0; i < 21; ++i)
	{
		Choose(20, i);
	}
}

float Choose(int d, int i)
{
	// Base cases
	if (d == 0 || d == i || i == 0)
	{
		return 1.0f;
	}

	typedef std::pair<int, int> pairInt;
	static std::map<pairInt, float> lookUpTable;
	std::pair<int, int> pair(d, i);

	// Check if value is already in table
	auto it = lookUpTable.find(pair);
	if (it != lookUpTable.end())
	{
		return it->second;
	}

	// Add the value to the table otherwise
	float value = Choose(d - 1, i - 1) + Choose(d - 1, i);
	lookUpTable[{d, i}] = value;
	//lookUpTable.insert(std::make_pair(std::make_pair(d, i), value));
	return value;
}

void InitImGui(SDL_Window* pWindow, SDL_GLContext context)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Setup ImGui style
	ImGui::StyleColorsDark();

	// Setup platform/renderer bindings
	ImGui_ImplSDL2_InitForOpenGL(pWindow, context);
	const char* glsl_version = "#version 430";
	ImGui_ImplOpenGL3_Init(glsl_version);

	// Set window rounding
	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowRounding = 0;
}

bool DisplayImGuiMenu(int& projectNumber)
{
	// Set the window size and position
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(800, 80));

	// Keep track of which project should be running
	int previousProjectNumber = projectNumber;

	ImGui::Begin("Properties", (bool*)0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar);
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Menu"))
			{
				if (ImGui::BeginMenu("Project"))
				{
					ImGui::RadioButton("Project 1", &projectNumber, 1);
					ImGui::RadioButton("Project 2", &projectNumber, 2);
					ImGui::RadioButton("Project 3", &projectNumber, 3);
					ImGui::RadioButton("Project 4", &projectNumber, 4);
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
	}

	// Check if project number has changed
	return previousProjectNumber != projectNumber;
}

void OpenGLClear()
{
	glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}