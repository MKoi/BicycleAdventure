#pragma once
#include "Box2D\Box2D.h"
#include "SimpleMath.h"
#include "GameArea.h"

class Hills2 : public GameArea
{
public:

	Hills2(b2World* world, const b2Vec2& pos, const Theme* theme, int variant) : GameArea(world, pos, theme, variant)
	{
		Physics::Parameters params = Physics::DefaultGroundParameters;
		if (m_variant == 0)
		{
			m_variant = std::rand() % 6 + 1;
		}
		const size_t controlPointCount = 8;
		b2Vec2 controlPoints[controlPointCount];
		switch (m_variant)
		{
		case 1:
		case 4:
			controlPoints[0].Set(10.0f, 3.0f);
			controlPoints[1].Set(20.0f, -5.0f);
			controlPoints[2].Set(5.0f, -5.0f);
			controlPoints[3].Set(30.0f, 10.0f);
			controlPoints[4].Set(10.0f, -3.0f);
			controlPoints[5].Set(10.0f, 4.0f);
			controlPoints[6].Set(20.0f, 10.0f);
			controlPoints[7].Set(10.0f, -3.0f);
			break;
		case 2:
		case 5:
			controlPoints[0].Set(10.0f, -4.0f);
			controlPoints[1].Set(5.0f, -1.0f);
			controlPoints[2].Set(5.0f, -5.0f);
			controlPoints[3].Set(30.0f, -10.0f);
			controlPoints[4].Set(10.0f, 2.0f);
			controlPoints[5].Set(10.0f, 4.0f);
			controlPoints[6].Set(20.0f, 3.0f);
			controlPoints[7].Set(10.0f, -2.0f);
			break;
		case 3:
		case 6:
			controlPoints[0].Set(20.0f, 3.0f);
			controlPoints[1].Set(20.0f, -5.0f);
			controlPoints[2].Set(20.0f, 8.0f);
			controlPoints[3].Set(20.0f, -20.0f);
			controlPoints[4].Set(20.0f, 5.0f);
			controlPoints[5].Set(20.0f, -5.0f);
			controlPoints[6].Set(20.0f, 10.0f);
			controlPoints[7].Set(20.0f, -15.0f);
			break;
		default:
			break;
		}

		vector<b2Vec2> points;
		points.push_back(b2Vec2_zero);
		points.emplace_back(m_startZoneWidth, 0.0f);
		//const float dx = 2.0f;
		Vector2 t(0.0f);
		for (int i = 0; i < controlPointCount; ++i)
		{
			Vector2 p1(points.back().x, points.back().y);
			Vector2 p2(p1.x + controlPoints[i].x, p1.y + controlPoints[i].y);
			float d = 0.1f;
			float s = d;
			t.x = p2.x - p1.x;
			Vector2 p3;
			while (s < 1.0f)
			{
				p3 = Vector2::Hermite(p1, t, p2, t, s);
				points.emplace_back(p3.x, p3.y);
				s += d;
			}
			points.emplace_back(p2.x, p2.y);
			if (m_variant > 3)
			{
				float angle = (m_theme->Style == Theme::e_urban) ? 0.0f : Physics::HorizontalAngle(p3, p2);
				float scale = 1.0f + 0.2f * sinf(p2.x);
				m_decorations.emplace_back(b2Vec2(p2.x, p2.y), 0.0f, angle, scale);
			}
			UpdateDimensions(points.back());
		}

		
		m_end = points.back();
		m_groundBody = Physics::CreateChain(world, params, points.data(), points.size(), pos);
	}

private:

};

