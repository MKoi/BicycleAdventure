#pragma once
#include <vector>
#include "Box2D\Box2D.h"
#include "GameArea.h"

using std::vector;

class Downhill2 : public GameArea
{
public:

	Downhill2(b2World* world, const b2Vec2& pos, const Theme* theme, int variant) : GameArea(world, pos, theme, variant)
	{
		Physics::Parameters params = Physics::DefaultGroundParameters;
		if (m_variant == 0)
		{
			m_variant = std::rand() % 2 + 1;
		}
		const size_t controlPointCount = (m_variant == 1) ? 4 : 2;
		vector<b2Vec2> points;
		points.push_back(b2Vec2_zero);
		points.emplace_back(m_startZoneWidth, 0.0f);
		b2Vec2 d(40.0f, -20.0f);
		b2Vec2 start = points.back();
		for (size_t i = 0; i < controlPointCount; ++i)
		{
			b2Vec2 end = start + d;
			size_t count = static_cast<size_t>(0.5f * (end.x - start.x));
			Graphics::CreateCurve(start, end, count, points);
			d *= 0.6f;
			start = end;
		}
		m_end = points.back();
		UpdateDimensions();

		m_groundBody = Physics::CreateChain(world, params, points.data(), points.size(), pos);
	}

	virtual bool IsFullScreen() const { return false; }

private:
};
