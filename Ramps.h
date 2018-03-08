#include "Box2D\Box2D.h"
#include "SimpleMath.h"
#include "GameArea.h"

class Ramps : public GameArea
{
public:

	Ramps(b2World* world, const b2Vec2& pos, const Theme* theme, int variant) : GameArea(world, pos, theme, variant)
	{
		Physics::Parameters params = Physics::DefaultGroundParameters;
		if (m_variant == 0)
		{
			m_variant = std::rand() % 4 + 1;
		}
		const size_t controlPointCount = 8;
		float dy[controlPointCount];
		switch (m_variant)
		{
		case 1:
		case 3:
			dy[0] = 10.0f;
			dy[1] = 5.0f;
			dy[2] = 0.0f;
			dy[3] = 4.0f;
			dy[4] = 8.0f;
			dy[5] = 12.0f;
			dy[6] = 2.0f;
			dy[7] = 8.0f;
			break;
		case 2:
		case 4:
			dy[0] = -4.0f;
			dy[1] = 4.0f;
			dy[2] = 0.0f;
			dy[3] = 4.0f;
			dy[4] = -4.0f;
			dy[5] = -8.0f;
			dy[6] = -4.0f;
			dy[7] = 0.0f;
			break;
		default:
			break;
		}

		vector<b2Vec2> points;
		points.push_back(b2Vec2_zero);
		points.emplace_back(m_startZoneWidth, 0.0f);
		for (int i = 0; i < controlPointCount; ++i)
		{
			b2Vec2 last = points.back();
			float dx;
			if (last.y < dy[i])
			{
				dx = 4.0f * (dy[i] - last.y);
				if (m_theme->Style != Theme::e_rock && m_theme->Style != Theme::e_urban)
				{
					float dif = dy[i] - last.y;
					points.emplace_back(last.x + 0.25f * dx, last.y + 0.6f * dif);
					points.emplace_back(last.x + 0.4f * dx, last.y + 0.9f * dif);
				}
				points.emplace_back(last.x + 0.5f * dx, dy[i]);
				points.emplace_back(last.x + dx, dy[i]);
			}
			else
			{
				dx = 4.0f * (last.y - dy[i]);
				if (m_theme->Style != Theme::e_rock && m_theme->Style != Theme::e_urban)
				{
					float dif = last.y - dy[i];
					points.emplace_back(last.x + 0.02f * dx, dy[i] + 0.95f * dif);
					points.emplace_back(last.x + 0.04f * dx, dy[i] + 0.85f * dif);
					points.emplace_back(last.x + 0.05f * dx, dy[i]);
				}
				else
				{
					points.emplace_back(last.x, dy[i]);
				}
				points.emplace_back(last.x + dx, dy[i]);
			}
			if (m_variant > 2)
			{
				float scale = 1.0f + 0.2f * sinf(last.x);
				b2Vec2 decpos = b2Vec2(last.x + 0.75f * dx, dy[i]);
				m_decorations.emplace_back(decpos, 0.0f, 0.0f, scale);
			}
			
			UpdateDimensions(points.back());
		}


		m_end = points.back();
		m_groundBody = Physics::CreateChain(world, params, points.data(), points.size(), pos);
	}

private:

};
