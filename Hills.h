#pragma once
#include "Box2D\Box2D.h"
#include "GameArea.h"

class Hills: public GameArea
{
public:

	Hills(b2World* world, const b2Vec2& pos, const Theme* theme, int variant) : GameArea(world, pos, theme, variant)
	{
		Physics::Parameters params = Physics::DefaultGroundParameters;
		if (m_variant == 0)
		{
			m_variant = std::rand() % 6 + 1;
		}
		const size_t pointCount = 65;

		const float segmentLength = 2.0f;
		float angle = b2_pi * 0.1;
		b2Vec2 points[pointCount];
		points[0] = b2Vec2_zero;
		points[1].Set(m_startZoneWidth, 0.0f);
		for (int i = 2; i < pointCount; ++i)
		{
			points[i].x = points[i - 1].x + segmentLength;
			float y = sinf((i - 1) * angle);
			switch (m_variant)
			{
			case 1:
			case 4:
				points[i].y = y * segmentLength * 1.5f;
				break;
			case 2:
			case 5:
				points[i].y = points[i - 1].y - abs(y) * segmentLength * 0.7f;
				break;
			case 3:
			case 6:
				points[i].y = points[i - 1].y + abs(y) * segmentLength * 0.7f;
				break;
			default:
				break;
			}
			if (m_variant < 4)
			{
				float angle = (m_theme->Style == Theme::e_urban) ? 0.0f : Physics::HorizontalAngle(points[i - 1], points[i]);
				float scale = 1.0f + 0.2f * y;
				m_decorations.emplace_back(points[i], 0.0f, angle, scale);
			}
			UpdateDimensions(points[i]);
		}
		m_end = points[pointCount - 1];

		m_groundBody = Physics::CreateChain(world, params, points, pointCount, pos);
	}

private:

};

