#pragma once
#include <set>
#include <vector>
#include "Box2D\Box2D.h"
#include "GameObject.h"
#include "Particles.h"
#include "GameAudio.h"

class Water : public GameObject
{
public:
	Water(b2World* world);
	Water(b2World* world, const b2Vec2& pos, b2Vec2* vertices, size_t count);
	void Set(b2World* world, const b2Vec2& pos, b2Vec2* vertices, size_t count, GameAudio* audio = nullptr);
	void Update(float dt);
	void Draw(Renderer^ renderBatch);

	void BeginContact(b2Contact* contact);
	void EndContact(b2Contact* contact);

	virtual void RestoreState(Windows::Storage::Streams::DataReader^ state);

	virtual ~Water();
private:
	struct NewSplash
	{
		NewSplash(const b2Vec2& p, const b2Vec2& v) : pos(p), vel(v) {}
		b2Vec2 pos;
		b2Vec2 vel;
	};
	Particles m_splash;
	b2Body* m_body;
	VerticeBatch m_gfx;
	std::set<b2Fixture*> m_fixturesInWater;
	std::vector<NewSplash> m_newsplash;
	std::vector<b2Fixture*> m_addedFixtures;
	GameAudio* m_audio;
};
	