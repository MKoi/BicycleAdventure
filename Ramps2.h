#include <vector>
#include "Box2D\Box2D.h"
#include "SimpleMath.h"
#include "GameArea.h"

class Ramps2 : public GameArea
{
public:

	Ramps2(b2World* world, const b2Vec2& pos, const Theme* theme, int variant) : GameArea(world, pos, theme, variant)
	{
		Physics::Parameters params = Physics::DefaultGroundParameters;
		if (m_variant == 0)
		{
			m_variant = std::rand() % 3 + 1;
		}
		vector<float> dy;
		switch (m_variant)
		{
		case 1:
			dy.push_back(5.0f);
			dy.push_back(10.0f);
			dy.push_back(15.0f);
			dy.push_back(10.0f);
			dy.push_back(5.0f);
			break;
		case 2:
			dy.push_back(7.0f);
			dy.push_back(0.0f);
			dy.push_back(7.0f);
			dy.push_back(0.0f);
			dy.push_back(7.0f);
			break;
		case 3:
			dy.push_back(3.0f);
			dy.push_back(6.0f);
			dy.push_back(12.0f);
			dy.push_back(24.0f);
			dy.push_back(0.0f);
		default:
			break;
		}

		vector<b2Vec2> points;
		points.push_back(b2Vec2_zero);
		points.emplace_back(m_startZoneWidth, 0.0f);
		for (auto y : dy)
		{
			b2Vec2 last = points.back();
			float dx;
			if (last.y < y)
			{
				dx = 4.0f * (y - last.y);
				if (m_theme->Style != Theme::e_rock && m_theme->Style != Theme::e_urban)
				{
					float dif = y - last.y;
					points.emplace_back(last.x + 0.25f * dx, last.y + 0.6f * dif);
					points.emplace_back(last.x + 0.4f * dx, last.y + 0.9f * dif);
				}
				points.emplace_back(last.x + 0.5f * dx, y);
				points.emplace_back(last.x + dx, y);
			}
			else
			{
				dx = 4.0f * (last.y - y);
				if (m_theme->Style != Theme::e_rock && m_theme->Style != Theme::e_urban)
				{
					float dif = last.y - y;
					points.emplace_back(last.x + 0.02f * dx, y + 0.95f * dif);
					points.emplace_back(last.x + 0.04f * dx, y + 0.85f * dif);
					points.emplace_back(last.x + 0.05f * dx, y);
				}
				else
				{
					points.emplace_back(last.x, y);
				}
				points.emplace_back(last.x + dx, y);
			}

			float scale = 1.0f + 0.2f * sinf(last.x);
			b2Vec2 decpos = b2Vec2(last.x + 0.75f * dx, y);
			m_decorations.emplace_back(decpos, 0.0f, 0.0f, scale);
			UpdateDimensions(points.back());
		}


		m_end = points.back();
		UpdateDimensions();
		m_groundBody = Physics::CreateChain(world, params, points.data(), points.size(), pos);
	}

private:

};
