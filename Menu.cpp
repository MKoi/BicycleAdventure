#include "pch.h"
#include <string>
#include "Menu.h"
#include "SimpleMath.h"
#include "Graphics.h"

using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace DirectX;
using namespace DirectX::SimpleMath;

Menu::Menu(GameAudio* audio) :
m_transition(0.0f),
m_bigradius(2.5f),
m_smallradius(1.0f),
m_menuelements(),
m_textelements(),
m_play(m_menuelements, &Menu::DrawPlay),
m_pause(m_menuelements, &Menu::DrawPause),
m_restart(m_menuelements, &Menu::DrawRestart),
m_title(m_textelements, &Menu::DrawTitle),
m_score(m_textelements, &Menu::DrawScore),
m_bonus(m_textelements, &Menu::DrawBonus),
m_hiscore(m_textelements, &Menu::DrawHiScore),
m_gameover(m_textelements, &Menu::DrawGameOver),
m_newRecord(m_textelements, &Menu::DrawNewRecord),
m_help(m_textelements, &Menu::DrawHelp),
m_audio(audio)
{
	const float alpha = 0.5f;
	b2Vec2 v[3];
	float x = m_bigradius;
	v[0].Set(-0.4f * x, 0.5f * x);
	v[1].Set(-0.4f * x, -0.5f * x);
	v[2].Set(0.6f * x, 0.0f);
	Graphics::CreateSphere(b2Vec2_zero, m_bigradius, 0.8f, Graphics::orange, m_play.m_gfx, alpha);
	Graphics::CreateTriangle(v, Graphics::orange, m_play.m_gfx, alpha);

	x = 0.5f * m_smallradius;
	Graphics::CreateSphere(b2Vec2_zero, m_smallradius, 0.8f, Graphics::orange, m_pause.m_gfx, alpha);
	Graphics::CreateSolidLine(b2Vec2(0.6f * x, x), b2Vec2(0.6f * x, -x), 0.3f, Graphics::orange, m_pause.m_gfx, alpha);
	Graphics::CreateSolidLine(b2Vec2(-0.6f * x, x), b2Vec2(-0.6f * x, -x), 0.3f, Graphics::orange, m_pause.m_gfx, alpha);

	Graphics::CreateSphere(b2Vec2_zero, m_smallradius, 0.8f, Graphics::orange, m_restart.m_gfx, alpha);
	v[0].Set(-1.1f * x, 0.0f);
	v[1].Set(-0.1f * x, -x);
	v[2].Set(-0.1f * x, x);
	Graphics::CreateTriangle(v, Graphics::orange, m_restart.m_gfx, alpha);
	v[0].Set(-0.1f * x, 0.0f);
	v[1].Set(0.9f * x, -x);
	v[2].Set(0.9f * x, x);
	Graphics::CreateTriangle(v, Graphics::orange, m_restart.m_gfx, alpha);

	m_title.m_text = L"  Amazing  \n  Bicycle  \nAdventure";
	m_gameover.m_text = L"Game Over";
	m_newRecord.m_text = L"New Record";
	m_help.m_text = L"Tap and hold to brake";
}

void Menu::SetScreenDimensions(Renderer^ renderer)
{
	float width = renderer->GetScreenWidth();
	float height = renderer->GetScreenHeight();
	float bigRadiusScreen = m_bigradius * renderer->GetWorldToScreenRatio();
	float smallRadiusScreen = m_smallradius * renderer->GetWorldToScreenRatio();

	m_play.m_bounds.X = 0.5f * width - bigRadiusScreen;
	m_play.m_bounds.Y = 0.5f * height - bigRadiusScreen;
	m_play.m_bounds.Height = 2.0f * bigRadiusScreen;
	m_play.m_bounds.Width = 2.0f * bigRadiusScreen;
	m_play.m_gfx.CreateAnimation();
	m_play.m_gfx.GetAnimation()->SetMove(renderer->GetViewTopRight(), Vector2(0.0f, 0.0f));
	m_play.m_gfx.GetAnimation()->SetScale(m_smallradius / m_bigradius, 1.0f, m_smallradius / m_bigradius, 1.0f);

	m_pause.m_bounds.X = width - 2.0f * smallRadiusScreen;
	m_pause.m_bounds.Y = 0.0f;
	m_pause.m_bounds.Height = 2.0f * smallRadiusScreen;
	m_pause.m_bounds.Width = 2.0f * smallRadiusScreen;
	m_pause.m_gfx.CreateAnimation();
	m_pause.m_gfx.GetAnimation()->SetMove(-renderer->GetViewTopRight(), Vector2(0.0f, 0.0f));
	m_pause.m_gfx.GetAnimation()->SetScale(m_bigradius / m_smallradius, 1.0f, m_bigradius / m_smallradius, 1.0f);

	m_restart.m_bounds.X = 0.0f;
	m_restart.m_bounds.Y = 0.0f;
	m_restart.m_bounds.Height = 2.0f * smallRadiusScreen;
	m_restart.m_bounds.Width = 2.0f * smallRadiusScreen;
}

