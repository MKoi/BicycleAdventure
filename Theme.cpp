#include "pch.h"
#include "Theme.h"

Theme::Theme(Type type, const b2Color& color) :
Style(type),
Color(color),
m_decorations()
{
	switch (type)
	{
	case e_grass:
		InitGrassTheme();
		break;
	case e_desert:
		InitDesertTheme();
		break;
	case e_rock:
		InitRockTheme();
		break;
	case e_forest:
		InitForestTheme();
		break;
	case e_forest2:
		InitForest2Theme();
		break;
	case e_urban:
		InitUrbanTheme();
		break;
	}
}

const VerticeBatch& Theme::GetDecoration(size_t hash) const
{
	size_t i = hash % m_decorations.size();
	return m_decorations[i];
}

void Theme::Update(float dt)
{
	for (VerticeBatch& v : m_decorations)
	{
		v.Update(dt, 0.0f, b2Vec2_zero);
	}
}


void Theme::InitGrassTheme()
{
	// grass
	m_decorations.push_back(VerticeBatch());
	b2Vec2 v[3] = { b2Vec2(0.0f, -0.2f), b2Vec2(0.2f, -0.2f), b2Vec2(-0.2f, 0.5f) };
	b2Vec2 v2[3] = { b2Vec2(0.0f, -0.2f), b2Vec2(0.2f, -0.2f), b2Vec2(0.6f, 0.7f) };
	Graphics::CreateTriangle(v, Color, m_decorations.back());
	Graphics::CreateTriangle(v2, Color, m_decorations.back());
	m_decorations.back().SetBoundingBox();
	// grass
	m_decorations.push_back(VerticeBatch());
	b2Vec2 v3[3] = { b2Vec2(0.0f, -0.2f), b2Vec2(0.3f, -0.2f), b2Vec2(0.0f, 0.6f) };
	b2Vec2 v4[3] = { b2Vec2(0.2f, -0.2f), b2Vec2(0.5f, -0.2f), b2Vec2(0.6f, 0.8f) };
	b2Vec2 v5[3] = { b2Vec2(0.4f, -0.2f), b2Vec2(0.7f, -0.2f), b2Vec2(0.8f, 0.5f) };
	Graphics::CreateTriangle(v3, Color, m_decorations.back());
	Graphics::CreateTriangle(v4, Color, m_decorations.back());
	Graphics::CreateTriangle(v5, Color, m_decorations.back());
	m_decorations.back().SetBoundingBox();
}

void Theme::InitDesertTheme()
{
	// cactus
	m_decorations.push_back(VerticeBatch());
	Graphics::CreateSolidLineRoundEnd(b2Vec2_zero, b2Vec2(0.0f, 3.0f), 0.15f, Graphics::green, m_decorations.back(), Graphics::eRoundEnd);
	Graphics::CreateSolidLineRoundEnd(b2Vec2(0.0f, 2.0f), b2Vec2(-0.7f, 3.0f), 0.2f, Graphics::green, m_decorations.back(), Graphics::eRoundEnd);
	Graphics::CreateSolidLineRoundEnd(b2Vec2(0.0f, 1.5f), b2Vec2(0.5f, 2.5f), 0.2f, Graphics::green, m_decorations.back(), Graphics::eRoundEnd);
	m_decorations.back().CreateAnimation();
	m_decorations.back().GetAnimation()->SetScale(1.0f / 0.8f, 0.8f);
	m_decorations.back().GetAnimation()->Play(1.0f, true);
	m_decorations.back().SetBoundingBox();

	m_decorations.push_back(VerticeBatch());
	Graphics::CreateSolidEllipse(b2Vec2(0.0f, 1.0f), 0.6f, 1.3f, Graphics::green, m_decorations.back());
	Graphics::CreateSolidLineRoundEnd(b2Vec2(0.0f, 1.0f), b2Vec2(-0.7f, 2.0f), 0.2f, Graphics::green, m_decorations.back(), Graphics::eRoundEnd);
	m_decorations.back().CreateAnimation();
	m_decorations.back().GetAnimation()->SetScale(1.0f / 0.8f, 0.8f);
	m_decorations.back().GetAnimation()->Play(1.0f, true);
	m_decorations.back().SetBoundingBox();

}

