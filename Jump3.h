#pragma once
#include <vector>
#include "Box2D\Box2D.h"
#include "GameArea.h"

class Jump3 : public GameArea
{
public:

	Jump3(b2World* world, const b2Vec2& pos, const Theme* theme, int variant) : GameArea(world, pos, theme, variant)
	{
		Physics::Parameters params = Physics::DefaultGroundParameters;
		float jumpwidth = 23.0f;
		float jumpheight = jumpwidth * 0.3f;
		b2Vec2 jumpstart(10.0f, 0.0f);
		b2Vec2 jumpend(jumpstart.x + jumpwidth, jumpstart.y);
		b2Vec2 highpoint(jumpstart.x + 0.5f * jumpwidth, jumpstart.y + jumpheight);
		vector<b2Vec2> points;
		points.push_back(b2Vec2_zero);
		points.push_back(jumpstart);
		Graphics::CreateCurve(jumpstart, highpoint, 10, points);
		Graphics::CreateCurve(highpoint, jumpend, 10, points);
		jumpstart.Set(points.back().x, points.back().y);
		jumpend.Set(jumpstart.x + jumpwidth, jumpstart.y);
		highpoint.Set(jumpstart.x + 0.5f * jumpwidth, jumpstart.y + 0.7f * jumpheight);
		Graphics::CreateCurve(jumpstart, highpoint, 10, points);
		Graphics::CreateCurve(highpoint, jumpend, 10, points);
		points.emplace_back(jumpend.x + 3.0f, jumpend.y);
		m_end = points.back();
		UpdateDimensions();
		m_groundBody = Physics::CreateChain(world, params, points.data(), points.size(), pos);
	}

	virtual bool IsFullScreen() const { return true; }
private:

};
