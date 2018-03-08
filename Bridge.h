#pragma once
#include "Box2D\Box2D.h"
#include "GameArea.h"

class Bridge : public GameArea
{
public:

	Bridge(b2World* world, const b2Vec2& pos, const Theme* theme, int variant) : GameArea(world, pos, theme, variant)
	{
		Physics::Parameters params = Physics::DefaultGroundParameters;
		if (m_variant == 0)
		{
			m_variant = std::rand() % 2 + 1;
		}
		const size_t pointCount = 6;
		b2Vec2 points[pointCount];
		const float startx = 10.0f;
		const float endx = (m_variant == 1) ? 30.0f : 50.0f;
		points[0] = b2Vec2_zero;
		points[1].Set(startx, 0.0f);
		points[2].Set(startx, -20.0f);
		points[3].Set(endx, -20.0f);
		points[4].Set(endx, 0.0f);
		points[5].Set(endx  + 10.0f, 0.0f);
		m_end = points[5];
		m_groundBody = Physics::CreateChain(world, params, points, pointCount, pos);

		float width = points[pointCount - 1].x - points[0].x;
		m_topLeft.Set(0.0f, width);
		m_bottomRight.Set(width, -width);
		
		const float blockWidth = 2.0f;
		size_t bridgeBlocks = static_cast<size_t>((endx - startx) / blockWidth);
		b2Vec2 d(blockWidth, -0.3f);
		b2Vec2 topLeft = points[1];
		b2Vec2 bottomRight = topLeft + d;
		b2Body* previous = m_groundBody;
		float angle = 0.5f;
		Physics::Parameters bridgeParams = { b2BodyType::b2_dynamicBody, 2.0f, 0.7f, 0.0f, Physics::GroundCategory };
		for (size_t i = 0; i < bridgeBlocks; ++i)
		{
			m_bodies.push_back(Physics::CreateBoxAxisAligned(world, bridgeParams, topLeft, bottomRight, pos));
			b2RevoluteJoint* joint = Physics::CreateRevoluteJoint(previous, m_bodies.back(), topLeft + pos, angle, -angle);
			joint->EnableMotor(true);
			joint->SetMotorSpeed(0.0f);
			joint->SetMaxMotorTorque(10.0f);
			previous = m_bodies.back();
			topLeft.x += d.x;
			bottomRight.x += d.x;
		}
		Physics::CreateRevoluteJoint(previous, m_groundBody, topLeft + pos, angle, -angle);
	}

	virtual bool IsFullScreen() const { return (m_variant == 1) ? true : false; }

private:

};

