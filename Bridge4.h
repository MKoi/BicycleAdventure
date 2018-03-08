#pragma once
#include "Box2D\Box2D.h"
#include "GameArea.h"
#include "Script.h"
#include "Water.h"

class Bridge4 : public GameArea
{
public:

	Bridge4(b2World* world, const b2Vec2& pos, const Theme* theme, int variant) : GameArea(world, pos, theme, variant),
		m_fallingblock(), m_water(world)
	{
		Physics::Parameters params = Physics::DefaultGroundParameters;
		vector<b2Vec2> points;

		points.push_back(b2Vec2_zero);
		points.emplace_back(15.0f, 0.0f);
		points.emplace_back(15.0f, -10.0f);
		points.emplace_back(52.0f, -10.0f);
		points.emplace_back(52.0f, 0.0f);
		points.emplace_back(60.0f, 0.0f);
		m_end = points.back();
		m_groundBody = Physics::CreateChain(world, params, points.data(), points.size(), pos);
		UpdateDimensions();

		b2Vec2 water[4];
		water[0].Set(15.0f, -1.0f);
		water[1].Set(15.0f, -10.0f);
		water[2].Set(52.0f, -10.0f);
		water[3].Set(52.0f, -1.0f);
		m_water.Set(world, pos, water, 4, m_audio);

		m_bodies.push_back(Physics::CreateLineFromCorners(world, params, b2Vec2(15.0f, 0.0f), b2Vec2(20.0f, 3.0f), 0.2f, pos));
		m_bodies.push_back(Physics::CreateLineFromCorners(world, params, b2Vec2(20.0f, 3.0f), b2Vec2(25.0f, 5.0f), 0.2f, pos));
		m_bodies.push_back(Physics::CreateLineFromCorners(world, params, b2Vec2(25.0f, 5.0f), b2Vec2(30.0f, 6.0f), 0.2f, pos));
		m_bodies.push_back(Physics::CreateLineFromCorners(world, params, b2Vec2(30.0f, 6.0f), b2Vec2(31.0f, 6.0f), 1.0f, pos));
		m_fallingblock.m_block = Physics::CreateLineFromCorners(world, params, b2Vec2(31.1f, 6.0f), b2Vec2(36.0f, 6.0f), 0.2f, pos);
		m_bodies.push_back(m_fallingblock.m_block);
		m_bodies.push_back(Physics::CreateLineFromCorners(world, params, b2Vec2(36.0f, 6.0f), b2Vec2(37.0f, 6.0f), 1.0f, pos));
		m_bodies.push_back(Physics::CreateLineFromCorners(world, params, b2Vec2(37.0f, 6.0f), b2Vec2(42.0f, 5.0f), 0.2f, pos));
		m_bodies.push_back(Physics::CreateLineFromCorners(world, params, b2Vec2(42.0f, 5.0f), b2Vec2(47.0f, 3.0f), 0.2f, pos));
		m_bodies.push_back(Physics::CreateLineFromCorners(world, params, b2Vec2(47.0f, 3.0f), b2Vec2(52.0f, 0.0f), 0.2f, pos));
		
		m_fallingblock.m_script.reset(new Script(
			[&]()
			{
				m_fallingblock.m_block->SetType(b2BodyType::b2_dynamicBody);
			},
			world,
			pos + b2Vec2(25.0f, 5.0f),
			5.0f,
			Physics::PlayerCategory)
		);
	}

	virtual bool IsFullScreen() const { return true; }

	virtual void Draw(Renderer^ renderBatch)
	{
		m_water.Draw(renderBatch);
		GameArea::Draw(renderBatch);
	}

	virtual void Update(float dt)
	{
		m_fallingblock.m_script->Update(dt);
		m_water.Update(dt);
	}

	virtual void SaveState(Windows::Storage::Streams::DataWriter^ state) const
	{
		GameArea::SaveState(state);
		m_fallingblock.m_script->SaveState(state);
		m_water.SaveState(state);
	}

	virtual void RestoreState(Windows::Storage::Streams::DataReader^ state)
	{
		GameArea::RestoreState(state);
		m_fallingblock.m_script->RestoreState(state);
		m_water.RestoreState(state);
	}


private:
	struct BridgePart
	{
		b2Body* m_block;
		std::shared_ptr<Script> m_script;
	};

	BridgePart m_fallingblock;
	Water m_water;
};
