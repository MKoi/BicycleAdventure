#pragma once
#include "Box2D\Box2D.h"
#include "GameArea.h"
#include "Script.h"
#include "Water.h"

class Bridge3 : public GameArea
{
public:

	Bridge3(b2World* world, const b2Vec2& pos, const Theme* theme, int variant) : GameArea(world, pos, theme, variant),
		m_fallingblocks(), m_water(world)
	{
		Physics::Parameters params = Physics::DefaultGroundParameters;
		if (m_variant == 0)
		{
			m_variant = std::rand() % 2 + 1;
		}
		vector<b2Vec2> points;

		points.push_back(b2Vec2_zero);
		
		if (m_variant == 2)
		{
			points.emplace_back(15.0f, 0.0f);
			points.emplace_back(20.0f, 2.0f);
		}
		else
		{
			points.emplace_back(20.0f, 0.0f);
		}
		points.emplace_back(20.0f, -5.0f);
		points.emplace_back(50.0f, -5.0f);
		if (m_variant == 2)
		{
			points.emplace_back(50.0f, 2.0f);
			points.emplace_back(55.0f, 0.0f);
		}
		else
		{
			points.emplace_back(50.0f, 0.0f);
		}
		points.emplace_back(60.0f, 0.0f);
		m_end = points.back();
		m_groundBody = Physics::CreateChain(world, params, points.data(), points.size(), pos);
		UpdateDimensions();

		b2Vec2 water[4];
		water[0].Set(20.0f, -1.0f);
		water[1].Set(20.0f, -5.0f);
		water[2].Set(50.0f, -5.0f);
		water[3].Set(50.0f, -1.0f);
		m_water.Set(world, pos, water, 4, m_audio);

		const size_t max = 5;
		m_fallingblocks.resize(max);
		const b2Vec2 roofstart(20.0f, m_variant == 2 ? 2.0f : 0.0f);
		const float rooflength = 30.0f;
		const float dx = rooflength / max;
		const float dy = -0.3f;
		b2Vec2 topLeft(roofstart);
		b2Vec2 bottomRight(roofstart.x + dx, roofstart.y + dy);
		float triggerRadius = 1.0f;
		b2Vec2 triggerOffset(triggerRadius, 0.0f);
		for (size_t i = 0; i < m_fallingblocks.size(); ++i)
		{
			m_fallingblocks.at(i).m_block = Physics::CreateBoxAxisAligned(world, params, topLeft, bottomRight, pos);
			m_bodies.push_back(m_fallingblocks.at(i).m_block);
			m_fallingblocks.at(i).m_script.reset(new Script(
				[&, i]()
				{
					m_fallingblocks[i].m_block->SetType(b2BodyType::b2_dynamicBody);
				},
				world,
				pos + bottomRight + triggerOffset,
				triggerRadius,
				Physics::PlayerCategory));
			m_fallingblocks.at(i).m_script->SetMinMass(0.01f);
			topLeft.x += dx;
			bottomRight.x += dx;
		}
	}

	virtual bool IsFullScreen() const { return false; }

	virtual void Draw(Renderer^ renderBatch)
	{
		m_water.Draw(renderBatch);
		GameArea::Draw(renderBatch);
	}

	virtual void Update(float dt)
	{
		for (auto &i : m_fallingblocks)
		{
			i.m_script->Update(dt);
		}
		m_water.Update(dt);
	}

	virtual void SaveState(Windows::Storage::Streams::DataWriter^ state) const
	{
		GameArea::SaveState(state);
		for (const auto &i : m_fallingblocks)
		{
			i.m_script->SaveState(state);
		}
		m_water.SaveState(state);
	}

	virtual void RestoreState(Windows::Storage::Streams::DataReader^ state)
	{
		GameArea::RestoreState(state);
		for (const auto &i : m_fallingblocks)
		{
			i.m_script->RestoreState(state);
		}
		m_water.RestoreState(state);
	}


private:
	struct BridgePart
	{
		b2Body* m_block;
		std::shared_ptr<Script> m_script;
	};

	std::vector<BridgePart> m_fallingblocks;
	Water m_water;
};
