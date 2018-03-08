#pragma once
#include "Box2D\Box2D.h"
#include "GameArea.h"
#include "MovingPlatform.h"

class Elevator : public GameArea
{
public:

	Elevator(b2World* world, const b2Vec2& pos, const Theme* theme, int variant) : GameArea(world, pos, theme, variant),
		m_platformpos(25.0f, 0.0f),
		m_platformdim(10.0f, -10.0f),
		m_elevator(world, pos, m_platformpos, m_platformdim, theme->Color.Lighter())
	{
		Physics::Parameters params = Physics::DefaultGroundParameters;
		if (m_variant == 0)
		{
			m_variant = std::rand() % 3 + 1;
		}
		b2Vec2 platformroute = b2Vec2_zero;
		platformroute.y = (m_variant != 1) ? 0.9f * abs(m_platformdim.y) : 0.5f * abs(m_platformdim.y);
		const size_t pointCount = 4;
		b2Vec2 points[pointCount];
		points[0] = b2Vec2_zero;
		points[1].Set(m_platformpos.x + m_platformdim.x, 0.0f);
		points[2].Set(m_platformpos.x + m_platformdim.x, platformroute.y - 0.2f);
		points[3].Set(m_platformpos.x + m_platformdim.x + 10.0f, platformroute.y - 0.2f);
		m_end = points[pointCount - 1];
		m_groundBody = Physics::CreateChain(world, params, points, pointCount, pos);

		b2Vec2 mid = 0.5f * (m_end + m_start);
		float width = m_end.x - m_start.x;
		m_topLeft.Set(m_start.x, mid.y + width);
		m_bottomRight.Set(m_end.x, mid.y - width);

		m_elevator.SetSpeed(1.8f);
		b2Vec2 endpoint = pos + platformroute;
		m_elevator.SetSwitchPoint(endpoint);
		m_elevator.SetStopTime(2.0f);

		if (m_variant == 3)
		{
			b2Vec2 rooftopleft(m_platformpos.x - m_platformdim.x, m_end.y + 15.0f);
			b2Vec2 roofbottomright(m_platformpos.x + m_platformdim.x, m_end.y + 5.0f);
			b2Vec2 roofmiddle(m_platformpos.x, m_end.y + 5.0f);
			b2Vec2 roof[4];
			if (m_theme->Style == Theme::e_rock)
			{		
				roof[0].Set(rooftopleft.x, roofbottomright.y);
				roof[1].Set(roofbottomright.x, roofbottomright.y);
				roof[2].Set(roofmiddle.x, rooftopleft.y);
				m_bodies.push_back(Physics::CreatePolygon(world, params, roof, 3, pos));
			}
			else if (m_theme->Style == Theme::e_urban)
			{
				m_bodies.push_back(Physics::CreateBoxAxisAligned(world, params, rooftopleft, roofbottomright, pos));
			}
			else
			{
				b2Vec2 perimeter(m_platformdim.x, 0.0f);
				m_bodies.push_back(Physics::CreateHalfCircle(world, params, perimeter, pos + roofmiddle));
			}
			rooftopleft.y = roofbottomright.y;
			roofbottomright.x = m_platformpos.x; // -2.0f;
			roofbottomright.y = min(5.0f, m_end.y);
			m_bodies.push_back(Physics::CreateBoxAxisAligned(world, params, rooftopleft, roofbottomright, pos));
		}
	}

	virtual bool IsFullScreen() const { return true; }

	virtual void Update(float dt)
	{
		m_elevator.Update(dt);
	}

	virtual void Draw(Renderer^ renderBatch)
	{
		m_elevator.Draw(renderBatch);
		GameArea::Draw(renderBatch);
	}

	virtual void SaveState(Windows::Storage::Streams::DataWriter^ state) const
	{
		GameArea::SaveState(state);
		m_elevator.SaveState(state);
	}

	virtual void RestoreState(Windows::Storage::Streams::DataReader^ state)
	{
		GameArea::RestoreState(state);
		m_elevator.RestoreState(state);
	}


private:
	b2Vec2 m_platformpos;
	b2Vec2 m_platformdim;
	MovingPlatform m_elevator;
};

