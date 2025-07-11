#include "Headers\App.h"
#include "Ecs/EntityNodes.h"
#include "Shader\Shader.h"

#include "Ecs\ObjectManager.h"

#include "Camera\Camera2D.h"
#include "Headers\GlobalVars.h"

#include "Headers/EditorInput.h" // keybord & mouse input



App::App()
{
}

App* App::Instance()
{
	App* mainApp = new App;
	return mainApp;
}

void App::Initialize(GLFWwindow* window)
{
}

void App::RunApp()
{	

	WindowManager windowManager(SCR_WIDTH, SCR_HEIGHT, "Spidex 2d World");
		if (!windowManager.GLFWInitialize()) {
			return;
		}
		else
		{
			// Load an icon to the main form
			MainScreen::Instance()->Initialize(windowManager.GetWindow());
									
			glfwSetCursorPosCallback(windowManager.GetWindow(), mouse_callback); // this is the best place
			glfwSetScrollCallback(windowManager.GetWindow(), scroll_callback);  // set camera pos zoom

			MainScreen::Instance()->SetUpImGui(windowManager.GetWindow()); // must be set after mouse callback
		}	
				
		// ############################################# Drawing Object !!! ################################
		
		ShaderManager::SetupShaders(); // Initialize the shaders
			
		MainScreen::Instance()->Creat_FrameBuffer(); // my frame buffer to fill main scene window

        //EntityComponents entityComponents; // this fills the object list
		EntityNodes entityComponents;
	    entityComponents.Initialize();

	    int index = 0, objectIndex = 0, indexTypeID = 0;

		

		Settings settings;
		settings.LoadSettings("settings.txt"); // Loading the saved settings
		
	    // 20 lines so far
		while (AppIsRunning)
		{
			App::Instance()->Timer();

			// close it all down if you click the cross or press Esc button
			if (glfwWindowShouldClose(windowManager.GetWindow())) { AppIsRunning = false; }
						
			processInput(windowManager.GetWindow()); // Keybord and mouse control

			// ############################################ GUI from Here ####################################
			// NewImguiFrame, MainWindowMenu, AboutWindow, MainDockSpace, MainSceanWindow
			MainScreen::Instance()->WinInit(windowManager.GetWindow()); // Initialize all the above

			MainScreen::Instance()->MainSceneWindow(windowManager.GetWindow()); // main drawing window

			settings.SettingsWindow(window);  // Initialize the Settings Window

			EntityNodes::Instance()->EntityManagmentSystem(entityComponents.GetModels(), currentIndex,
				index, objectIndex, indexTypeID); // Entity Management System Scene list

			
			
			// #############################################  End GUI ##########################################

			MainScreen::Instance()->Bind_Framebuffer(); // for the main screen

			MainScreen::Instance()->BgColour(BgCol);
			// Clear buffers and enable depth testing
			MainScreen::Instance()->ClearScreen(); // glClear GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_DEPTH_TEST

			// ############################################# Camera Object !!! ################################
			App::MainCamera(); // ########## This is the main Camera ##########		
			
			EntityNodes::Instance()->ObjectEditor(entityComponents.GetModels());

			// Tile Editor Window
			static TileEditor tileEditor;
			tileEditor.Render(entityComponents.GetModels());
						
			// Darw all objects on screen
			EntityNodes::Instance()->RenderScene(camera.GetViewMatrix(),
				camera.GetProjectionMatrix(),
				entityComponents.GetModels(), currentIndex);	

		MainScreen::Instance()->Unbinde_Frambuffer();
		// ############################################# End Drawing Object !!! ################################
				
		MainScreen::Instance()->RenderImGui(windowManager.GetWindow()); // render the imgui windows	
				
		glfwSwapBuffers(windowManager.GetWindow()); // the last 2 lines of code
		glfwPollEvents();	

	}
	App::AppShutdown(); // clean-up
}

void App::MainCamera()
{
	/*viewMatrix = camera.GetViewMatrix();
	projectionMatrix = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.3f, 200.0f);*/
	viewMatrix = camera.GetViewMatrix();
	projectionMatrix = camera.GetProjectionMatrix();

}

glm::mat4 App::GetViewMatrix()
{
	return viewMatrix;
}

glm::mat4 App::GetProjectionMatrix(float aspectRatio)
{
	return projectionMatrix;
}

void App::Timer()
{
	float currentFrame = static_cast<float>(glfwGetTime());
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
}

void App::AppShutdown()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();

	LogInternals::Instance()->Info("Engine shutdown successfully");
	
}

App::~App()
{
}

