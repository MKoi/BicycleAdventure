#include "pch.h"
#include "Particles.h"
#include "Physics.h"
#include "Graphics.h"

Particles::Particles(b2World* world, const b2Color& color, float alpha) : m_world(world), m_particleGfx(), m_particles(), m_posDelta(0.1f), m_velDelta(0.2f)
{
	m_vertices[0].Set(0.0f, 0.05f);
	m_vertices[1].Set(-0.05f, -0.025f);
	m_vertices[2].Set(0.05f, -0.025f);
	Graphics::CreateTriangle(m_vertices, color, m_particleGfx, alpha);
}

void Particles::Emit(size_t count, const b2Vec2& pos, const b2Vec2& velocity, bool collide)
{
	const Physics::Parameters params = { b2BodyType::b2_dynamicBody, 0.1f, 1.0f, 0.1f, Physics::PlayerCategory };
	b2Vec2 vel_perp = b2Cross(velocity, 1.0f);
	for (size_t i = 0; i < count && m_particles.size() < m_max; ++i)
	{
		
		m_particles.emplace_back();
		b2Vec2 posdelta = Randomize(-m_posDelta, m_posDelta, -m_posDelta, m_posDelta);
		m_particles.back().m_body = Physics::CreatePolygon(m_world, params, m_vertices, 3, pos + posdelta);
		if (!collide)
		{
			b2Filter filter = m_particles.back().m_body->GetFixtureList()->GetFilterData();
			filter.maskBits = 0x0;
			m_particles.back().m_body->GetFixtureList()->SetFilterData(filter);
		}
		float f = Randomize(-m_velDelta, m_velDelta);
		m_particles.back().m_body->ApplyLinearImpulse((1.0f + abs(f)) * velocity + f * vel_perp, b2Vec2_zero, true);
		m_particles.back().m_gfx = m_particleGfx;
		m_particles.back().m_lifetime = 2.0f;
	}
}

void Particles::Update(float dt)
{
	if (dt == 0.0f)
	{
		return;
	}
	while (!m_particles.empty() && m_particles.front().m_lifetime - dt < 0.0)
	{
		m_world->DestroyBody(m_particles.front().m_body);
		m_particles.pop_front();
	}
	for (Particle& p : m_particles)
	{
		p.m_lifetime -= dt;
		p.m_gfx.Update(dt, p.m_body->GetAngle(), p.m_body->GetPosition());
	}
}

void Particles::Draw(Renderer^ renderer) const
{
	for (const Particle& p : m_particles)
	{
		renderer->AddToBatch(&(p.m_gfx));
	}
}

void Particles::Clear()
{
	while (!m_particles.empty())
	{
		m_world->DestroyBody(m_particles.front().m_body);
		m_particles.pop_front();
	}
}

Particles::~Particles()
{
	Clear();
}

b2Vec2 Particles::Randomize(float x1, float x2, float y1, float y2)
{
	b2Vec2 r;
	r.x = Randomize(x1, x2);
	r.y = Randomize(y1, y2);
	return r;
}

float Particles::Randomize(float x1, float x2)
{
	if (x1 == x2)
	{
		return x1;
	}
	else if (x1 > x2)
	{
		return x2 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (x1 - x2)));
	}
	else
	{
		return x1 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (x2 - x1)));
	}
}