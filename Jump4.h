#pragma once
#include <vector>
#include "Box2D\Box2D.h"
#include "GameArea.h"
#include "Water.h"

class Jump4 : public GameArea
{
public:

	Jump4(b2World* world, const b2Vec2& pos, const Theme* theme, int variant) : GameArea(world, pos, theme, variant),
		m_water(world)
	{
		Physics::Parameters params = Physics::DefaultGroundParameters;

		vector<b2Vec2> points;
		points.push_back(b2Vec2_zero);
		points.emplace_back(m_startZoneWidth, 0.0f);
		b2Vec2 curvestart = points.back();
		b2Vec2 curveend = b2Vec2(15.0f, -5.0f);
		Graphics::CreateCurve(curvestart, curveend, 10, points);

		b2Vec2 water[7];
		water[0].Set(17.0f, -20.0f);
		water[1].Set(18.0f, -22.0f);
		water[2].Set(20.0f, -24.0f);
		water[3].Set(22.0f, -24.5f);
		water[4].Set(24.0f, -24.0f);
		water[5].Set(26.0f, -22.0f);
		water[6].Set(27.0f, -20.0f);

		points.insert(points.end(), water, water + 7);
		m_water.Set(world, pos, water, 7, m_audio);
		
		curvestart = points.back();
		curveend.Set(35.0f, -15.0f);
		Graphics::CreateCurve(curvestart, curveend, 10, points);
		points.emplace_back(50.0f, -15.0f);
		m_end = points.back();
		UpdateDimensions();
		m_groundBody = Physics::CreateChain(world, params, points.data(), points.size(), pos);
	}

	virtual bool IsFullScreen() const { return true; }

	virtual void Draw(Renderer^ renderBatch)
	{
		m_water.Draw(renderBatch);
		GameArea::Draw(renderBatch);
	}

	virtual void Update(float dt)
	{
		m_water.Update(dt);
	}

	virtual void SaveState(Windows::Storage::Streams::DataWriter^ state) const
	{
		GameArea::SaveState(state);
		m_water.SaveState(state);
	}

	virtual void RestoreState(Windows::Storage::Streams::DataReader^ state)
	{
		GameArea::RestoreState(state);
		m_water.RestoreState(state);
	}

private:
	Water m_water;
};
