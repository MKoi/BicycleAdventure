#pragma once
#include <vector>
#include "Box2D\Box2D.h"
#include "GameArea.h"

using std::vector;

class Downhill: public GameArea
{
public:

	Downhill(b2World* world, const b2Vec2& pos, const Theme* theme, int variant) : GameArea(world, pos, theme, variant)
	{
		Physics::Parameters params = Physics::DefaultGroundParameters;
		if (m_variant == 0)
		{
			m_variant = std::rand() % 3 + 1;
		}

		const float startx = 10.0f;
		const float endx = (m_variant == 1) ? 80.0f : 50.0f;
		const float starty = 0.0f;
		const float factor = (m_variant == 3) ? -0.7f : -0.5f;
		const float endy = factor * (endx - startx);
		vector<b2Vec2> points;
		points.push_back(b2Vec2_zero);
		points.emplace_back(startx, starty);

		Vector2 p1(points.back().x, points.back().y);
		Vector2 p2(endx, endy);
		Vector2 t(p2.x - p1.x, 0.0f);
		float d = 2.0f / (endx - startx);
		float s = d;
		while (s < 1.0f)
		{
			Vector2 p3 = Vector2::Hermite(p1, t, p2, t, s);
			points.emplace_back(p3.x, p3.y);
			s += d;
		}
		points.emplace_back(p2.x, p2.y);

		m_end = points.back();
		float midy = 0.5f * (endy - starty);
		m_topLeft.Set(0.0f, midy + endx);
		m_bottomRight.Set(endx, midy - endx);
		m_groundBody = Physics::CreateChain(world, params, points.data(), points.size(), pos);
	}

	virtual bool IsFullScreen() const { return (m_variant != 1) ? true : false; }


private:
};
