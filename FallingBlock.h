#pragma once
#include "Box2D\Box2D.h"
#include "GameObject.h"
#include "GameAudio.h"

class FallingBlock : public GameObject
{
public:
	b2Body* m_block;
	virtual void BeginContact(b2Contact* c);
	virtual void EndContact(b2Contact* c) {}
	virtual ~FallingBlock() { m_block->SetUserData(nullptr); }
};
