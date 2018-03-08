#pragma once
#include <Box2D/Box2D.h>


class Block
{
public:
	Block(b2World* world, const b2Vec2& start, const b2Vec2& end);
	Block(b2World* world, b2Body* connected, const b2Vec2& start, const b2Vec2& end);

	void Update(float dt);

	static b2Body* GetOverlappingBody(b2World* world, b2Body* body, const b2Vec2& p);
private:
	b2Body* m_body;
	b2RevoluteJoint* CreateJoint(const b2Vec2& pos, b2Body* bodyB);
	
	b2Body* CreateBlock(b2World* world, const b2Vec2& start, const b2Vec2& end);

	b2RevoluteJoint* m_joint1;
	b2RevoluteJoint* m_joint2;

};
