#include "pch.h"
#include "Physics.h"
#include "Graphics.h"

const Physics::Parameters Physics::DefaultGroundParameters = { b2BodyType::b2_staticBody, 1.0f, 0.7f, 0.0f, Physics::GroundCategory };

b2Fixture* Physics::Touching(b2Contact* c, b2Body* body, int16 category)
{
	if (body == nullptr)
	{
		return nullptr;
	}
	b2Fixture* A = c->GetFixtureA();
	b2Fixture* B = c->GetFixtureB();
	if (A->GetBody() == body && B->GetFilterData().groupIndex == category)
	{
		return B;
	}
	else if (B->GetBody() == body && A->GetFilterData().groupIndex == category)
	{
		return A;
	}
	return nullptr;
}

void Physics::EvaluateContact(b2Contact* c)
{
	b2Fixture* fixtureA = c->GetFixtureA();
	b2Fixture* fixtureB = c->GetFixtureB();
	bool sensorContact = fixtureA->IsSensor() || fixtureB->IsSensor();
	if (sensorContact && c->GetManifold()->pointCount == 0)
	{
		c->Evaluate(c->GetManifold(), fixtureA->GetBody()->GetTransform(), fixtureB->GetBody()->GetTransform());
	}
}


b2Vec2 Physics::ContactVelocity(b2Contact* c, b2Body* body)
{
	EvaluateContact(c);
	b2WorldManifold worldManifold;
	c->GetWorldManifold(&worldManifold);
	b2Vec2 vA = c->GetFixtureA()->GetBody()->GetLinearVelocityFromWorldPoint(worldManifold.points[0]);
	b2Vec2 vB = c->GetFixtureB()->GetBody()->GetLinearVelocityFromWorldPoint(worldManifold.points[0]);
	return (c->GetFixtureA()->GetBody() == body) ? vA - vB : vB - vA;
}


bool Physics::Touching(b2Body* body, b2Vec2& point, b2Vec2& normal, int16 category, bool debug)
{
	for (b2ContactEdge* ce = body->GetContactList(); ce; ce = ce->next)
	{
		b2Contact* c = ce->contact;
		if (c->IsEnabled() && c->IsTouching())
		{
			if (Touching(c, body, category))
			{		
				if (debug)
				{
					CollisionDebug(c);
				}
				EvaluateContact(c);
				b2WorldManifold worldManifold;
				c->GetWorldManifold(&worldManifold);
				normal = (c->GetFixtureA()->GetBody() == body) ? worldManifold.normal : -worldManifold.normal;
				point = worldManifold.points[0];
				return true;
			}
		}
	}
	return false;
}

bool Physics::Touching(b2Body* body, int16 category, bool debug)
{
	for (b2ContactEdge* ce = body->GetContactList(); ce; ce = ce->next)
	{
		b2Contact* c = ce->contact;
		if (c->IsEnabled() && c->IsTouching())
		{
			if (Touching(c, body, category))
			{
				if (debug)
				{
					CollisionDebug(c);
				}
				return true;
			}
		}
	}
	return false;
}

bool Physics::TouchingGround(b2Body* body, b2Vec2& point, b2Vec2& normal, bool debug)
{
	return Touching(body, point, normal, Physics::GroundCategory, debug);
}


bool Physics::TouchingGround(b2Body* body, bool debug)
{
	return Touching(body, Physics::GroundCategory, debug);
}

void Physics::ResetBody(b2Body* body, const b2Vec2& pos)
{
	ResetBody(body, pos, 0.0f, b2Vec2_zero, 0.0f);
}


void Physics::ResetBody(b2Body* body, const b2Vec2& pos, float angle, const b2Vec2& vel, float avel)
{
	body->SetAngularDamping(0.0f);
	body->SetLinearDamping(0.0f);
	body->SetTransform(pos, angle);
	body->SetAngularVelocity(avel);
	body->SetLinearVelocity(vel);
}

void Physics::Serialize(b2Body* body, Windows::Storage::Streams::DataWriter^ stream)
{
	Serialize(body->GetPosition(), stream);
	stream->WriteSingle(body->GetAngle());
	Serialize(body->GetLinearVelocity(), stream);
	stream->WriteSingle(body->GetAngularVelocity());
}

void Physics::Serialize(const b2Vec2& v, Windows::Storage::Streams::DataWriter^ stream)
{
	stream->WriteSingle(v.x);
	stream->WriteSingle(v.y);
}

void Physics::DeSerialize(b2Body* body, Windows::Storage::Streams::DataReader^ stream)
{
	b2Vec2 pos, vel;
	float angle, avel;
	DeSerialize(pos, stream);
	angle = stream->ReadSingle();
	DeSerialize(vel, stream);
	avel = stream->ReadSingle();
	ResetBody(body, pos, angle, vel, avel);
}

