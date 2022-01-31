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
	getTransform()->position = glm::vec2(100.0f, 400.0f);
	getRigidBody()->velocity = glm::vec2(0, 0);
	getRigidBody()->acceleration = glm::vec2(0, 0);
	getRigidBody()->isColliding = false;

	// starting motion properties
	m_maxSpeed = 50.0f; // a maximum number of pixels moved per frame
	m_turnRate = 5.0f; // a maximum number of degrees to turn each time-step
	m_accelerationRate = 2.0f; // a maximum number of pixels to add to the velocity each frame

	setType(AGENT);
}

SpaceShip::~SpaceShip()
= default;

void SpaceShip::draw()
{
	// alias for x and y
	const auto x = getTransform()->position.x;
	const auto y = getTransform()->position.y;

	// draw the target
	TextureManager::Instance().draw("space_ship", x, y, getCurrentHeading(), 255, isCentered());
}

void SpaceShip::update()
{
	m_move();
}

void SpaceShip::clean()
{
}


float SpaceShip::getMaxSpeed() const
{
	return m_maxSpeed;
}

float SpaceShip::getTurnRate() const
{
	return m_turnRate;
}

float SpaceShip::getAccelerationRate() const
{
	return m_accelerationRate;
}

glm::vec2 SpaceShip::getDesiredVelocity() const
{
	return m_desiredVelocity;
}

void SpaceShip::setMaxSpeed(const float speed)
{
	m_maxSpeed = speed;
}

void SpaceShip::setTurnRate(const float angle)
{
	m_turnRate = angle;
}

void SpaceShip::setAccelerationRate(const float rate)
{
	m_accelerationRate = rate;
}

void SpaceShip::setDesiredVelocity(const glm::vec2 target_position)
{
	m_desiredVelocity = Util::normalize(target_position - getTransform()->position);
}

void SpaceShip::Seek()
{

	setDesiredVelocity(getTargetPosition());

	const glm::vec2 steering_direction = getDesiredVelocity() - getCurrentDirection();

	LookWhereYoureGoing(steering_direction);

	getRigidBody()->acceleration = getCurrentDirection() * getAccelerationRate();
}

void SpaceShip::LookWhereYoureGoing(const glm::vec2 target_direction)
{
	const float target_rotation = Util::signedAngle(getCurrentDirection(), target_direction);

	const float turn_sensitivity = 5.0f;

	if (abs(target_rotation) > turn_sensitivity)
	{
		if (target_rotation > 0.0f)
		{
			setCurrentHeading(getCurrentHeading() + getTurnRate());
		}
		else if (target_rotation < 0.0f)
		{
			setCurrentHeading(getCurrentHeading() - getTurnRate());
		}
	}
}


void SpaceShip::m_move()
{
	Seek();

	//                                   final Position     position term    velocity term     acceleration term
	// kinematic equation for motion --> Pf            =      Pi     +     Vi*(time)    +   (0.5)*Ai*(time * time)

	const float dt = TheGame::Instance().getDeltaTime();

	// compute the position term
	const glm::vec2 initial_position = getTransform()->position;

	// compute the velocity term
	const glm::vec2 velocity_term = getRigidBody()->velocity * dt;

	// compute the acceleration term
	const glm::vec2 acceleration_term = getRigidBody()->acceleration * 0.5f;// *dt;


	// compute the new position
	glm::vec2 final_position = initial_position + velocity_term + acceleration_term;

	getTransform()->position = final_position;

	// add our acceleration to velocity
	getRigidBody()->velocity += getRigidBody()->acceleration;

	// clamp our velocity at max speed
	getRigidBody()->velocity = Util::clamp(getRigidBody()->velocity, getMaxSpeed());
}

