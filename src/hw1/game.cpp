#include "game.h"

using namespace std;
using namespace hw1;

/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */

Game::Game()
{
}

Game::~Game()
{
}

void Game::Init()
{
	Element *el;

	glm::vec3 position(-4.5f, 4.0f, 6.0f);
	glm::vec3 target(0.0f, 1.0f, 0.0f);
	glm::vec3 up(0.0f, 1.0f, 0.0f);

	glm::mat4 view = glm::lookAt(position, target, up);
	glm::quat rotation = glm::quat_cast(glm::inverse(view));

	GetSceneCamera()->SetPositionAndRotation(position, rotation);
}

void Game::FrameStart()
{
	// Clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0.765, 0.839, 0.827, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// Sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}

void Game::Update(float deltaTimeSeconds)
{
	for (auto el : elements) {
		el->Render();
	}
}

void Game::FrameEnd()
{
	//DrawCoordinateSystem();
}

/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */

void Game::OnInputUpdate(float deltaTime, int mods)
{
}

void Game::OnKeyPress(int key, int mods)
{
}

void Game::OnKeyRelease(int key, int mods)
{
	// Add key release event
}

void Game::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
}

void Game::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
}

void Game::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// Add mouse button release event
}

void Game::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Game::OnWindowResize(int width, int height)
{
}