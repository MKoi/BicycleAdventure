#include "pch.h"
#include "Water.h"
#include "Physics.h"
#include "Graphics.h"

const Physics::Parameters WaterParameters = { b2BodyType::b2_staticBody, 0.5f, 0.7f, 0.0f, Physics::GroundCategory };

Water::Water(b2World* world) :
m_splash(world, Graphics::blue.Lighter(), 0.5f),
m_body(),
m_gfx(),
m_fixturesInWater(),
m_newsplash(),
m_addedFixtures(),
m_audio(nullptr)
{
}

Water::Water(b2World* world, const b2Vec2& pos, b2Vec2* vertices, size_t count):
m_splash(world),
m_body(),
m_gfx(),
m_fixturesInWater(),
m_newsplash(),
m_addedFixtures(),
m_audio(nullptr)
{
	Set(world, pos, vertices, count);
	m_splash.SetPosDelta(0.2f);
	m_splash.SetVelDelta(0.5f);
}

void Water::Set(b2World* world, const b2Vec2& pos, b2Vec2* vertices, size_t count, GameAudio* audio)
{
	m_body = Physics::CreatePolygon(world, WaterParameters, vertices, count, pos);
	m_body->GetFixtureList()->SetSensor(true);
	Graphics::CreateSolidConvex(vertices, count, Graphics::blue.Lighter(), m_gfx, 0.5f);
	AddPart(m_body, &m_gfx);
	Update(0.0f);
	m_audio = audio;
}

void Water::Update(float dt)
{
	GameObject::Update(dt);
	if (!m_addedFixtures.empty() && m_audio != nullptr)
	{
		float maxp = 0.0f;
		for (b2Fixture* a : m_addedFixtures)
		{
			b2Vec2 p = b2Vec2_zero;
			if (a && a->GetBody())
			{
				p = a->GetBody()->GetMass() * a->GetBody()->GetLinearVelocity();
			}
			maxp = max(maxp, p.Length());
		}
		float vol = min(1.0f, maxp / 50.0f);
		m_audio->Play(GameAudio::eSplash, vol);
		m_addedFixtures.clear();
	}
	m_splash.Update(dt);
	for (const NewSplash& splash : m_newsplash) 
	{
		m_splash.Emit(20, splash.pos, splash.vel, false);
	}
	/*
	for (b2Fixture* f : m_fixturesInWater)
	{
		b2Vec2 vel = f->GetBody()->GetLinearVelocity();
		float l = vel.Normalize();
		float dragMag = f->GetDensity() * l * l;
		b2Vec2 dragForce = dragMag * -vel;
		f->GetBody()->ApplyForceToCenter(dragForce, true);
	}
	*/
	m_newsplash.clear();
}

void Water::Draw(Renderer^ renderBatch)
{
	renderBatch->AddToBatch(&m_gfx, Renderer::Foreground);
	m_splash.Draw(renderBatch);
}

void Water::BeginContact(b2Contact* c)
{
	//b2Fixture* other = Physics::Touching(c, m_body, Physics::PlayerCategory);
	b2Fixture* other = (c->GetFixtureA()->GetBody() == m_body) ? c->GetFixtureB() : c->GetFixtureA();
	// process only collision to big enough bodies
	if (other && other->GetBody()->GetMass() > 0.001f)
	{
		m_fixturesInWater.insert(other);
		m_addedFixtures.push_back(other);

		other->GetBody()->SetLinearDamping(10.0f);
		other->GetBody()->SetAngularDamping(10.0f);
		Physics::EvaluateContact(c);
		b2WorldManifold worldManifold;
		c->GetWorldManifold(&worldManifold);
		b2Vec2 vel = (c->GetFixtureA()->GetBody() == m_body)
			? c->GetFixtureB()->GetBody()->GetLinearVelocity()
			: c->GetFixtureA()->GetBody()->GetLinearVelocity();
		b2Vec2 point = worldManifold.points[0];
		point.y += 0.2f;
		vel *= -0.0003f;
		m_newsplash.emplace_back(point, vel);
	}
}


void Water::EndContact(b2Contact* contact)
{
	b2Fixture* other = Physics::Touching(contact, m_body, Physics::PlayerCategory);
	if (other)
	{
		other->GetBody()->SetLinearDamping(0.0f);
		other->GetBody()->SetAngularDamping(0.0f);
		m_fixturesInWater.erase(other);
	}
}

void Water::RestoreState(Windows::Storage::Streams::DataReader^ state)
{
	GameObject::RestoreState(state);
	Update(0.0f);
}

Water::~Water()
{
	if (m_body)
	{
		m_body->GetWorld()->DestroyBody(m_body);
		m_body = nullptr;
	}
}