#include "pch.h"
#include "MovingPlatform.h"

static const Physics::Parameters platformParams = { b2BodyType::b2_kinematicBody, 1.0f, 0.7f, 0.0f, Physics::GroundCategory };

MovingPlatform::MovingPlatform(b2World* world, const b2Vec2& pos, const b2Vec2& topLeft, const b2Vec2& dim, const b2Color& color) :
m_platform(Physics::CreateBoxAxisAligned(world, platformParams, topLeft, topLeft + dim, pos)),
m_gfx(),
m_topleft(topLeft),
m_dim(dim),
m_end(m_platform->GetPosition()),
m_start(m_platform->GetPosition()),
m_speed(0.0f),
m_stoptime(0.0f),
m_time(0.0f)
{
	Graphics::CreateBoxAxisAligned(topLeft, topLeft + dim, color, m_gfx);
	AddPart(m_platform, &m_gfx);
	Update(0.0f);
}

void MovingPlatform::Update(float dt)
{
	if (m_time < 0.0f)
	{
		b2Vec2 d1 = m_start - m_platform->GetPosition();
		b2Vec2 d2 = m_end - m_platform->GetPosition();
		if (m_platform->GetLinearVelocity() == b2Vec2_zero)
		{		
			if (m_speed > 0.0f)
			{
				b2Vec2 dfar = (d1.Length() > d2.Length()) ? d1 : d2;
				m_time = dfar.Length() / m_speed;
				dfar.Normalize();
				m_platform->SetLinearVelocity(m_speed * dfar);
			}
			else
			{
				m_time = m_stoptime;
			}
		}
		else
		{
			float dotproduct = b2Dot(d1, d2);
			float offset = min(d1.Length(), d2.Length());
			if (offset < 0.1f || dotproduct > 0)
			{
				m_platform->SetLinearVelocity(b2Vec2_zero);
				m_time = m_stoptime;
			}
			else
			{
				m_time = 0.0f;
			}		
		}
	}
	else
	{
		m_time -= dt;
	}
	GameObject::Update(dt);
}

void MovingPlatform::Draw(Renderer^ renderBatch)
{
	renderBatch->AddToBatch(&m_gfx, Renderer::Background);
}

void MovingPlatform::SaveState(Windows::Storage::Streams::DataWriter^ state) const
{
	GameObject::SaveState(state);
	Physics::Serialize(m_start, state);
	Physics::Serialize(m_end, state);
	state->WriteSingle(m_time);
}

void MovingPlatform::RestoreState(Windows::Storage::Streams::DataReader^ state)
{
	GameObject::RestoreState(state);
	Physics::DeSerialize(m_start, state);
	Physics::DeSerialize(m_end, state);
	m_time = state->ReadSingle();
	GameObject::Update(0.0f);
}

MovingPlatform::~MovingPlatform()
{
	if (m_platform)
	{
		m_platform->GetWorld()->DestroyBody(m_platform);
	}
}
