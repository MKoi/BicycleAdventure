#pragma once
#include <vector>
#include "Box2D\Box2D.h"
#include "GameArea.h"
#include "MovingPlatform.h"

using std::vector;

class Elevator3 : public GameArea
{
public:

	Elevator3(b2World* world, const b2Vec2& pos, const Theme* theme, int variant) : GameArea(world, pos, theme, variant),
		m_platformpos(20.0f, 3.0f),
		m_platformdim(8.0f, -10.0f),
		m_platformpos2(m_platformpos.x + m_platformdim.x + 7.0f, m_platformpos.y + m_platformdim.y),
		m_elevator(world, pos, m_platformpos, m_platformdim, theme->Color.Lighter()),
		m_elevator2(world, pos, m_platformpos2, m_platformdim, theme->Color.Lighter())
	{
		Physics::Parameters params = Physics::DefaultGroundParameters;
		b2Vec2 platformroute = b2Vec2_zero;
		platformroute.y = m_platformdim.y;
		vector<b2Vec2> points;
		points.push_back(b2Vec2_zero);
		points.emplace_back(m_startZoneWidth, 0.0f);

		points.push_back(m_platformpos);
		points.emplace_back(m_platformpos.x, m_platformpos.y + platformroute.y + 0.1f);
		points.emplace_back(m_platformpos2.x, m_platformpos.y + platformroute.y + 0.1f);
		points.emplace_back(m_platformpos2.x, m_platformpos.y + 2.0f * platformroute.y + 0.2f);
		points.emplace_back(m_platformpos2.x + m_platformdim.x + 7.0f, m_platformpos.y + 2.0f * platformroute.y + 0.2f);
		m_end = points.back();
		m_groundBody = Physics::CreateChain(world, params, points.data(), points.size(), pos);

		UpdateDimensions();

		m_elevator.SetSpeed(1.8f);
		b2Vec2 endpoint = pos + platformroute;
		m_elevator.SetSwitchPoint(endpoint);
		m_elevator.SetStopTime(2.0f);

		m_elevator2.SetSpeed(2.0f);
		m_elevator2.SetSwitchPoint(endpoint);
		m_elevator2.SetStopTime(1.3f);


		b2Vec2 rooftopleft(m_platformpos.x - 5.0f, 10.0f);
		b2Vec2 roofbottomright(m_platformpos2.x + m_platformdim.x + 5.0f, 7.0f);
		b2Vec2 roofmiddle(0.5f * (rooftopleft.x + roofbottomright.x), 7.0f);
		b2Vec2 roof[4];
		if (m_theme->Style == Theme::e_rock)
		{
			roof[0].Set(rooftopleft.x, roofbottomright.y);
			roof[1].Set(roofbottomright.x, roofbottomright.y);
			roof[2].Set(roofmiddle.x + 5.0f, rooftopleft.y);
			m_bodies.push_back(Physics::CreatePolygon(world, params, roof, 3, pos));
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
		rooftopleft.Set(m_platformpos.x + m_platformdim.x, 7.0f);
		roofbottomright.Set(m_platformpos2.x + m_platformdim.x, m_platformpos.y + platformroute.y + 5.0f);
		m_bodies.push_back(Physics::CreateBoxAxisAligned(world, params, rooftopleft, roofbottomright, pos));
		rooftopleft.Set(m_platformpos2.x + m_platformdim.x, 7.0f);
		roofbottomright.Set(m_platformpos2.x + m_platformdim.x + 5.0f, m_end.y + 5.0f);
		m_bodies.push_back(Physics::CreateBoxAxisAligned(world, params, rooftopleft, roofbottomright, pos));
	}

	virtual bool IsFullScreen() const { return true; }

	virtual void Update(float dt)
	{
		m_elevator.Update(dt);
		m_elevator2.Update(dt);
	}

	virtual void Draw(Renderer^ renderBatch)
	{
		m_elevator.Draw(renderBatch);
		m_elevator2.Draw(renderBatch);
		GameArea::Draw(renderBatch);
	}

	virtual void SaveState(Windows::Storage::Streams::DataWriter^ state) const
	{
		GameArea::SaveState(state);
		m_elevator.SaveState(state);
		m_elevator2.SaveState(state);
	}

	virtual void RestoreState(Windows::Storage::Streams::DataReader^ state)
	{
		GameArea::RestoreState(state);
		m_elevator.RestoreState(state);
		m_elevator2.RestoreState(state);
	}


private:
	b2Vec2 m_platformpos;
	b2Vec2 m_platformdim;
	b2Vec2 m_platformpos2;
	MovingPlatform m_elevator;
	MovingPlatform m_elevator2;
};