bool Menu::OnPressed(Point point)
{
	for (auto i : m_menuelements)
	{
		if (i->m_enabled && i->m_bounds.Contains(point))
		{
			i->m_pressed = true;
			return true;
		}
	}
	return false;
}

void Menu::Draw(Renderer^ renderBatch)
{
	for (auto i : m_menuelements)
	{
		if (i->m_enabled)
		{
			(this->*(i->m_draw))(renderBatch);
		}
	}
	for (auto i : m_textelements)
	{
		if (i->m_enabled)
		{
			(this->*(i->m_draw))(renderBatch);
		}
	}
}

void Menu::EnablePlay(bool enable)
{ 
	m_play.m_enabled = enable;
	if (m_play.m_gfx.GetAnimation())
	{
		m_play.m_gfx.GetAnimation()->SetScale(m_smallradius / m_bigradius, 1.0f, m_smallradius / m_bigradius, 1.0f);
		m_play.m_gfx.GetAnimation()->Stop();
		m_play.m_gfx.GetAnimation()->Play(0.3f, false);
	}
}

void Menu::EnablePause(bool enable)
{ 
	if (m_pause.m_gfx.GetAnimation())
	{
		m_pause.m_gfx.GetAnimation()->Stop();
		m_pause.m_gfx.GetAnimation()->Play(0.3f, false);
	}
	m_pause.m_enabled = enable; 
}

void Menu::EnableRestart(bool enable)
{ 
	m_restart.m_enabled = enable; 
}

void Menu::EnableGameOver(bool enable)
{
	if (enable && !m_gameover.m_enabled)
	{
		const XMFLOAT2 start(1.0f, 1.0f);
		const XMFLOAT2 end(2.0f, 2.0f);
		Animation::Reset(m_gameover.m_scale, start, end, false, 1.0f);
	}
	m_gameover.m_enabled = enable; 
}

void Menu::EnableTitle(bool enable)
{
	if (enable && !m_title.m_enabled)
	{
		const XMFLOAT2 start(1.5f, 1.5f);
		const XMFLOAT2 end(1.7f, 1.6f);
		Animation::Reset(m_title.m_scale, start, end, true, 2.0f);
	}
	m_title.m_enabled = enable;

}

void Menu::EnableBonus(bool enable)
{
	if (enable && !m_bonus.m_enabled)
	{
		const XMFLOAT2 start(0.1f, 0.1f);
		const XMFLOAT2 end(1.0f, 1.0f);
		Animation::Reset(m_bonus.m_scale, start, end, false, 0.5f);
	}
	m_bonus.m_enabled = enable;
}

void Menu::EnableHiscore(bool enable)
{
	if (enable && !m_hiscore.m_enabled)
	{
		const XMFLOAT2 scalestart(1.0f, 1.0f);
		const XMFLOAT2 scaleend(1.3f, 1.3f);
		Animation::Reset(m_hiscore.m_scale, scalestart, scaleend, false, 1.0f);

		const XMFLOAT2 posstart(0.0f, 0.0f);
		const XMFLOAT2 posend(1.0f, 1.0f);
		Animation::Reset(m_hiscore.m_pos, posstart, posend, false, 1.0f);
	}
	m_hiscore.m_enabled = enable;
}

void Menu::EnableNewRecord(bool enable)
{
	if (enable && !m_newRecord.m_enabled)
	{
		const XMFLOAT2 scalestart(1.0f, 1.0f);
		const XMFLOAT2 scaleend(1.3f, 1.3f);
		Animation::Reset(m_newRecord.m_scale, scalestart, scaleend, true, 1.5f);
	}
	m_newRecord.m_enabled = enable;
}

