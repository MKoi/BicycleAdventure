#pragma once
#include <vector>
#include "Box2D\Box2D.h"
#include "GameArea.h"
#include "Theme.h"
#include "Graphics.h"
#include "Background.h"

class LevelManager
{
public:
	LevelManager(b2World* world);
	void Update(float dt);
	void Draw(Renderer^ m_renderer);
	void Reset();
	const GameArea* PreviousLevel() const { return m_previous.m_level.get(); }
	const GameArea* CurrentLevel() const { return m_current.m_level.get(); }
	const GameArea* NextLevel() const { return m_next.m_level.get(); }
	const GameArea* NextNextLevel() const { return m_nextnext.m_level.get(); }
	void GenerateNextLevel();
	void Save(Windows::Storage::Streams::DataWriter^ state);
	void Restore(Windows::Storage::Streams::DataReader^ state);

private:
	const Theme* ThemeByType(Theme::Type type);

	b2World* m_world;
	std::vector<Theme> m_themes;

	typedef std::unique_ptr<GameArea>(*factory_ptr)(b2World* world, const b2Vec2& pos, const Theme* theme, int variant);
	std::vector<factory_ptr> m_levels;
	class LevelData
	{
	private:
		size_t GetIndex(LevelManager* manager) const;
		LevelData(LevelData const &);
		LevelData &operator=(LevelData const &);
		size_t m_index;
	public:
		LevelData() : m_index(0), m_level() {}
		LevelData(LevelManager* manager, const b2Vec2& pos, const Theme* theme);
		LevelData(LevelData &&o);
		LevelData &operator=(LevelData &&o);
		void Restore(LevelManager* manager, Windows::Storage::Streams::DataReader^ reader);
		void Save(Windows::Storage::Streams::DataWriter^ writer) const;

		std::unique_ptr<GameArea> m_level;
	};

	LevelData m_current;
	LevelData m_previous;
	LevelData m_next;
	LevelData m_nextnext;


};