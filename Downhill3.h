#pragma once
#include <vector>
#include "Box2D\Box2D.h"
#include "GameArea.h"

using std::vector;

class Downhill3 : public GameArea
{
public:

	Downhill3(b2World* world, const b2Vec2& pos, const Theme* theme, int variant) : GameArea(world, pos, theme, variant)
	{
		Physics::Parameters params = Physics::DefaultGroundParameters;
		if (m_variant == 0)
		{
			m_variant = std::rand() % 3 + 1;
		}

		vector<b2Vec2> controlpoints;
		switch (m_variant)
		{
		case 1:
			controlpoints.emplace_back(10.0f, -3.0f);
			controlpoints.emplace_back(20.0f, -7.0f);
			controlpoints.emplace_back(10.0f, -3.0f);
			controlpoints.emplace_back(10.0f, -5.0f);
			break;
		case 2:
			controlpoints.emplace_back(30.0f, -15.0f);
			controlpoints.emplace_back(20.0f, -7.0f);
			controlpoints.emplace_back(10.0f, -5.0f);
			controlpoints.emplace_back(20.0f, -5.0f);
			break;
		case 3:
			controlpoints.emplace_back(30.0f, -20.0f);
			controlpoints.emplace_back(30.0f, -10.0f);
			controlpoints.emplace_back(30.0f, -5.0f);
		default:
			break;
		}
		vector<b2Vec2> points;
		points.push_back(b2Vec2_zero);
		points.emplace_back(m_startZoneWidth, 0.0f);
		for (auto p : controlpoints)
		{
			b2Vec2 start(points.back().x, points.back().y);
			b2Vec2 end(start.x + p.x, start.y + p.y);
			size_t count = max(5, static_cast<size_t>(0.5f * p.x));
			Graphics::CreateCurve(start, end, count, points);
		}
		m_end = points.back();
		UpdateDimensions();
		m_groundBody = Physics::CreateChain(world, params, points.data(), points.size(), pos);
	}

	virtual bool IsFullScreen() const { return false; }

private:
};
