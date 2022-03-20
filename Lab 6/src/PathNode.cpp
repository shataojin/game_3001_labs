#include "PathNode.h"

#include "Util.h"

PathNode::PathNode()
{
	setWidth(10);
	setHeight(10);
	getTransform()->position = glm::vec2(0.0f, 0.0f);
	getRigidBody()->isColliding = false;
	setType(PATH_NODE);

	setCurrentHeading(0.0f);
	setCurrentDirection(glm::vec2(1.0f, 0.0f)); // facing right

	setLOSDistance(1000.0f);
	setLOSColour(glm::vec4(0, 0.5, 1, 1)); // light blue colour
}

PathNode::~PathNode()
= default;

void PathNode::draw()
{
	Util::DrawRect(getTransform()->position - glm::vec2(5, 5), getWidth(), getHeight(), getLOSColour());
}

void PathNode::update()
{
}

void PathNode::clean()
{
}
