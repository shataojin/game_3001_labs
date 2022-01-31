#include "SpaceShip.h"

#include "Game.h"
#include "TextureManager.h"
#include "Util.h"

SpaceShip::SpaceShip()
{
	TextureManager::Instance().load("../Assets/textures/ncl.png", "space_ship");

	const auto size = TextureManager::Instance().getTextureSize("space_ship");
	setWidth(size.x);
	setHeight(size.y);
	getTransform()->position = glm::vec2(600.0f, 400.0f);
	getRigidBody()->velocity = glm::vec2(0, 0);
	getRigidBody()->acceleration = glm::vec2(0, 0);
	getRigidBody()->isColliding = false;

	// starting motion properties
	m_maxSpeed = 20.0f; // a maximum number of pixels moved per frame
	m_turnRate = 5.0f; // a maximum number of degrees to turn each time-step
	m_accelerationRate = 4.0f; // a maximum number of pixels to add to the velocity each frame

	setType(SPACE_SHIP);
}

SpaceShip::~SpaceShip()
= default;

void SpaceShip::draw()
{
	// alias for x and y
	const auto x = getTransform()->position.x;
	const auto y = getTransform()->position.y;

	// draw the target
	TextureManager::Instance().draw("space_ship", x, y, getCurrentHeading(), 255, true);
}

void SpaceShip::update()
{
	m_move();
}

void SpaceShip::clean()
{
}


void SpaceShip::m_move()
{
	
}
