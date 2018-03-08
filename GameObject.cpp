#include "pch.h"
#include "GameObject.h"
#include "Physics.h"

void GameObject::AddPart(b2Body* body, VerticeBatch* gfx)
{
	Part newPart = { body, gfx };
	newPart.m_body->SetUserData(this);
	m_parts.push_back(newPart);
}

void GameObject::Update(float dt)
{
	for (auto p : m_parts)
	{
		if (p.m_body && p.m_gfx)
		{
			p.m_gfx->Update(dt, p.m_body->GetAngle(), p.m_body->GetPosition());
		}
	}
}

void GameObject::Draw(Renderer^ renderer) const
{
	for (auto p : m_parts)
	{
		if (p.m_gfx)
		{
			renderer->AddToBatch(p.m_gfx);
		}
	}
}


void GameObject::SaveState(Windows::Storage::Streams::DataWriter^ state) const
{
	for (auto p : m_parts)
	{
		if (p.m_body)
		{
			Physics::Serialize(p.m_body, state);
		}
	}
	
}

void GameObject::RestoreState(Windows::Storage::Streams::DataReader^ state)
{
	for (auto p : m_parts)
	{
		if (p.m_body)
		{
			Physics::DeSerialize(p.m_body, state);
		}
	}
}