void Menu::EnableHelp(bool enable)
{
	if (enable && !m_help.m_enabled)
	{
		const XMFLOAT2 scalestart(0.8f, 0.8f);
		const XMFLOAT2 scaleend(0.9f, 0.9f);
		Animation::Reset(m_help.m_scale, scalestart, scaleend, true, 1.5f);
		m_help.m_delay = 4.5f;
	}
	m_help.m_enabled = enable;
}

void Menu::SetScore(int score)
{
	m_score.m_text = std::to_wstring(score);
}

void Menu::BumpScore()
{
	const XMFLOAT2 start(1.0f, 1.0f);
	const XMFLOAT2 end(1.5f, 1.3f);
	Animation::Set(m_score.m_scale, start, end, false, 0.3f);
	m_score.m_scale.Play = true;
}

void Menu::SetBonus(int bonus)
{
	m_bonus.m_text = std::wstring(L"+") + std::to_wstring(bonus);
}

void Menu::SetHiScore(int hiscore)
{
	if (hiscore > 0)
	{
		m_hiscore.m_text = std::to_wstring(hiscore);
	}
}

void Menu::InputHandled()
{
	for (auto i : m_menuelements)
	{
		i->m_pressed = false;
	}
}

void Menu::Update(float dt)
{
	for (auto i : m_menuelements)
	{
		if (i->m_enabled && i->m_gfx.GetAnimation())
		{
			i->m_gfx.GetAnimation()->Update(dt);
		}
	}
	for (auto i : m_textelements)
	{
		i->Update(dt);
	}
}


Menu::MenuElement::MenuElement(std::vector<MenuElement*>& menus, void (Menu::*drawfunc)(Renderer^ renderer)) : 
m_pressed(false), m_enabled(false), m_gfx(), m_bounds(), m_draw(drawfunc) 
{
	menus.push_back(this);
}

Menu::TextElement::TextElement(std::vector<TextElement*> &texts, void (Menu::*drawfunc)(Renderer^ renderer)) :
m_enabled(false), m_text(L""), m_draw(drawfunc),
m_scale(DirectX::XMFLOAT2(1.0f, 1.0f)), m_pos(DirectX::XMFLOAT2(0.0f, 0.0f)), m_delay(0.0f)
{
	texts.push_back(this);
}

void Menu::TextElement::Update(float dt)
{
	if (m_enabled)
	{
		m_pos.Update(dt);
		m_scale.Update(dt);
		if (m_delay > 0.0f)
		{
			m_delay -= dt;
			if (m_delay <= 0.0)
			{
				m_delay = 0.0f;
				m_enabled = false;
			}
		}
	}
}

void Menu::DrawPlay(Renderer^ renderBatch)
{
	
	if (m_play.m_gfx.GetAnimation())
	{
		Vector2 pos = m_restart.m_enabled ? renderBatch->GetViewTopRight() : renderBatch->GetViewTopLeft();
		m_play.m_gfx.GetAnimation()->Position.SetStart(pos - renderBatch->GetViewCenter());
		if (m_play.m_gfx.GetAnimation()->Position.AtEnd())
		{
			m_play.m_gfx.GetAnimation()->SetScale(0.9f, 1.0f, 1.0f, 0.9f);
			m_play.m_gfx.GetAnimation()->Xscale.SetContinuous(true);
			m_play.m_gfx.GetAnimation()->Xscale.SetDuration(1.0f);
			m_play.m_gfx.GetAnimation()->Yscale.SetContinuous(true);
			m_play.m_gfx.GetAnimation()->Yscale.SetDuration(1.0f);
		}
	}
	m_play.m_gfx.Update(0.0f, 0.0f, renderBatch->GetViewCenter(), renderBatch->GetScale());
	renderBatch->AddToBatch(&m_play.m_gfx, Renderer::Foremost);
}

void Menu::DrawPause(Renderer^ renderBatch)
{
	float scale = renderBatch->GetScale();
	Vector2 pos = renderBatch->GetViewTopRight(scale * m_smallradius);
	if (m_pause.m_gfx.GetAnimation())
	{
		m_pause.m_gfx.GetAnimation()->Position.SetStart(-renderBatch->GetViewTopRight() + renderBatch->GetViewCenter());
	}
	m_pause.m_gfx.Update(0.0f, 0.0f, pos, scale);
	renderBatch->AddToBatch(&m_pause.m_gfx, Renderer::Foremost);
}

