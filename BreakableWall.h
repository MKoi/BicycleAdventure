#pragma once
#include "GameArea.h"
#include "Script.h"
#include "FallingBlock.h"

class BreakableWall : public GameArea
{
public:
	BreakableWall(b2World* world, const b2Vec2& pos, const Theme* theme, int variant) : GameArea(world, pos, theme, variant),
		m_triggerPos(30.0f, 5.0f),
		m_fallingblocks(),
		m_script([&]() {
			for (FallingBlock& b : m_fallingblocks)
			{
				b.m_block->SetType(b2BodyType::b2_dynamicBody);
			}
		}, world, pos + m_triggerPos, 5.0f)
	{
		Physics::Parameters params = Physics::DefaultGroundParameters;
		vector<b2Vec2> ground;
		ground.push_back(b2Vec2_zero);
		if (m_theme->Style == Theme::e_rock || m_theme->Style == Theme::e_urban)
		{
			ground.emplace_back(10.0f, 0.0f);
			ground.emplace_back(20.0f, 5.0f);
			ground.emplace_back(20.0f, -5.0f);
			ground.emplace_back(40.0f, -5.0f);
		}
		else
		{
			ground.emplace_back(10.0f, 0.0f);
			ground.emplace_back(15.0f, 3.0f);
			ground.emplace_back(18.0f, 4.5f);
			ground.emplace_back(20.0f, 5.0f);
			ground.emplace_back(21.0f, 0.0f);
			ground.emplace_back(22.0f, -3.0f);
			ground.emplace_back(23.0f, -5.0f);
			ground.emplace_back(40.0f, -5.0f);
		}
		m_end = ground.back();
		m_groundBody = Physics::CreateChain(world, params, ground.data(), ground.size(), pos);
		m_topLeft.Set(0.0f, 40.0f);
		m_bottomRight.Set(40.0f, -40.0f);

		const size_t blockCount = 5;
		const float dx = 15.0f / blockCount;
		b2Vec2 diag(dx, -0.3f);
		b2Vec2 topLeft(20.0f, 5.0f);
		for (size_t i = 0; i < blockCount; ++i)
		{
			m_fallingblocks.emplace_back();
			m_fallingblocks.back().m_block = Physics::CreateBoxAxisAligned(world, params, topLeft, topLeft + diag, pos);
			m_fallingblocks.back().m_block->SetUserData(&m_fallingblocks.back());
			m_bodies.push_back(m_fallingblocks.back().m_block);
			topLeft.x += dx;
		}
		topLeft.Set(35.0f, 20.0f);
		diag.Set(5.0f, -15.0f);
		m_bodies.push_back(Physics::CreateBoxAxisAligned(world, params, topLeft, topLeft + diag, pos));
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
	const b2Vec2 m_triggerPos;
	std::vector<FallingBlock> m_fallingblocks;
	Script m_script;

};
