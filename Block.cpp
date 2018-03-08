#include "pch.h"
#include "Block.h"

class BodySearchCallback : public b2QueryCallback
{
public:
	BodySearchCallback(const b2Vec2& point, b2Body* thisBody): m_point(point)
	{
		m_overlappingBody = NULL;
		m_thisBody = thisBody;
	}

	bool ReportFixture(b2Fixture* fixture)
	{
		b2Body* body = fixture->GetBody();
		if (body != m_thisBody)
		{
			bool inside = fixture->TestPoint(m_point);
			if (inside)
			{
				m_overlappingBody = body;

				// We are done, terminate the query.
				return false;
			}
		}

		// Continue the query.
		return true;
	}

	b2Vec2 m_point;
	b2Body* m_overlappingBody;
	b2Body* m_thisBody;
};

Block::Block(b2World* world, const b2Vec2& start, const b2Vec2& end)
{
	m_body = CreateBlock(world, start, end);
	m_joint1 = CreateJoint(start, GetOverlappingBody(world, m_body, start));
	m_joint2 = CreateJoint(end, GetOverlappingBody(world, m_body, end));
}

Block::Block(b2World* world, b2Body* connected, const b2Vec2& start, const b2Vec2& end)
{
	m_body = CreateBlock(world, start, end);
	m_joint1 = CreateJoint(start, connected);
	m_joint2 = CreateJoint(end, GetOverlappingBody(world, m_body, end));
}

b2Body* Block::CreateBlock(b2World* world, const b2Vec2& start, const b2Vec2& end)
{
	b2BodyDef bd;
	bd.type = b2_dynamicBody;
	bd.position = b2Vec2((start.x + end.x) * 0.5f, (start.y + end.y) * 0.5f);
	b2Body* newBody = world->CreateBody(&bd);

	b2Vec2 dif = end - start;
	const float overhang = 0.2f;
	const float blockHeight = 1.5f;
	float blockLength = dif.Length() + 2 * overhang;
	float angle = (dif.x == 0.0f) ? b2_pi / 2 : atan(dif.y / dif.x);
	b2PolygonShape blockShape;
	blockShape.SetAsBox(blockLength, blockHeight, b2Vec2_zero, angle);

	b2FixtureDef blockFixtureDef;
	blockFixtureDef.density = 1.0f;
	blockFixtureDef.shape = &blockShape;
	blockFixtureDef.filter.groupIndex = -1;
	newBody->CreateFixture(&blockFixtureDef);
	return newBody;
}


b2RevoluteJoint* Block::CreateJoint(const b2Vec2& pos, b2Body* bodyB)
{
	if (bodyB == NULL)
		return NULL;
	b2RevoluteJointDef rjd;
	rjd.Initialize(m_body, bodyB, pos);

	rjd.lowerAngle = -b2_pi * 0.1;
	rjd.upperAngle = b2_pi * 0.1;
	rjd.enableMotor = true;
	rjd.collideConnected = false;
	rjd.enableLimit = true;
	rjd.maxMotorTorque = 1000.0f;
	rjd.motorSpeed = 0.0f;
	return (b2RevoluteJoint*)m_body->GetWorld()->CreateJoint(&rjd);
}

b2Body* Block::GetOverlappingBody(b2World* world, b2Body* body, const b2Vec2& p)
{
	b2AABB aabb;
	b2Vec2 d;
	d.Set(0.001f, 0.001f);
	aabb.lowerBound = p - d;
	aabb.upperBound = p + d;

	// Query the world for overlapping shapes.
	BodySearchCallback callback(p, body);
	world->QueryAABB(&callback, aabb);
	return callback.m_overlappingBody;
}