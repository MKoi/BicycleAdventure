#include "pch.h"
#include "Script.h"
#include "Physics.h"

const Physics::Parameters sensorparams = { b2BodyType::b2_staticBody, 1.0f, 0.7f, 0.0f, Physics::SensorCategory };

Script::Script(std::function<void()> action, b2World* world, const b2Vec2& topLeft, const b2Vec2& bottomRight, const b2Vec2& pos, int16 collisionFilter) :
m_trigger(Physics::CreateSensor(world, sensorparams, topLeft, bottomRight, pos)),
m_action(action),
m_actionTriggered(false),
m_triggerHit(false),
m_collisionFilter(collisionFilter),
m_minMass(0.0f)
{
	m_trigger->SetUserData(this);
}

Script::Script(std::function<void()> action, b2World* world, const b2Vec2& sensorPos, float sensorRadius, int16 collisionFilter) :
m_trigger(Physics::CreateSensor(world, sensorparams, sensorRadius, sensorPos)), 
m_action(action), 
m_actionTriggered(false),
m_triggerHit(false),
m_collisionFilter(collisionFilter),
m_minMass(0.0f)
{
	m_trigger->SetUserData(this);
}

Script::Script() :
m_trigger(nullptr),
m_action(nullptr),
m_actionTriggered(false),
m_triggerHit(false),
m_minMass(0.0f)
{

}

void Script::Update(float dt)
{
	if (m_action && m_triggerHit)
	{
		m_action();		
		m_triggerHit = false;
	}
}

void Script::BeginContact(b2Contact* c) 
{
	if (m_trigger && !m_actionTriggered)
	{
		b2Fixture* other = Physics::Touching(c, m_trigger, m_collisionFilter);
		if (m_collisionFilter == 0 || other)
		{
			if (m_minMass == 0.0f || (other && other->GetBody()->GetMass() > m_minMass))
			{
				m_actionTriggered = true;
				m_triggerHit = true;
			}
		}
	}
}


void Script::SaveState(Windows::Storage::Streams::DataWriter^ state) const
{
	if (m_trigger)
	{
		state->WriteSingle(m_minMass);
		state->WriteBoolean(m_actionTriggered);
		state->WriteInt16(m_collisionFilter);
		Physics::Serialize(m_trigger, state);
	}
}

void Script::RestoreState(Windows::Storage::Streams::DataReader^ state)
{
	if (m_trigger)
	{
		m_minMass = state->ReadSingle();
		m_actionTriggered = state->ReadBoolean();
		m_collisionFilter = state->ReadInt16();
		Physics::DeSerialize(m_trigger, state);
		if (m_action && m_actionTriggered)
		{
			m_action();
		}
	}
}

void Script::SetAction(std::function<void()> action)
{
	if (!m_action)
	{
		m_action = action;
	}
}


void Script::SetTrigger(b2World* world, const b2Vec2& sensorPos, float sensorRadius, int16 collisionFilter)
{
	if (!m_trigger)
	{
		m_collisionFilter = collisionFilter;
		m_trigger = Physics::CreateSensor(world, sensorparams, sensorRadius, sensorPos);
		m_trigger->SetUserData(this);
	}
}


void Script::SetTrigger(b2World* world, const b2Vec2& topLeft, const b2Vec2& bottomRight, const b2Vec2& pos, int16 collisionFilter)
{
	if (!m_trigger)
	{
		m_collisionFilter = collisionFilter;
		m_trigger = Physics::CreateSensor(world, sensorparams, topLeft, bottomRight, pos);
		m_trigger->SetUserData(this);
	}
}

void Script::SetMinMass(float m)
{
	m_minMass = m;
}

Script::~Script()
{
	if (m_trigger)
	{
		m_trigger->GetWorld()->DestroyBody(m_trigger);
		m_trigger = nullptr;
	}
}
