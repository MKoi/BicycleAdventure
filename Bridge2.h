#pragma once
#include "Box2D\Box2D.h"
#include "GameArea.h"
#include "Script.h"

class Bridge2 : public GameArea
{
public:

	Bridge2(b2World* world, const b2Vec2& pos, const Theme* theme, int variant) : GameArea(world, pos, theme, variant),
		m_script([&]() {
		if (m_joint)
		{
			m_joint->EnableMotor(true);
			m_joint->SetMotorSpeed(0.2f);
			m_joint->SetMaxMotorTorque(2000.0f);
		}
		if (m_joint2)
		{
			m_joint2->EnableMotor(true);
			m_joint2->SetMotorSpeed(-0.2f);
			m_joint2->SetMaxMotorTorque(2000.0f);
		}
		}, world, pos + b2Vec2(5.0f, 0.0f), 4.0f)
	{
		Physics::Parameters params = Physics::DefaultGroundParameters;
		if (m_variant == 0)
		{
			m_variant = std::rand() % 2 + 1;
		}
		const size_t pointCount = 6;
		b2Vec2 points[pointCount];
		const float startx = 10.0f;
		const float endx = (m_variant == 1) ? 35.0f : 25.0f;
		points[0] = b2Vec2_zero;
		points[1].Set(startx, 0.0f);
		points[2].Set(startx, -30.0f);
		points[3].Set(endx, -30.0f);
		points[4].Set(endx, 0.0f);
		points[5].Set(endx + 10.0f, 0.0f);
		m_end = points[5];
		m_groundBody = Physics::CreateChain(world, params, points, pointCount, pos);

		float width = points[pointCount - 1].x - points[0].x;
		m_topLeft.Set(0.0f, width);
		m_bottomRight.Set(width, -width);

		const float gap = 0.3f;
		const float blockWidth = (m_variant == 1) ? 0.5f * (endx - startx - gap) : (endx - startx);
		b2Vec2 d(blockWidth, -1.0f);
		b2Vec2 topLeft = points[1];
		b2Vec2 bottomRight = topLeft + d;
		Physics::Parameters bridgeParams = { b2BodyType::b2_dynamicBody, 1.0f, 0.7f, 0.0f, Physics::GroundCategory };
		
		m_bodies.push_back(Physics::CreateBoxAxisAligned(world, bridgeParams, topLeft, bottomRight, pos));
		m_joint = Physics::CreateRevoluteJoint(m_groundBody, m_bodies.back(), topLeft + pos, 0.5f * b2_pi, 0.0f);

		if (m_variant == 1)
		{
			topLeft.x += d.x + gap;
			bottomRight.x += d.x + gap;
			m_bodies.push_back(Physics::CreateBoxAxisAligned(world, bridgeParams, topLeft, bottomRight, pos));
			topLeft.x += d.x;
			m_joint2 = Physics::CreateRevoluteJoint(m_groundBody, m_bodies.back(), topLeft + pos, 0.0f, -0.5f * b2_pi);
		}
	}

	virtual bool IsFullScreen() const { return true; }

	virtual void Update(float dt)
	{
		m_script.Update(dt);
	}

	virtual void SaveState(Windows::Storage::Streams::DataWriter^ state) const
	{
		GameArea::SaveState(state);
		m_script.SaveState(state);
	}

	virtual void RestoreState(Windows::Storage::Streams::DataReader^ state)
	{
		GameArea::RestoreState(state);
		m_script.RestoreState(state);
	}

private:
	b2RevoluteJoint* m_joint;
	b2RevoluteJoint* m_joint2;

	Script m_script;
};

