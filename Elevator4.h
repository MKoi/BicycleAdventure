#pragma once
#include <vector>
#include "Box2D\Box2D.h"
#include "GameArea.h"
#include "MovingPlatform.h"
#include "Water.h"

class Elevator4 : public GameArea
{
public:

	Elevator4(b2World* world, const b2Vec2& pos, const Theme* theme, int variant) : GameArea(world, pos, theme, variant),
		m_platformpos(13.0f, 0.0f),
		m_platformdim(7.0f, -20.0f),
		m_elevator(world, pos, m_platformpos, m_platformdim, theme->Color.Lighter()),
		m_water(world)
	{
		Physics::Parameters params = Physics::DefaultGroundParameters;
		b2Vec2 platformroute = b2Vec2_zero;
		platformroute.y = 0.95f * abs(m_platformdim.y);
		m_elevator.SetSpeed(2.5f);
		b2Vec2 endpoint = pos + platformroute;
		m_elevator.SetSwitchPoint(endpoint);
		m_elevator.SetStopTime(2.0f);

		vector<b2Vec2> points;
		points.push_back(b2Vec2_zero);
		points.emplace_back(m_startZoneWidth, 0.0f);
		points.emplace_back(m_platformpos.x + m_platformdim.x, 0.0f);
		points.emplace_back(m_platformpos.x + m_platformdim.x, platformroute.y - 0.1f);
		b2Vec2 curvestart = points.back();
		b2Vec2 curveend(35.0f, 2.5f);
		Graphics::CreateCurve(curvestart, curveend, 10, points);
		curvestart = points.back();
		curveend.Set(40.0f, 5.0f);
		Graphics::CreateCurve(curvestart, curveend, 5, points);
		points.emplace_back(40.0f, -5.0f);
		points.emplace_back(50.0f, -5.0f);
		points.emplace_back(50.0f, 0.0f);
		points.emplace_back(55.0f, 0.0f);
		m_end = points.back();
		UpdateDimensions();
		m_groundBody = Physics::CreateChain(world, params, points.data(), points.size(), pos);

		b2Vec2 water[4];
		water[0].Set(40.0f, 0.0f);
		water[1].Set(40.0f, -5.0f);
		water[2].Set(50.0f, -5.0f);
		water[3].Set(50.0f, 0.0f);
		m_water.Set(world, pos, water, 4, m_audio);

	}

	virtual bool IsFullScreen() const { return true; }

	virtual void Update(float dt)
	{
		m_elevator.Update(dt);
		m_water.Update(dt);
	}

	virtual void Draw(Renderer^ renderBatch)
	{
		m_elevator.Draw(renderBatch);
		m_water.Draw(renderBatch);
		GameArea::Draw(renderBatch);
	}

	virtual void SaveState(Windows::Storage::Streams::DataWriter^ state) const
	{
		GameArea::SaveState(state);
		m_elevator.SaveState(state);
		m_water.SaveState(state);
	}

	virtual void RestoreState(Windows::Storage::Streams::DataReader^ state)
	{
		GameArea::RestoreState(state);
		m_elevator.RestoreState(state);
		m_water.RestoreState(state);
	}


private:
	b2Vec2 m_platformpos;
	b2Vec2 m_platformdim;
	MovingPlatform m_elevator;
	Water m_water;
};