void Theme::InitRockTheme()
{
	// rock
	m_decorations.push_back(VerticeBatch());
	b2Vec2 v[5] = { b2Vec2(1.7f, -0.5f), b2Vec2(2.0f, 1.5f), b2Vec2(1.0f, 2.0f), b2Vec2(-0.5f, 0.5f), b2Vec2(0.0f, -0.5f) };
	Graphics::CreateSolidConvex(v, 5, Color.Lighter(0.2f), m_decorations.back());
	m_decorations.back().SetBoundingBox();
	// another rock
	m_decorations.push_back(VerticeBatch());
	b2Vec2 v2[5] = { b2Vec2(1.5f, -0.5f), b2Vec2(3.0f, 2.0f), b2Vec2(1.0f, 2.5f), b2Vec2(-0.5f, 0.5f), b2Vec2(0.0f, -0.5f) };
	Graphics::CreateSolidConvex(v2, 5, Color.Lighter(0.1f), m_decorations.back());
	m_decorations.back().SetBoundingBox();
	// yet another rock
	m_decorations.push_back(VerticeBatch());
	b2Vec2 v3[4] = { b2Vec2(2.0f, -0.5f), b2Vec2(2.5f, 4.0f), b2Vec2(1.5f, 3.5f), b2Vec2(0.0f, -0.5f) };
	b2Vec2 v4[4] = { b2Vec2(2.5f, -0.5f), b2Vec2(2.0f, 3.0f), b2Vec2(1.0f, 3.2f), b2Vec2(0.0f, -0.5f) };
	Graphics::CreateSolidQuadilateral(v3, Color.Lighter(0.1f), m_decorations.back());
	Graphics::CreateSolidQuadilateral(v4, Color.Lighter(0.2f), m_decorations.back());
	m_decorations.back().SetBoundingBox();
}

void Theme::InitForestTheme()
{

	// tree
	m_decorations.push_back(VerticeBatch());
	Graphics::CreateSolidLine(b2Vec2_zero, b2Vec2(0.0f, 2.0f), 0.2f, Graphics::grey, m_decorations.back());
	Graphics::CreateSolidCircle(b2Vec2(0.0f, 2.0f), 1.0f, Color, m_decorations.back());
	Graphics::CreateSolidCircle(b2Vec2(0.0f, 3.0f), 1.5f, Color, m_decorations.back());
	m_decorations.back().CreateAnimation();
	m_decorations.back().GetAnimation()->SetBend(-0.3f, 0.3f);
	m_decorations.back().GetAnimation()->Play(2.0f, true);
	m_decorations.back().SetBoundingBox();

	// tree2
	m_decorations.push_back(VerticeBatch());
	Graphics::CreateSolidLine(b2Vec2_zero, b2Vec2(0.0f, 2.0f), 0.2f, Graphics::grey, m_decorations.back());
	Graphics::CreateSolidEllipse(b2Vec2(0.0f, 3.0f), 1.5f, 2.0f, Color.Darker(), m_decorations.back());
	m_decorations.back().CreateAnimation();
	m_decorations.back().GetAnimation()->SetBend(-0.5f, 0.5f);
	m_decorations.back().GetAnimation()->Play(2.0f, true);
	m_decorations.back().SetBoundingBox();

	// bush
	m_decorations.push_back(VerticeBatch());
	Graphics::CreateSolidCircle(b2Vec2(0.0f, 0.5f), 0.5f, Color.Darker(), m_decorations.back());
	Graphics::CreateSolidCircle(b2Vec2(0.7f, 0.4f), 1.0f, Color.Darker(), m_decorations.back());
	Graphics::CreateSolidCircle(b2Vec2(1.3f, 0.3f), 0.8f, Color.Darker(), m_decorations.back());
	m_decorations.back().CreateAnimation();
	m_decorations.back().GetAnimation()->SetScale(1.0f / 0.8f, 0.8f);
	m_decorations.back().GetAnimation()->Play(1.0f, true);
	m_decorations.back().SetBoundingBox();
}

void Theme::InitForest2Theme()
{
	// tree
	m_decorations.push_back(VerticeBatch());
	Graphics::CreateSolidLine(b2Vec2_zero, b2Vec2(0.0f, 2.0f), 0.2f, Graphics::brown, m_decorations.back());
	b2Vec2 v[3] = { b2Vec2(0.0f, 6.0f), b2Vec2(-1.0f, 4.0f), b2Vec2(1.0f, 4.0f) };
	b2Vec2 v2[3] = { b2Vec2(0.0f, 4.5f), b2Vec2(-1.5f, 2.7f), b2Vec2(1.5f, 2.7f) };
	b2Vec2 v3[3] = { b2Vec2(0.0f, 3.2f), b2Vec2(-2.0f, 1.5f), b2Vec2(2.0f, 1.5f) };
	Graphics::CreateTriangle(v, Color, m_decorations.back());
	Graphics::CreateTriangle(v2, Color, m_decorations.back());
	Graphics::CreateTriangle(v3, Color, m_decorations.back());
	m_decorations.back().CreateAnimation();
	m_decorations.back().GetAnimation()->SetBend(-0.3f, 0.3f);
	m_decorations.back().GetAnimation()->Play(2.0f, true);
	m_decorations.back().SetBoundingBox();
	
	// rock
	m_decorations.push_back(VerticeBatch());
	b2Vec2 v4[5] = { b2Vec2(1.7f, -0.5f), b2Vec2(2.0f, 1.5f), b2Vec2(1.0f, 2.0f), b2Vec2(-0.5f, 0.5f), b2Vec2(0.0f, -0.5f) };
	Graphics::CreateSolidConvex(v4, 5, Graphics::darkGrey, m_decorations.back());
	m_decorations.back().SetBoundingBox();

}

void Theme::InitUrbanTheme()
{
}
