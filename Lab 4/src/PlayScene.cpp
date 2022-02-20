#include "PlayScene.h"
#include "Game.h"
#include "EventManager.h"

// required for IMGUI
#include "imgui.h"
#include "imgui_sdl.h"
#include "Renderer.h"
#include "Util.h"
#include "Config.h"

PlayScene::PlayScene()
{
	PlayScene::start();
}

PlayScene::~PlayScene()
= default;

void PlayScene::draw()
{
	drawDisplayList();


	
	SDL_SetRenderDrawColor(Renderer::Instance().getRenderer(), 255, 255, 255, 255);
}

void PlayScene::update()
{
	updateDisplayList();

}

void PlayScene::clean()
{
	removeAllChildren();
}

void PlayScene::handleEvents()
{
	EventManager::Instance().update();

	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_ESCAPE))
	{
		TheGame::Instance().quit();
	}

	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_1))
	{
		TheGame::Instance().changeSceneState(START_SCENE);
	}

	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_2))
	{
		TheGame::Instance().changeSceneState(END_SCENE);
	}
}

void PlayScene::start()
{
	// Set GUI Title
	m_guiTitle = "Play Scene";


	m_buildGrid();
	m_pTarget = new Target();
	addChild(m_pTarget);

	m_pSpaceShip = new SpaceShip();
	addChild(m_pSpaceShip);

	/*m_pTile = new Tile();
	m_pTile->getTransform()->position = glm::vec2(400.0f, 300.0f);
	m_pTile->setParent(this);
	m_pTile->addLabels();
	addChild(m_pTile);
	m_pTile->setLabelsEnabled(true);*/


	// preload sounds
	SoundManager::Instance().load("../Assets/audio/yay.ogg", "yay", SOUND_SFX);
	SoundManager::Instance().load("../Assets/audio/thunder.ogg", "thunder", SOUND_SFX);

	ImGuiWindowFrame::Instance().setGUIFunction(std::bind(&PlayScene::GUI_Function, this));
}

void PlayScene::m_buildGrid()
{
	auto tileSize = Config::TILE_SIZE;

	// add tiles to the grid
	for (int row = 0; row < Config::ROW_NUM; ++row)
	{
		for (int col = 0; col < Config::COL_NUM; ++col)
		{
			Tile* tile = new Tile(); // create empty tile
			tile->getTransform()->position = glm::vec2(col * tileSize, row * tileSize);
			tile->setGridPosition(col, row);
			tile->setParent(this);
			tile->addLabels();
			addChild(tile);
			tile->setLabelsEnabled(true);
			m_pGrid.push_back(tile);
		}
	}
}

bool PlayScene::m_getGridEnabled() const
{
	return m_isGridEnabled;
}

void PlayScene::m_setGridEnabled(bool state)
{
	m_isGridEnabled = state;
	for (auto tile : m_pGrid) 
	{
		tile->setEnabled(m_isGridEnabled);
		tile->setLabelsEnabled(m_isGridEnabled);
	}
}

Tile* PlayScene::m_getTile(const int col, const int row)
{
	return m_pGrid[(row * Config::COL_NUM) + col];
}

Tile* PlayScene::m_getTile(const glm::vec2 grid_position)
{
	const auto col = grid_position.x;
	const auto row = grid_position.y;
	return m_getTile(col, row);
}

void PlayScene::GUI_Function()
{
	auto offset = glm::vec2(Config::TILE_SIZE * 0.5f, Config::TILE_SIZE * 0.5f);

	// Always open with a NewFrame
	ImGui::NewFrame();

	// See examples by uncommenting the following - also look at imgui_demo.cpp in the IMGUI filter
	//ImGui::ShowDemoWindow();
	
	ImGui::Begin("Lab 4 Debug Properties", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoMove);

	ImGui::Separator();

	static bool toggle_grid = false;
	if (ImGui::Checkbox("Toggle Grid", &toggle_grid))
	{
		m_isGridEnabled = toggle_grid;
		m_setGridEnabled(m_isGridEnabled);
	}
	
	ImGui::Separator();
	// spaceship properties
	
	static float start_position[2] = { m_pSpaceShip->getTransform()->position.x, m_pTarget->getTransform()->position.y };
	if (ImGui::SliderFloat2("Target Position", start_position, 0.0f, 800.0f))
	{
		m_pSpaceShip->getTransform()->position = glm::vec2(start_position[0], start_position[1]);

	}

	// Target properties
	
	static float goal_position[2] = { m_pTarget->getTransform()->position.x, m_pTarget->getTransform()->position.y};
	if(ImGui::SliderFloat2("Goal Position", goal_position, 0.0f, 800.0f))
	{
		m_pTarget->getTransform()->position = glm::vec2(goal_position[0], goal_position[1]);
		
	}

	ImGui::End();
}
