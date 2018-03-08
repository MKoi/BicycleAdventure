#pragma once
#include <Box2D/Box2D.h>


class Character
{
public:
	enum Direction {
		LEFT,
		RIGHT,
		STOP,
		FALLEN
	};

	Character(b2World* world, const b2Vec2& position);
	void Update(float timeDelta);
	void Move(Direction direction);
	void FallDown(Direction direction);
	const b2Vec2& GetPosition();

private:

	void CreateTorso(b2World* world, const b2Vec2& pos);
	void CreateHead(b2World* world);
	void CreateArms(b2World* world);
	void CreateFeet(b2World* world);
	void SetFixture(b2Body* body, b2Shape* shape, float density, int16 collisionGroup);
	void SetDistanceJoint(b2World* world, b2Body* bodyA, b2Body* bodyB);
	void SetRevoluteJoint(b2World* world, b2Body* bodyA, b2Body* bodyB, float lower, float upper);
	bool FeetOnGround();
	bool BodyTouchesGround(b2Body* body);
	float GetMass();

	b2Body* m_head;
	b2Body* m_torso;
	b2Body* m_rightFoot;
	b2Body* m_leftFoot;
	b2Body* m_rightHand;
	b2Body* m_leftHand;

	static const int16 k_CollisionCategory = -1;
	const float k_headDensity;
	const float k_BodyDensity;
	Direction m_direction;
	float m_timeSinceLastMove;
	float m_mass;
};