void Physics::DeSerialize(b2Vec2& v, Windows::Storage::Streams::DataReader^ stream)
{
	v.x = stream->ReadSingle();
	v.y = stream->ReadSingle();
}

void Physics::SetMotor(b2WheelJoint* joint, float speed, float torque, bool enabled)
{
	joint->SetMotorSpeed(speed);
	joint->SetMaxMotorTorque(torque);
	joint->EnableMotor(enabled);
}


void Physics::SetMotor(b2RevoluteJoint* joint, float speed, float torque, bool enabled)
{
	joint->SetMotorSpeed(speed);
	joint->SetMaxMotorTorque(torque);
	joint->EnableMotor(enabled);
}

b2Body* Physics::CreateLine(b2World* world, const Parameters& params, const b2Vec2& start, const b2Vec2& end, float ratio, const b2Vec2& pos)
{
	b2Vec2 verts[4];
	Graphics::CreateLineFromEndpoint(start, end, ratio, verts);
	return CreatePolygon(world, params, verts, 4, pos);
}

b2Body* Physics::CreateLineFromCorners(b2World* world, const Parameters& params, const b2Vec2& start, const b2Vec2& end, float ratio, const b2Vec2& pos)
{
	b2Vec2 verts[4];
	Graphics::CreateLineFromCornerPoints(start, end, ratio, verts);
	return CreatePolygon(world, params, verts, 4, pos);
}

b2Body* Physics::CreateLine(b2World* world, const Parameters& params, const b2Vec2& end, float ratio, const b2Vec2& pos)
{
	return CreateLine(world, params, b2Vec2_zero, end, ratio, pos);
}

b2Body* Physics::CreateChain(b2World* world, const Parameters& params, const b2Vec2* points, int pointcount, const b2Vec2& pos)
{
	b2ChainShape chainShape;
	chainShape.CreateChain(points, pointcount);
	return CreateBody(world, params, &chainShape, pos);
}

b2Body* Physics::CreatePolygon(b2World* world, const Parameters& params, const b2Vec2* points, int pointcount, const b2Vec2& pos)
{
	b2PolygonShape polygonShape;
	polygonShape.Set(points, pointcount);
	return CreateBody(world, params, &polygonShape, pos);
}

void Physics::AddPolygon(b2Body* body, const Parameters& params, const b2Vec2* points, int pointcount)
{
	b2PolygonShape polygonShape;
	polygonShape.Set(points, pointcount);
	CreateFixture(body, params, &polygonShape);
}

b2Body* Physics::CreateSensor(b2World* world, const Parameters& params, float radius, const b2Vec2& pos)
{
	b2CircleShape circleShape;
	circleShape.m_radius = radius;
	return CreateBody(world, params, &circleShape, pos, true);
}

b2Body* Physics::CreateSensor(b2World* world, const Parameters& params, const b2Vec2& topLeft, const b2Vec2& bottomRight, const b2Vec2& pos)
{
	b2Body* b = CreateBoxAxisAligned(world, params, topLeft, bottomRight, pos);
	if (b && b->GetFixtureList())
	{
		b->GetFixtureList()->SetSensor(true);
	}
	return b;
}

b2Body* Physics::CreateBoxAxisAligned(b2World* world, const Parameters& params, const b2Vec2& topLeft, const b2Vec2& bottomRight, const b2Vec2& pos)
{
	b2Vec2 verts[4];
	Graphics::CreateBoxAxisAligned(topLeft, bottomRight, verts);
	return CreatePolygon(world, params, verts, 4, pos);
}

b2Body* Physics::CreateCircle(b2World* world, const Parameters& params, float radius, const b2Vec2& pos)
{
	b2CircleShape circleShape;
	circleShape.m_radius = radius;
	return CreateBody(world, params, &circleShape, pos);
}

b2Body* Physics::CreateHalfCircle(b2World* world, const Parameters& params, const b2Vec2& perimeter, const b2Vec2& pos)
{
	b2Vec2 verts[b2_maxPolygonVertices];
	Graphics::CreateSolidHalfCircle(b2Vec2_zero, perimeter, verts, b2_maxPolygonVertices - 1);
	verts[b2_maxPolygonVertices - 1] = -perimeter;
	return CreatePolygon(world, params, verts, b2_maxPolygonVertices, pos);
}

b2WheelJoint* Physics::CreateWheelJoint(b2Body* A, b2Body* B, const b2Vec2& pos, const b2Vec2& axis)
{
	b2WheelJointDef jd;
	jd.Initialize(A, B, pos, axis);
	jd.frequencyHz = 8.0f;
	jd.dampingRatio = 0.9f;
	return (b2WheelJoint*)A->GetWorld()->CreateJoint(&jd);
}

