#pragma once
#include "Box2D\Box2D.h"
#include "GameArea.h"
#include "Script.h"
#include "FallingBlock.h"

class Tunnel : public GameArea
{
public:
	Tunnel(b2World* world, const b2Vec2& pos, const Theme* theme, int variant) : GameArea(world, pos, theme, variant)
	{
		Physics::Parameters params = Physics::DefaultGroundParameters;
		if (m_variant == 0)
		{
			m_variant = std::rand() % 2 + 1;
		}
		float roofY = 5.0f;
		b2Vec2 ground[8];
		size_t groundpoints = 0;
		ground[0] = b2Vec2_zero;
		if (m_variant == 1)
		{
			ground[1].Set(50.0f, 0.0f);
			m_end = ground[1];
			m_groundBody = Physics::CreateChain(world, params, ground, 2, pos);
		}
		else if (m_variant == 2)
		{
			float floorY = -3.5f;
			ground[1].Set(12.0f, 0.0f);
			
			if (m_theme->Style == Theme::e_rock || m_theme->Style == Theme::e_urban)
			{
				ground[2].Set(20.0f, floorY);
				ground[3].Set(40.0f, floorY);
				ground[4].Set(48.0f, 0.0f);
				ground[5].Set(50.0f, 0.0f);
				m_end = ground[5];
				groundpoints = 6;
			}
			else
			{
				ground[2].Set(16.0f, 0.3f * floorY);
				ground[3].Set(20.0f, floorY);
				ground[4].Set(40.0f, floorY);
				ground[5].Set(44.0f, 0.3f * floorY);
				ground[6].Set(48.0f, 0.0f);
				ground[7].Set(50.0f, 0.0f);			
				m_end = ground[7];
				groundpoints = 8;
			}		
			m_groundBody = Physics::CreateChain(world, params, ground, groundpoints, pos);
			roofY = 2.0f;
		}
		m_decorations.emplace_back(b2Vec2(5.0f, 0.0f), 0.0f);
		m_decorations.emplace_back(b2Vec2(10.0f, 0.0f), 0.0f, 0.0f, 1.2f);
		m_decorations.emplace_back(b2Vec2(48.0f, 0.0f), 0.0f);
	
		m_topLeft.Set(0.0f, 50.0f);
		m_bottomRight.Set(50.0f, -50.0f);

		if (m_theme->Style == Theme::e_rock)
		{
			b2Vec2 roof[3];
			roof[0].Set(20.0f, roofY);
			roof[1].Set(40.0f, roofY);
			roof[2].Set(30.0f, roofY + 15.0f);
			m_bodies.push_back(Physics::CreatePolygon(world, params, roof, 3, pos));
		}
		else if (m_theme->Style == Theme::e_urban)
		{
			b2Vec2 roof1(20.0f, roofY + 7.0f);
			b2Vec2 roof2(40.0f, roofY);
			m_bodies.push_back(Physics::CreateBoxAxisAligned(world, params, roof1, roof2, pos));
		}
		else
		{
			b2Vec2 roofcenter(30.0f, roofY);
			b2Vec2 perimeter(10.0f, 0.0f);
			m_bodies.push_back(Physics::CreateHalfCircle(world, params, perimeter, pos + roofcenter));
			m_decorations.emplace_back(b2Vec2(roofcenter.x, roofY + perimeter.x), 0.0f);
		}

		if (m_theme->Style == Theme::e_rock || m_theme->Style == Theme::e_urban)
		{
			const size_t max = 5;
			m_fallingblocks.resize(max);
			const b2Vec2 roofstart(20.0f, roofY);
			const float rooflength = 20.0f;
			const float dx = rooflength / max;
			const float dy = -1.0f;
			b2Vec2 topLeft(roofstart);
			b2Vec2 bottomRight(roofstart.x + dx, roofstart.y + dy);
			for (size_t i = 0; i < m_fallingblocks.size(); ++i)
			{
				m_fallingblocks.at(i).m_block = Physics::CreateBoxAxisAligned(world, params, topLeft, bottomRight, pos);
				m_fallingblocks.at(i).m_block->SetUserData(&m_fallingblocks.at(i));
				m_bodies.push_back(m_fallingblocks.at(i).m_block);
				m_fallingblocks.at(i).m_script.reset(new Script(
					[&, i]()
					{
					m_fallingblocks[i].m_block->SetType(b2BodyType::b2_dynamicBody);
					},
					world,
					pos + bottomRight,
					2.0f));
				topLeft.x += dx;
				bottomRight.x += dx;
			}
		}
	}

	virtual void Update(float dt)
	{
		for (auto &i : m_fallingblocks)
		{
			i.m_script->Update(dt);
		}
	}

	virtual void SaveState(Windows::Storage::Streams::DataWriter^ state) const
	{
		GameArea::SaveState(state);
		for (const auto &i : m_fallingblocks)
		{
			i.m_script->SaveState(state);
		}
	}

	virtual void RestoreState(Windows::Storage::Streams::DataReader^ state)
	{
		GameArea::RestoreState(state);
		for (auto &i : m_fallingblocks)
		{
			i.m_script->RestoreState(state);
		}
	}

	virtual bool IsFullScreen() const { return true; }

private:
	class ScriptedBlock : public FallingBlock
	{
	public:
		std::shared_ptr<Script> m_script;
	};

	std::vector<ScriptedBlock> m_fallingblocks;
};
