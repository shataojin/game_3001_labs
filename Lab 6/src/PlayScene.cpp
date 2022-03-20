#include "PlayScene.h"
#include "Game.h"
#include "EventManager.h"

// required for IMGUI
#include "imgui.h"
#include "imgui_sdl.h"

// required for scene
#include "Renderer.h"
#include "Util.h"
#include "Config.h"
#include <fstream>

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
	m_checkAgentLOS(m_pSpaceShip, m_pTarget);
	switch (m_LOSMode)
	{
	case 0:
		m_checkAllNodesWithTarget(m_pTarget); // target
		break;
	case 1:
		m_checkAllNodesWithTarget(m_pSpaceShip); // space ship
		break;
	case 2:
		m_checkAllNodesWithBoth(); // both
		break;
	}
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

	// Create new obstacles from a file
	m_createObstaclesFromFile();

	// Setup a few more fields
	m_LOSMode = 0; // future enum?
	m_obstacleBuffer = 0;
	m_pathNodeLOSDistance = 1000;
	m_setPathNodeLOSDistance(m_pathNodeLOSDistance);

	//setup the Grid
	m_isGridEnabled = false;
	m_buildGrid();
	m_toggleGrid(m_isGridEnabled);

	m_pTarget = new Target();
	m_pTarget->getTransform()->position = glm::vec2(600.0f, 300.0f);
	addChild(m_pTarget, 2);

	m_pSpaceShip = new SpaceShip();
	m_pSpaceShip->getTransform()->position = glm::vec2(150.0f, 300.0f);
	addChild(m_pSpaceShip, 3);

	// preload sounds
	SoundManager::Instance().load("../Assets/audio/yay.ogg", "yay", SOUND_SFX);
	SoundManager::Instance().load("../Assets/audio/thunder.ogg", "thunder", SOUND_SFX);

	ImGuiWindowFrame::Instance().setGUIFunction(std::bind(&PlayScene::GUI_Function, this));
}

void PlayScene::m_buildGrid()
{
	const auto tile_size = Config::TILE_SIZE;
	auto offset = glm::vec2(Config::TILE_SIZE * 0.5f, Config::TILE_SIZE * 0.5f);

	m_clearNodes(); // we will need clear nodes because we will rebuild/redraw the grid if we move an obstacle

	// lay out a grid of path_nodes
	for (int row = 0; row < Config::ROW_NUM; ++row)
	{
		for (int col = 0; col < Config::COL_NUM; ++col)
		{
			PathNode* path_node = new PathNode();
			path_node->getTransform()->position = glm::vec2((col * tile_size) + offset.x, (row * tile_size) + offset.y);
			bool keep_node = true;
			for (auto obstacle : m_pObstacles)
			{
				// determine which path_nodes to keep
				if (CollisionManager::AABBCheckWithBuffer(path_node, obstacle, m_obstacleBuffer))
				{
					keep_node = false;
				}
			}
			if (keep_node)
			{
				addChild(path_node, 1);
				m_pGrid.push_back(path_node);
			}
			else
			{
				delete path_node;
			}
		}
	}

	// if Grid is supposed to be hidden - make it so!
	m_toggleGrid(m_isGridEnabled);
}

void PlayScene::m_toggleGrid(const bool state)
{
	for (auto path_node : m_pGrid)
	{
		path_node->setVisible(state);
	}
}

bool PlayScene::m_checkAgentLOS(Agent * agent, DisplayObject * target_object)
{
	bool has_LOS = false; // default - no LOS
	agent->setHasLOS(has_LOS);
	glm::vec4 LOSColour;

	// if ship to target distance is less than or equal to LOS Distance
	const auto AgentToTargetDistance = Util::getClosestEdge(agent->getTransform()->position, target_object);
	if (AgentToTargetDistance <= agent->getLOSDistance()) // we are in range
	{
		std::vector<DisplayObject*> contact_list;
		for (auto display_object : getDisplayList())
		{
			// check if the display_object is closer to the spaceship than the target
			const auto AgentToObjectDistance = Util::getClosestEdge(agent->getTransform()->position, display_object);
			if (AgentToObjectDistance > AgentToTargetDistance) continue;
			if ((display_object->getType() != AGENT) && (display_object->getType() != PATH_NODE) && (display_object->getType() != TARGET))
			{
				contact_list.push_back(display_object);
			}
		}

		const glm::vec2 agentEndPoint = agent->getTransform()->position + agent->getCurrentDirection() * agent->getLOSDistance();
		has_LOS = CollisionManager::LOSCheck(agent, agentEndPoint, contact_list, target_object);

		LOSColour = (target_object->getType() == AGENT) ? glm::vec4(0, 0, 1, 1) : glm::vec4(0, 1, 0, 1);
		agent->setHasLOS(has_LOS, LOSColour);
	}
	return has_LOS;
}

bool PlayScene::m_checkPathNodeLOS(PathNode * path_node, DisplayObject * target_object)
{
	// check angle to target so we can still use LOS distance for path_nodes
	const auto target_direction = target_object->getTransform()->position - path_node->getTransform()->position;
	const auto normalized_direction = Util::normalize(target_direction);
	path_node->setCurrentDirection(normalized_direction);
	return m_checkAgentLOS(path_node, target_object);
}

