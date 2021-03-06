#pragma once
#ifndef __PLAY_SCENE__
#define __PLAY_SCENE__

#include "Scene.h"
#include "Plane.h"
#include "Player.h"
#include "Button.h"
#include "Label.h"

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
	void GUI_Function() const;
	std::string m_guiTitle;
	
	glm::vec2 m_mousePosition;

	Plane* m_pPlaneSprite;//UI Items (2022/01/16)
	Player* m_pPlayer;//UI Items (2022/01/16)
	bool m_playerFacingRight;

	// UI Items
	Button* m_pBackButton;//UI Items (2022/01/16)
	Button* m_pNextButton;//UI Items (2022/01/16)
	Label* m_pInstructionsLabel;//UI Items (2022/01/16)
};

#endif /* defined (__PLAY_SCENE__) */