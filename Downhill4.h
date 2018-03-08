#pragma once
#include <vector>
#include "Box2D\Box2D.h"
#include "GameArea.h"

using std::vector;

class Downhill4 : public GameArea
{
public:

	Downhill4(b2World* world, const b2Vec2& pos, const Theme* theme, int variant) : GameArea(world, pos, theme, variant)
	{
		Physics::Parameters params = Physics::DefaultGroundParameters;

		vector<b2Vec2> points;
		points.push_back(b2Vec2_zero);
		points.emplace_back(m_startZoneWidth, 0.0f);
		b2Vec2 curvestart = points.back();
		b2Vec2 curveend(20.0f, -30.0f);
		Graphics::CreateCurve(curvestart, curveend, 10, points);
		curvestart = points.back();
		curveend.Set(50.0f, -40.0f);
		Graphics::CreateCurve(curvestart, curveend, 10, points);
		curvestart = points.back();
		curveend.Set(53.0f, -45.0f);
		Graphics::CreateCurve(curvestart, curveend, 5, points);
		curvestart = points.back();
		curveend.Set(67.0f, -50.0f);
		Graphics::CreateCurve(curvestart, curveend, 10, points);
		curvestart = points.back();
		curveend.Set(70.0f, -57.0f);
		Graphics::CreateCurve(curvestart, curveend, 5, points);
		curvestart = points.back();
		curveend.Set(90.0f, -60.0f);
		Graphics::CreateCurve(curvestart, curveend, 10, points);

		m_end = points.back();
		UpdateDimensions();
		m_groundBody = Physics::CreateChain(world, params, points.data(), points.size(), pos);
	}

	virtual bool IsFullScreen() const { return false; }

private:
};
