#pragma once
#include "GameArea.h"
#include "Script.h"
#include "FallingBlock.h"

class BreakableWall2 : public GameArea
{
public:
	BreakableWall2(b2World* world, const b2Vec2& pos, const Theme* theme, int variant) : GameArea(world, pos, theme, variant),
		m_topLeft(20.0f, 5.0f),
		m_bottomRight(20.3f, 0.0f),
		m_sensorDelta(0.1f, 0.1f),
		m_fallingblocks(),
		m_fallingblocks2(),
		m_script([&]() {
		for (FallingBlock& b : m_fallingblocks)
		{
			b.m_block->SetType(b2BodyType::b2_dynamicBody);
		}
		}, world, m_topLeft - m_sensorDelta, m_bottomRight + m_sensorDelta, pos),
		m_script2()
	{
		Physics::Parameters params = Physics::DefaultGroundParameters;
		if (m_variant == 0)
		{
			m_variant = std::rand() % 2 + 1;
		}
		vector<b2Vec2> ground;
		ground.push_back(b2Vec2_zero);
		ground.emplace_back(40.0f, 0.0f);
		m_end = ground.back();
		m_groundBody = Physics::CreateChain(world, params, ground.data(), ground.size(), pos);
		UpdateDimensions();

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
		if (m_variant == 2)
		{
			float dif = 10.0f;
			b2Vec2 topleft2(m_topLeft.x + dif, m_topLeft.y);
			b2Vec2 bottomright2(m_bottomRight.x + dif, m_bottomRight.y);
			m_script2.SetAction([&]() {
				for (FallingBlock& b : m_fallingblocks2)
				{
					b.m_block->SetType(b2BodyType::b2_dynamicBody);
				}
			});
			m_script2.SetTrigger(world, topleft2 - m_sensorDelta, bottomright2 + m_sensorDelta, pos);
			for (size_t i = 0; i < blockCount; ++i)
			{
				m_fallingblocks2.emplace_back();
				m_fallingblocks2.back().m_block = Physics::CreateBoxAxisAligned(world, boxparams, topleft2, topleft2 + diag, pos);
				m_fallingblocks2.back().m_block->SetUserData(&m_fallingblocks2.back());
				m_bodies.push_back(m_fallingblocks2.back().m_block);
				topleft2.y -= dy;
			}
		}
	}

	virtual bool IsFullScreen() const { return true; }

	virtual void Update(float dt)
	{
		m_script.Update(dt);
		if (m_variant == 2)
		{
			m_script2.Update(dt);
		}
	}

	virtual void SaveState(Windows::Storage::Streams::DataWriter^ state) const
	{
		GameArea::SaveState(state);
		m_script.SaveState(state);
		if (m_variant == 2)
		{
			m_script2.SaveState(state);
		}
	}

	virtual void RestoreState(Windows::Storage::Streams::DataReader^ state)
	{
		GameArea::RestoreState(state);
		m_script.RestoreState(state);
		if (m_variant == 2)
		{
			m_script2.RestoreState(state);
		}
	}

private:
	const b2Vec2 m_topLeft;
	const b2Vec2 m_bottomRight;
	const b2Vec2 m_sensorDelta;
	std::vector<FallingBlock> m_fallingblocks;
	std::vector<FallingBlock> m_fallingblocks2;
	Script m_script;
	Script m_script2;

};