void PlayScene::m_checkAllNodesWithTarget(DisplayObject * target_object)
{
	for (auto path_node : m_pGrid)
	{
		m_checkPathNodeLOS(path_node, target_object);
	}
}

void PlayScene::m_checkAllNodesWithBoth()
{
	for (auto path_node : m_pGrid)
	{
		bool LOSWithSpaceShip = m_checkPathNodeLOS(path_node, m_pSpaceShip);
		bool LOSWithTarget = m_checkPathNodeLOS(path_node, m_pTarget);
		path_node->setHasLOS(LOSWithSpaceShip && LOSWithTarget, glm::vec4(0, 1, 1, 1));
	}
}

void PlayScene::m_setPathNodeLOSDistance(const int dist)
{
	for (auto path_node : m_pGrid)
	{
		path_node->setLOSDistance(static_cast<float>(dist));
	}
}

void PlayScene::m_clearNodes()
{
	m_pGrid.clear();
	for (auto display_object : getDisplayList())
	{
		if (display_object->getType() == PATH_NODE)
		{
			removeChild(display_object);
		}
	}
}

void PlayScene::m_createObstaclesFromFile()
{
	std::ifstream inFile("../Assets/data/obstacles.txt");
	while (!inFile.eof())
	{
		std::cout << "Obstacle" << std::endl;
		Obstacle* obstacle = new Obstacle();
		float x, y, w, h; // same way the file is organized
		inFile >> x >> y >> w >> h; // read data from line in the file
		obstacle->getTransform()->position = glm::vec2(x, y);
		obstacle->setWidth(w);
		obstacle->setHeight(h);
		addChild(obstacle, 0);
		m_pObstacles.push_back(obstacle);
	}
	inFile.close();
}

void PlayScene::GUI_Function()
{
	auto offset = glm::vec2(Config::TILE_SIZE * 0.5f, Config::TILE_SIZE * 0.5f);

	// Always open with a NewFrame
	ImGui::NewFrame();

	// See examples by uncommenting the following - also look at imgui_demo.cpp in the IMGUI filter
	//ImGui::ShowDemoWindow();

	ImGui::Begin("Lab 6 Debug Properties", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoMove);

	ImGui::Separator();

	if (ImGui::Checkbox("Toggle Grid", &m_isGridEnabled))
	{
		m_toggleGrid(m_isGridEnabled);
	}

	ImGui::Separator();
	if (ImGui::Button("Node LOS Target", { 300, 20 }))
	{
		m_LOSMode = 0;
	}

	if (m_LOSMode == 0)
	{
		ImGui::SameLine();
		ImGui::Text("<Active>");
	}

	if (ImGui::Button("Node LOS SpaceShip", { 300, 20 }))
	{
		m_LOSMode = 1;
	}

	if (m_LOSMode == 1)
	{
		ImGui::SameLine();
		ImGui::Text("<Active>");
	}

	if (ImGui::Button("Node LOS to both Target and SpaceShip", { 300, 20 }))
	{
		m_LOSMode = 2;
	}

	if (m_LOSMode == 2)
	{
		ImGui::SameLine();
		ImGui::Text("<Active>");
	}

	if (ImGui::SliderInt("Path Node LOS Distance", &m_pathNodeLOSDistance, 0, 1000))
	{
		m_setPathNodeLOSDistance(m_pathNodeLOSDistance);
	}

	ImGui::Separator();

	// spaceship properties

	static int shipPosition[] = { m_pSpaceShip->getTransform()->position.x, m_pSpaceShip->getTransform()->position.y };
	if (ImGui::SliderInt2("Ship Position", shipPosition, 0, 800))
	{
		m_pSpaceShip->getTransform()->position.x = shipPosition[0];
		m_pSpaceShip->getTransform()->position.y = shipPosition[1];
	}

	// allow the ship to rotate
	static int angle;
	if (ImGui::SliderInt("Ship Direction", &angle, -360, 360))
	{
		m_pSpaceShip->setCurrentHeading(angle);
	}

	ImGui::Separator();

	// Target properties

	static int targetPosition[] = { m_pTarget->getTransform()->position.x, m_pTarget->getTransform()->position.y };
	if (ImGui::SliderInt2("Target Position", targetPosition, 0, 800))
	{
		m_pTarget->getTransform()->position.x = targetPosition[0];
		m_pTarget->getTransform()->position.y = targetPosition[1];
	}

	ImGui::Separator();

	// Add Obstacle Position Control for all obstacles
	for (unsigned i = 0; i < m_pObstacles.size(); ++i)
	{
		int obstaclePosition[] = { m_pObstacles[i]->getTransform()->position.x, m_pObstacles[i]->getTransform()->position.y };
		std::string label = "Obstacle" + std::to_string(i + 1) + " Position";
		if (ImGui::SliderInt2(label.c_str(), obstaclePosition, 0, 800))
		{
			m_pObstacles[i]->getTransform()->position.x = obstaclePosition[0];
			m_pObstacles[i]->getTransform()->position.y = obstaclePosition[1];
			m_buildGrid();
		}
	}

	ImGui::Separator();

	if (ImGui::SliderInt("Obstacle Buffer", &m_obstacleBuffer, 0, 100))
	{
		m_buildGrid();
	}

	ImGui::End();
}


