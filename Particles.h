#pragma once
#include <deque> 
#include "Box2D\Box2D.h"
#include "Renderer.h"
#include "Graphics.h"
#include "VerticeBatch.h"

class Particles
{
public:
	Particles(b2World* world, const b2Color& color = Graphics::black, float alpha = 1.0f);
	void Emit(size_t count, const b2Vec2& pos, const b2Vec2& force, bool collide = true);
	void Update(float dt);
	void Draw(Renderer^ renderer) const;
	void SetPosDelta(float d) { m_posDelta = d; }
	void SetVelDelta(float d) { m_velDelta = d; }
	void Clear();
	virtual ~Particles();

private:
	b2Vec2 Randomize(float x1, float x2, float y1, float y2);
	float Randomize(float x1, float x2);
	static const size_t m_max = 50;
	b2Vec2 m_vertices[3];
	b2World* m_world;
	struct Particle
	{
		float m_lifetime;
		b2Body* m_body;
		VerticeBatch m_gfx;
	};
	VerticeBatch m_particleGfx;
	std::deque<Particle> m_particles;
	float m_posDelta;
	float m_velDelta;
//	RandomFloat m_random;
};
