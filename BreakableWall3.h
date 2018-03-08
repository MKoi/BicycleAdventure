#pragma once
#include "GameArea.h"
#include "Script.h"
#include "FallingBlock.h"

class BreakableWall3 : public GameArea
{
public:
	BreakableWall3(b2World* world, const b2Vec2& pos, const Theme* theme, int variant) : GameArea(world, pos, theme, variant),
		m_topLeft(30.0f, 15.0f),
		m_bottomRight(30.3f, 10.0f),
		m_sensorDelta(0.1f, 0.1f),
		m_fallingblocks(),
		m_script([&]() {
		for (FallingBlock& b : m_fallingblocks)
		{
			b.m_block->SetType(b2BodyType::b2_dynamicBody);
		}
	}, world, m_topLeft - m_sensorDelta, m_bottomRight + m_sensorDelta, pos)
	{
		Physics::Parameters params = Physics::DefaultGroundParameters;
		vector<b2Vec2> ground;
		ground.push_back(b2Vec2_zero);
		ground.emplace_back(m_startZoneWidth, 0.0f);
		ground.emplace_back(25.0f, 10.0f);
		ground.emplace_back(35.0f, 10.0f);
		ground.emplace_back(50.0f, 0.0f);
		m_end = ground.back();
		m_groundBody = Physics::CreateChain(world, params, ground.data(), ground.size(), pos);
		UpdateDimensions();

		vector<b2Vec2> peak;
		peak.emplace_back(27.5f, 15.0f);
		peak.emplace_back(32.5f, 15.0f);
		peak.emplace_back(30.0f, 18.0f);
		m_bodies.push_back(Physics::CreatePolygon(world, params, peak.data(), peak.size(), pos));

		const size_t blockCount = 3;
		const float dy = (m_topLeft.y - m_bottomRight.y) / blockCount;
		const float dx = m_bottomRight.x - m_topLeft.x;
		b2Vec2 diag(dx, -dy);
		b2Vec2 topLeft = m_topLeft;
		Physics::Parameters boxparams = { b2BodyType::b2_staticBody, 0.5f, 0.7f, 0.0f, Physics::WallCategory };
		for (size_t i = 0; i < blockCount; ++i)
		{
			m_fallingblocks.emplace_back();
			m_fallingblocks.back().m_block = Physics::CreateBoxAxisAligned(world, boxparams, topLeft, topLeft + diag, pos);
			m_fallingblocks.back().m_block->SetUserData(&m_fallingblocks.back());
			m_bodies.push_back(m_fallingblocks.back().m_block);
			topLeft.y -= dy;
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
	const b2Vec2 m_topLeft;
	const b2Vec2 m_bottomRight;
	const b2Vec2 m_sensorDelta;
	std::vector<FallingBlock> m_fallingblocks;
	Script m_script;

};
