#include "pch.h"
#include "Background.h"

using std::vector;

const float points[] = { -0.3f, 2.5f, -1.0f, 0.0f, 0.5f, -1.3f, 1.1f, 0.0f, -0.8f, 1.0f, 2.0f, 2.5f };
const float points2[] = { -0.3f, 0.6f, 1.2f, -0.4f, 0.0f, -1.3f, -1.5f, 1.1f, 0.0f, 0.5f };

Background::Background() : 
m_parallaxfront(0.5f),
m_parallaxback(0.2f),
m_dx(4.0f),
m_backcolor(0.098f, 0.098f, 0.439f),
m_front(m_dx, points, sizeof(points) / sizeof(points[0]), m_backcolor.Lighter(), m_parallaxfront),
m_back(m_dx, points2, sizeof(points2) / sizeof(points2[0]), m_backcolor.Lighter().Lighter(0.15f), m_parallaxback)
{

}

void Background::Draw(Renderer^ renderer)
{
	const float y_limit = 1.0f;
	const float dy = max(min(0.3f * renderer->GetViewCenter().y, y_limit), -y_limit);

	m_back.Draw(renderer);
	m_front.Draw(renderer, dy);

}

void Background::Clear()
{
	m_front.Clear();
	m_back.Clear();
}

void Background::Reset(vector<const Theme*>& themes, vector<float>& points)
{
	size_t count = max(themes.size(), points.size());
	for (size_t i = 0; i < count; ++i)
	{
		SetTheme(themes[i], points[i]);
	}
}

void Background::SetTheme(const Theme* theme, float pointX)
{
	BackgroundLayer::Style style = ThemeToStyle(theme);
	m_front.SetStyle(style, pointX);
	m_back.SetStyle(style, pointX);
}


BackgroundLayer::Style Background::ThemeToStyle(const Theme* theme)
{
	BackgroundLayer::Style ret = BackgroundLayer::e_undefined;
	if (theme->Style == Theme::e_rock)
	{
		ret = BackgroundLayer::e_linear;
	}
	else if (theme->Style == Theme::e_urban)
	{
		ret = BackgroundLayer::e_discrete;
	}
	else
	{
		ret = BackgroundLayer::e_curve;
	}
	return ret;
}