void Menu::DrawRestart(Renderer^ renderBatch)
{
	float scale = renderBatch->GetScale();
	Vector2 pos = renderBatch->GetViewTopLeft(scale * m_smallradius);

	m_restart.m_gfx.Update(0.0, 0.0f, pos, scale);
	renderBatch->AddToBatch(&m_restart.m_gfx, Renderer::Foremost);
}

void Menu::DrawTitle(Renderer^ renderBatch)
{
	XMFLOAT2 pos(0.5f * renderBatch->GetScreenWidth(), 0.1f * renderBatch->GetScreenHeight());
	renderBatch->AddToBatch(m_title.m_text.c_str(), pos, m_title.m_scale.Get());

	if (!m_hiscore.m_text.empty() && !m_newRecord.m_enabled)
	{
		XMFLOAT2 poshiscore(0.5f * renderBatch->GetScreenWidth(), 0.7f * renderBatch->GetScreenHeight());
		renderBatch->AddToBatch(L"BEST SCORE:", poshiscore, XMFLOAT2(1.0f, 1.0f));
		if (!m_hiscore.m_enabled)
		{
			XMFLOAT2 poshiscore2(0.5f * renderBatch->GetScreenWidth(), 0.75f * renderBatch->GetScreenHeight());
			renderBatch->AddToBatch(m_hiscore.m_text.c_str(), poshiscore2, XMFLOAT2(1.5f, 1.5f));
		}
	}
	if (m_newRecord.m_enabled)
	{
		m_newRecord.m_scale.Stop();
	}

	XMFLOAT2 posauthor(0.5f * renderBatch->GetScreenWidth(), 0.9f * renderBatch->GetScreenHeight());
	renderBatch->AddToBatch(L"Created by Mikko Koivisto", posauthor, XMFLOAT2(0.5f, 0.5f));

	XMFLOAT2 posversion(0.5f * renderBatch->GetScreenWidth(), 0.96f * renderBatch->GetScreenHeight());
	renderBatch->AddToBatch(L"V1.1", posversion, XMFLOAT2(0.4f, 0.4f));
}

void Menu::DrawGameOver(Renderer^ renderBatch)
{
	XMFLOAT2 pos(0.5f * renderBatch->GetScreenWidth(), 0.3f * renderBatch->GetScreenHeight());
	renderBatch->AddToBatch(m_gameover.m_text.c_str(), pos, m_gameover.m_scale.Get());

}

void Menu::DrawHiScore(Renderer^ renderBatch)
{
	Vector2 startpos(0.5f * renderBatch->GetScreenWidth(), 0.0f * renderBatch->GetScreenHeight());
	Vector2 endpos(0.5f * renderBatch->GetScreenWidth(), 0.75f * renderBatch->GetScreenHeight());
	m_hiscore.m_pos.SetStart(startpos);
	m_hiscore.m_pos.SetEnd(endpos);

	if (m_hiscore.m_text.c_str())
	{
		renderBatch->AddToBatch(m_hiscore.m_text.c_str(), m_hiscore.m_pos.Get(), m_hiscore.m_scale.Get());
	}
}

void Menu::DrawScore(Renderer^ renderBatch)
{
	XMFLOAT2 pos(0.5f * renderBatch->GetScreenWidth(), 0.0f * renderBatch->GetScreenHeight());
	XMFLOAT2 scale = m_score.m_scale.Get();
	renderBatch->AddToBatch(m_score.m_text.c_str(), pos, scale);
	if (m_score.m_scale.AtEnd())
	{
		m_score.m_scale.Stop();
	}
}

void Menu::DrawBonus(Renderer^ renderBatch)
{
	XMFLOAT2 pos(0.5f * renderBatch->GetScreenWidth(), 0.05f * renderBatch->GetScreenHeight());
	renderBatch->AddToBatch(m_bonus.m_text.c_str(), pos, XMFLOAT2(1.0f, 1.0f));
}

void Menu::DrawNewRecord(Renderer^ renderBatch)
{
	XMFLOAT2 pos(0.5f * renderBatch->GetScreenWidth(), 0.7f * renderBatch->GetScreenHeight());
	renderBatch->AddToBatch(m_newRecord.m_text.c_str(), pos, m_newRecord.m_scale.Get());
}

void Menu::DrawHelp(Renderer^ renderBatch)
{
	XMFLOAT2 pos(0.5f * renderBatch->GetScreenWidth(), 0.7f * renderBatch->GetScreenHeight());
	renderBatch->AddToBatch(m_help.m_text.c_str(), pos, m_help.m_scale.Get());
}

