#pragma once
#include <vector>
#include <list>
#include "Box2D\Box2D.h"
#include "BackgroundLayer.h"
#include "VerticeBatch.h"
#include "Renderer.h"
#include "Theme.h"

class Background
{
public:
	Background();
	void Draw(Renderer^ renderer);
	void Clear();
	void Reset(vector<const Theme*>& themes, vector<float>& points);
	void SetTheme(const Theme* theme, float pointX);

private:
	BackgroundLayer::Style ThemeToStyle(const Theme* theme);

	float m_parallaxfront;
	float m_parallaxback;
	float m_dx;

	b2Color m_backcolor;

	BackgroundLayer m_front;
	BackgroundLayer m_back;

	
};