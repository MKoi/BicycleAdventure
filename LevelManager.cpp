#include "pch.h"
#include "LevelManager.h"
#include "Hills.h"
#include "Hills2.h"
#include "Jump.h"
#include "Tunnel.h"
#include "BreakableWall.h"
#include "Bridge.h"
#include "Uphill.h"
#include "Downhill.h"
#include "Bridge2.h"
#include "Ramps.h"
#include "Elevator.h"
#include "Tunnel2.h"
#include "Elevator2.h"
#include "Jump2.h"
#include "BreakableWall2.h"
#include "Bridge3.h"
#include "Downhill2.h"
#include "Elevator3.h"
#include "Hills3.h"
#include "Jump3.h"
#include "Ramps2.h"
#include "Tunnel3.h"
#include "Uphill2.h"
#include "BreakableWall3.h"
#include "Downhill3.h"
#include "Bridge4.h"
#include "Hills4.h"
#include "Jump4.h"
#include "Tunnel4.h"
#include "Elevator4.h"
#include "Downhill4.h"

LevelManager::LevelManager(b2World* world) :
m_world(world),
m_themes(),
m_levels()
{
	m_themes.emplace_back(Theme::Type::e_grass, Graphics::green.Lighter());
	m_themes.emplace_back(Theme::Type::e_desert, Graphics::yellow.Lighter());
	m_themes.emplace_back(Theme::Type::e_rock, Graphics::darkGrey);
	m_themes.emplace_back(Theme::Type::e_forest, Graphics::green);
	m_themes.emplace_back(Theme::Type::e_forest2, Graphics::green.Darker());
	m_themes.emplace_back(Theme::Type::e_urban, Graphics::blue.Lighter());

	m_levels.push_back(&GameArea::Create<Hills>); // 0
	m_levels.push_back(&GameArea::Create<Jump>);
	m_levels.push_back(&GameArea::Create<Tunnel>);
	m_levels.push_back(&GameArea::Create<BreakableWall>);
	m_levels.push_back(&GameArea::Create<Hills2>);
	m_levels.push_back(&GameArea::Create<Bridge>);
	m_levels.push_back(&GameArea::Create<Uphill>);
	m_levels.push_back(&GameArea::Create<Downhill>);
	m_levels.push_back(&GameArea::Create<Bridge2>);
	m_levels.push_back(&GameArea::Create<Ramps>);
	m_levels.push_back(&GameArea::Create<Elevator>); // 10
	m_levels.push_back(&GameArea::Create<Tunnel2>);
	m_levels.push_back(&GameArea::Create<Elevator2>);
	m_levels.push_back(&GameArea::Create<Jump2>);
	m_levels.push_back(&GameArea::Create<BreakableWall2>);
	m_levels.push_back(&GameArea::Create<Bridge3>);
	m_levels.push_back(&GameArea::Create<Downhill2>);
	m_levels.push_back(&GameArea::Create<Elevator3>);
	m_levels.push_back(&GameArea::Create<Hills3>);
	m_levels.push_back(&GameArea::Create<Jump3>);
	m_levels.push_back(&GameArea::Create<Ramps2>); // 20
	m_levels.push_back(&GameArea::Create<Tunnel3>);
	m_levels.push_back(&GameArea::Create<Uphill2>);
	m_levels.push_back(&GameArea::Create<BreakableWall3>);
	m_levels.push_back(&GameArea::Create<Downhill3>);
	m_levels.push_back(&GameArea::Create<Bridge4>);
	m_levels.push_back(&GameArea::Create<Hills4>);
	m_levels.push_back(&GameArea::Create<Jump4>);
	m_levels.push_back(&GameArea::Create<Tunnel4>);
	m_levels.push_back(&GameArea::Create<Elevator4>);
	m_levels.push_back(&GameArea::Create<Downhill4>); // 30
}

