#pragma once
#ifndef __PLAY_SCENE__
#define __PLAY_SCENE__

#include "Scene.h"

#include "Heuristic.h"
#include "Target.h"
#include "SpaceShip.h"
#include "Obstacle.h"
#include "PathNode.h"

class PlayScene : public Scene
{
public:
	PlayScene();
	~PlayScene();

	// Scene LifeCycle Functions
	virtual void draw() override;
	virtual void update() override;
	virtual void clean() override;
	virtual void handleEvents() override;
	virtual void start() override;
private:
	// IMGUI Function
	void GUI_Function();
	std::string m_guiTitle;
	glm::vec2 m_mousePosition;
	bool m_isGridEnabled;

	// Game Objects
	Target* m_pTarget;
	SpaceShip* m_pSpaceShip;

	// Create our Division Scheme (Grid of PathNodes)
	std::vector<PathNode*> m_pGrid;
	std::vector<Obstacle*> m_pObstacles;

	void m_buildGrid();
	void m_toggleGrid(bool state);
	bool m_checkAgentLOS(Agent* agent, DisplayObject* target_object);
	bool m_checkPathNodeLOS(PathNode* path_node, DisplayObject* target_object);
	void m_checkAllNodesWithTarget(DisplayObject* target_object);
	void m_checkAllNodesWithBoth();
	void m_setPathNodeLOSDistance(int dist);
	int m_LOSMode; // 0 = nodes visible to target, 1 = nodes visible to player, 2 = nodes visible to both

	void m_clearNodes();
	void m_createObstaclesFromFile();

	int m_obstacleBuffer;
	int m_pathNodeLOSDistance;

};

#endif /* defined (__PLAY_SCENE__) */