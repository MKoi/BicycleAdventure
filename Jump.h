#pragma once
#include "Box2D\Box2D.h"
#include "GameArea.h"
#include "Water.h"

class Jump : public GameArea
{
public:

	Jump(b2World* world, const b2Vec2& pos, const Theme* theme, int variant) : GameArea(world, pos, theme, variant),
		m_water(world)
	{
		Physics::Parameters params = Physics::DefaultGroundParameters;
		float angle = b2_pi * 0.11f;
		b2Vec2 points[b2_maxPolygonVertices];
		b2Vec2 endpoint(50.0f, 0.0f);
		size_t pointcount = 0;
		points[0] = b2Vec2_zero;
		if (m_theme->Style != Theme::e_rock && m_theme->Style != Theme::e_urban)
		{
			
			points[1].Set(23.0f, 0.0f);
			points[2].Set(25.5f, -5.0f);
			points[3].Set(28.0f, 0.0f);
			points[4] = endpoint;
			m_end = endpoint;
			pointcount = 5;
		}
		else
		{
			points[1] = endpoint;
			m_end = endpoint;
			pointcount = 2;
		}
		
		m_topLeft.Set(0.0f, endpoint.x);
		m_bottomRight.Set(endpoint.x, -endpoint.x);
		m_groundBody = Physics::CreateChain(world, params, points, pointcount, pos);

		b2Vec2 water[4];
		if (m_theme->Style == Theme::e_rock || m_theme->Style == Theme::e_urban)
		{
			points[0].Set(10.0f, 0.0f);
			points[1].Set(20.0f, 0.0f);
			points[2].Set(20.0f, 5.0f);
			m_bodies.push_back(Physics::CreatePolygon(world, params, points, 3, pos));
			points[0].Set(30.0f, 0.0f);
			points[1].Set(40.0f, 0.0f);
			points[2].Set(30.0f, 5.0f);
			m_bodies.push_back(Physics::CreatePolygon(world, params, points, 3, pos));
			water[0].Set(20.0f, 4.5f);
			water[1].Set(20.0f, 0.0f);
			water[2].Set(30.0f, 0.0f);
			water[3].Set(30.0f, 4.5f);
			m_water.Set(world, pos, water, 4, m_audio);
		}
		else
		{
			points[0].Set(14.0f, 0.0f);
			points[1].Set(15.5f, 1.0f);
			points[2].Set(17.5f, 2.0f);
			points[3].Set(19.5f, 2.75f);
			points[4].Set(21.5f, 3.0f);
			points[5].Set(22.5f, 1.0f);
			points[6].Set(23.0f, 0.0f);
			m_bodies.push_back(Physics::CreatePolygon(world, params, points, 7, pos));

			points[0].Set(28.0f, 0.0f);
			points[1].Set(28.5f, 1.0f);
			points[2].Set(29.5f, 3.0f);
			points[3].Set(31.5f, 2.75f);
			points[4].Set(33.5f, 2.0f);
			points[5].Set(35.5f, 1.0f);
			points[6].Set(37.0f, 0.0f);
			m_bodies.push_back(Physics::CreatePolygon(world, params, points, 7, pos));

			water[0].Set(23.0f, 0.0f);
			water[1].Set(25.5f, -5.0f);
			water[2].Set(28.0f, 0.0f);
			m_water.Set(world, pos, water, 3, m_audio);
		}
		m_decorations.emplace_back(b2Vec2(5.0f, 0.0f));
		m_decorations.emplace_back(b2Vec2(10.0f, 0.0f));
		m_decorations.emplace_back(b2Vec2(25.0f, 0.0f));
		m_decorations.emplace_back(b2Vec2(40.0f, 0.0f));
		m_decorations.emplace_back(b2Vec2(45.0f, 0.0f));
		
	}

	virtual void Update(float dt)
	{
		m_water.Update(dt);
	}

	virtual void Draw(Renderer^ renderBatch)
	{
		m_water.Draw(renderBatch);
		GameArea::Draw(renderBatch);
	}

	virtual void SaveState(Windows::Storage::Streams::DataWriter^ state) const
	{
		GameArea::SaveState(state);
		m_water.SaveState(state);
	}

	virtual void RestoreState(Windows::Storage::Streams::DataReader^ state)
	{
		GameArea::RestoreState(state);
		m_water.RestoreState(state);
		m_water.Update(0.0f);
	}

	virtual bool IsFullScreen() const { return true; }
private:
	Water m_water;
};