void LevelManager::Reset()
{
	const Theme* theme = &m_themes.at(std::rand() % m_themes.size());
	m_previous = LevelData(this, b2Vec2_zero, theme);
	m_current = LevelData(this, m_previous.m_level->EndPoint(), theme);
	m_next = LevelData(this, m_current.m_level->EndPoint(), theme);
	m_nextnext = LevelData(this, m_next.m_level->EndPoint(), theme);
}

void LevelManager::Update(float dt)
{
	for (Theme& t : m_themes)
	{
		t.Update(dt);
	}

	m_previous.m_level->Update(dt);
	m_current.m_level->Update(dt);
	m_next.m_level->Update(dt);
}

void LevelManager::Draw(Renderer^ m_renderer)
{
	m_previous.m_level->Draw(m_renderer);
	m_current.m_level->Draw(m_renderer);
	m_next.m_level->Draw(m_renderer);
}

void LevelManager::GenerateNextLevel()
{
	const int nextThemeThreshold = 1000;
	m_previous = std::move(m_current);
	m_current = std::move(m_next);
	m_next = std::move(m_nextnext);
	const Theme* theme = m_next.m_level->GetTheme();
	float xPos = m_current.m_level->StartPoint().x;
	if ((int)(xPos - m_previous.m_level->Width()) % nextThemeThreshold > (int)xPos % nextThemeThreshold)
	{
		theme = &m_themes.at(std::rand() % m_themes.size());
	}

	m_nextnext = LevelData(this, m_next.m_level->EndPoint(), theme);
	
}

const Theme* LevelManager::ThemeByType(Theme::Type type)
{
	for (const Theme& t : m_themes)
	{
		if (t.Style == type)
		{
			return &t;
		}
	}
	return nullptr;
}

void LevelManager::Save(Windows::Storage::Streams::DataWriter^ state)
{
	m_previous.Save(state);
	m_current.Save(state);
	m_next.Save(state);
	m_nextnext.Save(state);
}

void LevelManager::Restore(Windows::Storage::Streams::DataReader^ state)
{
	m_previous.Restore(this, state);
	m_current.Restore(this, state);
	m_next.Restore(this, state);
	m_nextnext.Restore(this, state);
}


LevelManager::LevelData::LevelData(LevelManager* manager, const b2Vec2& pos, const Theme* theme) :
m_index(GetIndex(manager)),
m_level(manager->m_levels.at(m_index)(manager->m_world, pos, theme, 0))
{
#if _DEBUG
	char buf[48];
	sprintf_s(buf, "Create %s\n", typeid(*(m_level.get())).name());
	OutputDebugStringA(buf);
#endif
}

size_t LevelManager::LevelData::GetIndex(LevelManager* manager) const
{
	size_t index = std::rand() % manager->m_levels.size();

	return index;
}

void LevelManager::LevelData::Restore(LevelManager* manager, Windows::Storage::Streams::DataReader^ reader)
{
	m_index = reader->ReadInt32();
	int variant = reader->ReadInt32();
	Theme::Type theme = static_cast<Theme::Type>(reader->ReadInt32());
	m_level = manager->m_levels.at(m_index)(manager->m_world, b2Vec2_zero, manager->ThemeByType(theme), variant);
	m_level->RestoreState(reader);
}

void LevelManager::LevelData::Save(Windows::Storage::Streams::DataWriter^ writer) const
{
	writer->WriteInt32(m_index);
	writer->WriteInt32(m_level->Variant());
	writer->WriteInt32(static_cast<int>(m_level->GetTheme()->Style));
	m_level->SaveState(writer);
}

LevelManager::LevelData::LevelData(LevelData &&o) :
m_index(o.m_index),
m_level(std::move(o.m_level))
{

}

LevelManager::LevelData &LevelManager::LevelData::operator=(LevelData &&o)
{
	if (this != &o)
	{
		m_index = o.m_index;
		m_level = std::move(o.m_level);
	}
	return *this;
}