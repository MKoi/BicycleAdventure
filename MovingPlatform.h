#pragma once
#include "Box2D\Box2D.h"
#include "GameObject.h"
#include "Graphics.h"
#include "VerticeBatch.h"
#include "Physics.h"


class MovingPlatform : public GameObject
{
public:

	MovingPlatform(b2World* world, const b2Vec2& pos, const b2Vec2& topLeft, const b2Vec2& dim, const b2Color& color);

	void SetSpeed(float speed) { m_speed = speed; }

	void SetSwitchPoint(const b2Vec2& p) { m_end = p; }

	void SetStopTime(float t) { m_stoptime = t; }

	virtual void Update(float dt);

	virtual void Draw(Renderer^ renderBatch);

	virtual void SaveState(Windows::Storage::Streams::DataWriter^ state) const;

	virtual void RestoreState(Windows::Storage::Streams::DataReader^ state);

	virtual ~MovingPlatform();
private:
	b2Body* m_platform;
	VerticeBatch m_gfx;
	b2Vec2 m_topleft;
	b2Vec2 m_dim;
	b2Vec2 m_end;
	b2Vec2 m_start;
	float m_speed;
	float m_stoptime;
	float m_time;
};
