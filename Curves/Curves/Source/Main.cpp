#include "Main.h"

int main(int argc, char* argv[])
{
	// Init SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		return -1;
	}
	
	// Set OpenGL version
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	// Create a window
	SDL_Window* pWindow = SDL_CreateWindow("MAT 500", 0, 0, 800, 600, SDL_WINDOW_OPENGL);
	SDL_SetWindowPosition(pWindow, 100, 100);

	// Create a context
	SDL_GLContext context = SDL_GL_CreateContext(pWindow);

	// Initialize GLEW
	glewExperimental = true;
	glewInit();

	// Initialize ImGui context
	InitImGui(pWindow, context);

	bool applicationIsRunning = true;

	// Pre-load some values
	InitChoose();

	// Start with a certain project
	static int projectNumber = 1;
	Project* currentProject = nullptr;
	bool changeProject = true;

	while (applicationIsRunning)
	{
		// Clear OpenGL
		OpenGLClear();

		// Check which project should be running
		if (changeProject)
		{
			delete currentProject;
			switch (projectNumber)
			{
				case 1:
					currentProject = new Project1(pWindow);
					break;
				case 2:
					currentProject = new Project2(pWindow);
					break;
				case 3:
					currentProject = new Project3(pWindow);
					break;
				case 4:
					currentProject = new Project4(pWindow);
					break;
				default:
					break;
			}

			currentProject->Initialize();

			changeProject = false;
		}

		// Process events and check if application should still run
		currentProject->ProcessEvents(applicationIsRunning);		
		
		// Display ImGui project menu
		currentProject->StartFrame();
		changeProject = DisplayImGuiMenu(projectNumber);

		// Display the current project data
		currentProject->Display();

	}

	// Clean up
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(pWindow);
	SDL_Quit();

	return 0;
}