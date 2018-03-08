#pragma once
#include <vector>
#include "Box2D\Box2D.h"
#include "GameArea.h"
#include "Script.h"

class Tunnel4 : public GameArea
{
public:
	Tunnel4(b2World* world, const b2Vec2& pos, const Theme* theme, int variant) : GameArea(world, pos, theme, variant)
	{
		Physics::Parameters params = Physics::DefaultGroundParameters;
		vector<b2Vec2> points;
		points.push_back(b2Vec2_zero);
		points.emplace_back(m_startZoneWidth, 0.0f);
		points.emplace_back(15.0f, 0.0f);
		points.emplace_back(25.0f, -5.0f);
		points.emplace_back(30.0f, -5.0f);
		points.emplace_back(35.0f, -2.0f);
		points.emplace_back(40.0f, -2.0f);
		points.emplace_back(50.0f, -10.0f);
		points.emplace_back(55.0f, -8.0f);
		points.emplace_back(60.0f, -6.0f);
		m_groundBody = Physics::CreateChain(world, params, points.data(), points.size(), pos);
		m_end = points.back();
		UpdateDimensions();

		points.clear();
		points.emplace_back(15.0f, 5.0f);
		points.emplace_back(25.0f, 0.0f);
		points.emplace_back(30.0f, 0.0f);
		points.emplace_back(35.0f, 3.0f);
		points.emplace_back(35.0f, 5.0f);
		m_bodies.push_back(Physics::CreatePolygon(world, params, points.data(), points.size(), pos));

		points.clear();
		points.emplace_back(35.0f, 5.0f);
		points.emplace_back(35.0f, 3.0f);
		points.emplace_back(40.0f, 3.0f);
		points.emplace_back(40.0f, 5.0f);
		m_bodies.push_back(Physics::CreatePolygon(world, params, points.data(), points.size(), pos));

		points.clear();
		points.emplace_back(40.0f, 5.0f);
		points.emplace_back(40.0f, 3.0f);
		points.emplace_back(50.0f, -5.0f);
		points.emplace_back(55.0f, -3.0f);
		points.emplace_back(55.0f, 5.0f);
		m_bodies.push_back(Physics::CreatePolygon(world, params, points.data(), points.size(), pos));

		b2Vec2 rooftopleft(15.0f, 15.0f);
		b2Vec2 roofbottomright(55.0f, 5.0f);
		b2Vec2 roofmiddle(0.5f * (rooftopleft.x + roofbottomright.x), 5.0f);
		points.clear();
		if (m_theme->Style == Theme::e_rock)
		{
			points.emplace_back(rooftopleft.x, roofbottomright.y);
			points.emplace_back(roofbottomright.x, roofbottomright.y);
			points.emplace_back(roofmiddle.x + 5.0f, rooftopleft.y);
			m_bodies.push_back(Physics::CreatePolygon(world, params, points.data(), points.size(), pos));
		}
		else if (m_theme->Style == Theme::e_urban)
		{
			m_bodies.push_back(Physics::CreateBoxAxisAligned(world, params, rooftopleft, roofbottomright, pos));
		}
		else
		{
			b2Vec2 perimeter(0.5f * (roofbottomright.x - rooftopleft.x), 0.0f);
			m_bodies.push_back(Physics::CreateHalfCircle(world, params, perimeter, pos + roofmiddle));
		}

	}

	virtual bool IsFullScreen() const { return true; }

private:
};