b2RevoluteJoint* Physics::CreateRevoluteJoint(b2Body* A, b2Body* B, const b2Vec2& pos)
{
	b2RevoluteJointDef jd;
	jd.Initialize(A, B, pos);
	return (b2RevoluteJoint*)A->GetWorld()->CreateJoint(&jd);
}

void Physics::CreateRevoluteJoint(b2RevoluteJoint*& joint, b2Body* A, b2Body* B, const b2Vec2& pos)
{
	if (joint == NULL)
	{
		joint = CreateRevoluteJoint(A, B, pos);
	}
}

b2RevoluteJoint* Physics::CreateRevoluteJoint(b2Body* A, b2Body* B, const b2Vec2& pos, float hAngle, float lAngle)
{
	b2RevoluteJointDef jd;
	jd.Initialize(A, B, pos);
	jd.enableLimit = true;
	jd.lowerAngle = lAngle;
	jd.upperAngle = hAngle;
	return (b2RevoluteJoint*)A->GetWorld()->CreateJoint(&jd);
}

void Physics::CreateRevoluteJoint(b2RevoluteJoint*& joint, b2Body* A, b2Body* B, const b2Vec2& pos, float hAngle, float lAngle)
{
	if (joint == NULL)
	{
		joint = CreateRevoluteJoint(A, B, pos, hAngle, lAngle);
	}
}

b2DistanceJoint* Physics::CreateDistanceJoint(b2Body* A, b2Body* B, const b2Vec2& pos)
{
	b2DistanceJointDef djd;
	djd.Initialize(A, B, pos, pos);
	djd.frequencyHz = 4.0f;
	djd.collideConnected = false;
	djd.dampingRatio = 1.0f;

	return (b2DistanceJoint*)A->GetWorld()->CreateJoint(&djd);
}

void Physics::CreateDistanceJoint(b2DistanceJoint*& joint, b2Body* A, b2Body* B, const b2Vec2& pos)
{
	if (joint == NULL)
	{
		joint = CreateDistanceJoint(A, B, pos);
	}
}

void Physics::DeleteDistanceJoint(b2DistanceJoint*& joint)
{
	if (joint != NULL)
	{
		DeleteJoint(joint);
		joint = NULL;
	}
}

void Physics::DeleteRevoluteJoint(b2RevoluteJoint*& joint)
{
	if (joint != NULL)
	{
		DeleteJoint(joint);
		joint = NULL;
	}
}

float Physics::HorizontalAngle(const b2Vec2& p1, const b2Vec2& p2)
{
	float dy = p2.y - p1.y;
	float dx = p2.x - p1.x;
	return atan2(dy, dx);
}

float Physics::HorizontalAngle(const Vector2& p1, const Vector2& p2)
{
	float dy = p2.y - p1.y;
	float dx = p2.x - p1.x;
	return atan2(dy, dx);
}

void Physics::DeleteJoint(b2Joint* joint)
{
	b2World* world = joint->GetBodyA()->GetWorld();
	world->DestroyJoint(joint);
}

b2Body* Physics::CreateBody(b2World* world, const Parameters& params, b2Shape* shape, const b2Vec2& pos, bool isSensor)
{
	b2BodyDef bd(pos, params.m_bodyType);
	b2Body* body = world->CreateBody(&bd);
	CreateFixture(body, params, shape, isSensor);
	return body;
}

void Physics::CreateFixture(b2Body* body, const Parameters& params, b2Shape* shape, bool isSensor)
{
	b2FixtureDef fd;
	fd.density = params.m_density;
	fd.friction = params.m_friction;
	fd.filter.groupIndex = params.m_category;
	fd.shape = shape;
	fd.isSensor = isSensor;
	body->CreateFixture(&fd);
}

void Physics::CollisionDebug(b2Contact* c)
{
#if _DEBUG
	char buf[32];
	char* Aname; // [8];
	char* Bname; // [8];
	CollisionCategoryToString(Aname, c->GetFixtureA()->GetFilterData().groupIndex);
	CollisionCategoryToString(Bname, c->GetFixtureB()->GetFilterData().groupIndex);
	sprintf_s(buf, "collision %s %s\n", Aname, Bname);
	OutputDebugStringA(buf);
#endif
}

void Physics::CollisionCategoryToString(char*& name, int16 cat)
{
	switch (cat)
	{
	case Physics::GroundCategory:
		name = "ground";
		break;
	case Physics::PlayerCategory:
		name = "player";
		break;
	case Physics::SensorCategory:
		name = "sensor";
		break;
	default:
		name = "unknown";
		break;
	}
}