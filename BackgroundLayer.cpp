#include "pch.h"
#include "Graphics.h"
#include "BackgroundLayer.h"

BackgroundLayer::BackgroundLayer(float dx, const float* dy, size_t ycount, const b2Color& color, float parallax) :
m_styles(), m_dy(dy, ycount), m_dx(dx), m_parallax(parallax), m_color(color), m_graphics() 
{
}

void BackgroundLayer::Draw(Renderer^ renderer, float baseY)
{
	if (m_dx == 0.0f)
	{
		return;
	}
	float start_x = ParallaxTransform(renderer->GetViewLeft());
	float screen_width = renderer->GetViewRight() - renderer->GetViewLeft() + m_dx;
	screen_width = screen_width - fmod(screen_width, m_dx) + m_dx;
	float end_x = start_x + screen_width;
	size_t begin_index = (size_t)(start_x / m_dx);
	m_dy.SetBaseIndex(begin_index);

	auto start = m_styles.lower_bound(start_x);
	auto end = m_styles.upper_bound(end_x);

	if (start != m_styles.begin())
	{
		m_styles.erase(m_styles.begin(), start);
	}

	m_dy.SetOffset(renderer->GetViewBox()->Center.y - baseY);
	m_graphics.Clear();
	float current_x = start_x;
	while (start != end && start != m_styles.end())
	{		
		current_x = DrawConvexTile(renderer, start->second, current_x, min(start->first, end_x));
		start++;
	}
	if (start != m_styles.end())
	{
		DrawConvexTile(renderer, start->second, current_x, end_x);
	}

	float x_offset = (1 - m_parallax) * renderer->GetViewLeft();
	m_graphics.Update(0.0f, 0.0f, b2Vec2(x_offset, 0.0f));
	renderer->AddToBatch(&m_graphics, Renderer::Background);
}

BackgroundLayer::Style BackgroundLayer::GetStyle(float point) const
{
	auto it = m_styles.lower_bound(point);
	if (it != m_styles.end())
	{
		return it->second;
	}
	return e_undefined;
}

void BackgroundLayer::SetStyle(Style style, float point)
{
	m_styles[ParallaxTransform(point)] = style;
}

void BackgroundLayer::Clear()
{
	m_styles.clear();
}

float BackgroundLayer::DrawConvexTile(Renderer^ renderer, Style style, float start_x, float end_x)
{
	if (start_x + m_dx > end_x)
	{
		return start_x;
	}

	vector<b2Vec2> points;
	points.emplace_back(start_x, m_dy.At(0));
	start_x += m_dx;
	m_dy.UpdateBaseIndex();
	if (style == e_curve)
	{
		CreateCurvePoints(start_x, end_x, points);
	}
	else if (style == e_discrete)
	{
		CreateDiscretePoints(start_x, end_x, points);
	}
	else if (style == e_linear)
	{
		CreateLinearPoints(start_x, end_x, points);
	}
	float bottomY = renderer->GetViewBox()->Center.y - renderer->GetViewBox()->Extents.y;
	Graphics::CreateGround(points, bottomY, m_color, m_graphics);

	return points.back().x;
}


void BackgroundLayer::CreateLinearPoints(float start_x, float end_x, vector<b2Vec2>& points)
{
	size_t i;
	for (i = 0; start_x < end_x; ++i)
	{
		points.emplace_back(start_x, m_dy.At(i));
		start_x += m_dx;
	}
	points.emplace_back(end_x, m_dy.At(i));
	m_dy.UpdateBaseIndex(i);
}

void BackgroundLayer::CreateDiscretePoints(float start_x, float end_x, vector<b2Vec2>& points)
{
	size_t i;
	float prev_y;
	for (i = 0; start_x < end_x; ++i)
	{
		prev_y = points.back().y;
		points.emplace_back(start_x, prev_y);
		points.emplace_back(start_x, m_dy.At(i));
		start_x += m_dx;
	}
	prev_y = points.back().y;
	points.emplace_back(end_x, prev_y);

	m_dy.UpdateBaseIndex(i);
}

void BackgroundLayer::CreateCurvePoints(float start_x, float end_x, vector<b2Vec2>& points)
{
	const size_t interpolations = 5;
	size_t i;
	b2Vec2 newPoint;
	for (i = 0; start_x < end_x; ++i)
	{
		newPoint.Set(start_x, m_dy.At(i));
		Graphics::CreateCurve(points.back(), newPoint, interpolations, points);
		start_x += m_dx;
	}
	newPoint.Set(end_x, m_dy.At(i));
	Graphics::CreateCurve(points.back(), newPoint, interpolations, points);
	m_dy.UpdateBaseIndex(i);
}

float BackgroundLayer::ParallaxTransform(float x)
{
	float background_x = m_parallax * x;
	return background_x - fmod(background_x, m_dx);
}


float BackgroundLayer::RingBuffer::At(size_t index) const
{
	return m_offset + m_data.at((m_basei + index) % m_data.size());
}
