#pragma once
#include <vector>
#include <Box2D/Box2D.h>
#include "Renderer.h"
#include "VerticeBatch.h"

class GameObject
{
public:
	void AddPart(b2Body* body, VerticeBatch* gfx);
	virtual void Update(float dt);
	virtual void Draw(Renderer^ renderer) const;
	virtual void BeginContact(b2Contact* c) {}
	virtual void EndContact(b2Contact* c) {}
	virtual void SaveState(Windows::Storage::Streams::DataWriter^ state) const;
	virtual void RestoreState(Windows::Storage::Streams::DataReader^ state);

private:
	struct Part
	{
		b2Body* m_body;
		VerticeBatch* m_gfx;
	};
	std::vector<Part> m_parts;
//	std::vector<b2Fixture*> m_contacts;
};