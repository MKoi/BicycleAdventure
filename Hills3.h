#pragma once
#include <vector>
#include "Box2D\Box2D.h"
#include "SimpleMath.h"
#include "GameArea.h"

using std::vector;

class Hills3 : public GameArea
{
public:

	Hills3(b2World* world, const b2Vec2& pos, const Theme* theme, int variant) : GameArea(world, pos, theme, variant)
	{
		Physics::Parameters params = Physics::DefaultGroundParameters;
		if (m_variant == 0)
		{
			m_variant = std::rand() % 6 + 1;
		}
		vector<b2Vec2> controlPoints;
		switch (m_variant)
		{
		case 1:
		case 4:
			controlPoints.emplace_back(10.0f, -5.0f);
			controlPoints.emplace_back(50.0f, 20.0f);
			controlPoints.emplace_back(20.0f, -5.0f);
			controlPoints.emplace_back(30.0f, -20.0f);
			controlPoints.emplace_back(10.0f, -5.0f);
			break;
		case 2:
		case 5:
			controlPoints.emplace_back(5.0f, 2.0f);
			controlPoints.emplace_back(10.0f, -5.0f);
			controlPoints.emplace_back(20.0f, 7.0f);
			controlPoints.emplace_back(30.0f, -10.0f);
			controlPoints.emplace_back(40.0f, 15.0f);
			break;
		case 3:
		case 6:
			controlPoints.emplace_back(20.0f, 7.0f);
			controlPoints.emplace_back(15.0f, -10.0f);
			controlPoints.emplace_back(5.0f, 3.0f);
			controlPoints.emplace_back(10.0f, -3.0f);
			controlPoints.emplace_back(30.0f, 10.0f);
			controlPoints.emplace_back(10.0f, -3.0f);
			break;
		default:
			break;
		}

		vector<b2Vec2> points;
		points.push_back(b2Vec2_zero);
		points.emplace_back(m_startZoneWidth, 0.0f);
		for (const b2Vec2& p : controlPoints)
		{
			b2Vec2 start(points.back().x, points.back().y);
			b2Vec2 end(start.x + p.x, start.y + p.y);
			size_t count = max(5, static_cast<size_t>(0.5f * p.x));
			Graphics::CreateCurve(start, end, count, points);
			if (m_variant > 3)
			{
				float angle = 0.0f;
				float scale = 1.0f + 0.2f * sinf(end.x);
				m_decorations.emplace_back(b2Vec2(end.x, end.y), 0.0f, angle, scale);
			}
		}

		m_end = points.back();
		UpdateDimensions();
		m_groundBody = Physics::CreateChain(world, params, points.data(), points.size(), pos);
	}

private:

};
