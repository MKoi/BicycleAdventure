#pragma once
#include <functional>
#include "Box2D\Box2D.h"
#include "GameObject.h"


class Script : public GameObject
{
public:
	Script(std::function<void()> action, b2World* world, const b2Vec2& topLeft, const b2Vec2& bottomRight, const b2Vec2& pos, int16 collisionfilter = 0);
	Script(std::function<void()> action, b2World* world, const b2Vec2& sensorPos, float sensorRadius, int16 collisionfilter = 0);
	Script();
	virtual void Update(float dt);
	virtual void BeginContact(b2Contact* c);
	virtual void SaveState(Windows::Storage::Streams::DataWriter^ state) const;
	virtual void RestoreState(Windows::Storage::Streams::DataReader^ state);
	bool ActionTriggered() const { return m_actionTriggered; }
	void SetAction(std::function<void()> action);
	void SetTrigger(b2World* world, const b2Vec2& sensorPos, float sensorRadius, int16 collisionfilter = 0);
	void SetTrigger(b2World* world, const b2Vec2& topLeft, const b2Vec2& bottomRight, const b2Vec2& pos, int16 collisionfilter = 0);
	void SetMinMass(float m = 0.0f);
	~Script();
private:
	b2Body* m_trigger;
	std::function<void()> m_action;
	bool m_actionTriggered;
	bool m_triggerHit;
	int16 m_collisionFilter;
	float m_minMass;
};
