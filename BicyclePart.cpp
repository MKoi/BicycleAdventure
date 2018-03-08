#include "pch.h"
#include "BicyclePart.h"

using namespace DirectX;

BicyclePart::BicyclePart(b2World* world, const b2BodyDef& bd): m_body(world->CreateBody(&bd))
{

}

void BicyclePart::Init(b2Shape* shape, const Parameters& parameters)
{
	b2FixtureDef fd;
	fd.density = parameters.m_density;
	fd.friction = parameters.m_friction;
	fd.filter.groupIndex = parameters.m_category;
	fd.shape = shape;
	m_body->CreateFixture(&fd);
}

void BicyclePart::GetBoxShape(b2Vec2* points, const b2Vec2& start, const b2Vec2& end, float ratio) const
{
	b2Vec2 diff = end - start;
	points[0] = b2Vec2(start.x + ratio*diff.y, start.y - ratio*diff.x);
	points[1] = b2Vec2(end.x + ratio*diff.y, end.y - ratio*diff.x);
	points[2] = b2Vec2(end.x - ratio*diff.y, end.y + ratio*diff.x);
	points[3] = b2Vec2(start.x - ratio*diff.y, start.y + ratio*diff.x);
}

float BicyclePart::GetAngle() const
{
	return DirectX::XMScalarModAngle(m_body->GetAngle());
#if 0
	float sinX = m_body->GetTransform().q.s;
	float cosX = m_body->GetTransform().q.c;
	float angle = atan2(sinX, cosX);
#endif
}

void BicyclePart::Reset(const b2Vec2& pos)
{
	m_body->SetTransform(pos, 0.0f);
	m_body->SetAngularVelocity(0.0f);
	m_body->SetLinearVelocity(b2Vec2_zero);
}


void BicyclePart::ConnectTo(BicyclePart* other, b2JointDef* jd)
{
	jd->bodyA = other->m_body;
	jd->bodyB = this->m_body;
	jd->collideConnected = false;
}

bool BicyclePart::TouchingGround() const
{
	for (b2ContactEdge* ce = m_body->GetContactList(); ce; ce = ce->next)
	{
		if (IsGroundContact(ce->contact))
		{
			return true;
		}
	}
	return false;
}

bool BicyclePart::IsGroundContact(b2Contact* c) const
{
	if (c->IsEnabled() && c->IsTouching())
	{
		b2Fixture* A = c->GetFixtureA();
		b2Fixture* B = c->GetFixtureB();
		if (A->GetFilterData().groupIndex != B->GetFilterData().groupIndex)
		{
			return true;
		}
	}
	return false;
}

Frame::Frame(b2World* world, const b2BodyDef& bd, const Parameters& parameters, const FramePoints& framepoints): 
	BicyclePart(world, bd), m_points(framepoints)
{
	b2PolygonShape frameShape;
	b2Vec2 vertices[3];
	vertices[0] = framepoints.m_hub;
	vertices[1] = framepoints.m_handleBar;
	vertices[2] = framepoints.m_saddle;
	frameShape.Set(vertices, 3);
	Init(&frameShape, parameters);
}

void Frame::Draw(DebugDraw* drawer) const
{
	const int count = 4;
	b2Vec2 vertices[count];
	GetBoxShape(vertices, m_body->GetWorldPoint(m_points.m_hub), m_body->GetWorldPoint(m_points.m_saddle), 0.03f);
	b2Color red(1.0f, 0.0f, 0.0f);
	drawer->DrawSolidPolygon(vertices, count, red);
	GetBoxShape(vertices, m_body->GetWorldPoint(m_points.m_saddle), m_body->GetWorldPoint(m_points.m_handleBar), 0.05f);
	drawer->DrawSolidPolygon(vertices, count, red);
	GetBoxShape(vertices, m_body->GetWorldPoint(m_points.m_handleBar), m_body->GetWorldPoint(m_points.m_hub), 0.05f);
	drawer->DrawSolidPolygon(vertices, count, red);

}

b2Vec2 Frame::GetHub() const
{
	return m_body->GetWorldPoint(m_points.m_hub);
}

b2Vec2 Frame::GetSaddle() const
{
	return m_body->GetWorldPoint(m_points.m_saddle);
}

b2Vec2 Frame::GetHandleBar() const
{
	return m_body->GetWorldPoint(m_points.m_handleBar);
}

Wheel::Wheel(b2World* world, const b2BodyDef& bd, const Parameters& parameters, float radius): BicyclePart(world, bd)
{
	b2CircleShape circleShape;
	circleShape.m_radius = radius;
	Init(&circleShape, parameters);
}

void Wheel::Draw(DebugDraw* drawer) const
{
	XMVECTOR scale = XMVectorReplicate(1.0f);
	XMVECTOR origin = XMVectorSet(m_body->GetPosition().x, m_body->GetPosition().y, 0.0f, 0.0f);
	const float32 theta = XM_2PI / 25.0f;
	XMVECTOR translation =  XMVectorZero();
	XMMATRIX transform = XMMatrixAffineTransformation2D(scale, origin, theta, translation);
	b2Vec2 perimeterPoint = m_body->GetWorldPoint(b2Vec2(GetRadius(), 0.0f));
	XMVECTOR point = XMVectorSet(perimeterPoint.x, perimeterPoint.y, 0.0f, 0.0f);
	//XMVECTOR point = XMVectorSet(m_body->GetPosition().x + GetRadius(), m_body->GetPosition().y, 0.0f, 0.0f);
	XMVECTOR point2;	
	for (int i = 0; i < 25; ++i)
	{
		point2 = XMVector2Transform(point, transform);
		b2Vec2 diff1(XMVectorGetX(point) - m_body->GetPosition().x, XMVectorGetY(point) - m_body->GetPosition().y);
		b2Vec2 diff2(XMVectorGetX(point2) - m_body->GetPosition().x, XMVectorGetY(point2) - m_body->GetPosition().y);
		b2Vec2 vertices[4];
		b2Color black(0.0f, 0.0f, 0.0f);
		b2Color grey(0.9f, 0.9f, 0.9f);
		float ratio1 = 0.15f;
		vertices[0].Set(m_body->GetPosition().x, m_body->GetPosition().y);
		vertices[1].Set(m_body->GetPosition().x + ratio1*diff1.x, m_body->GetPosition().y + ratio1*diff1.y);
		vertices[2].Set(m_body->GetPosition().x + ratio1*diff2.x, m_body->GetPosition().y + ratio1*diff2.y);
		
		drawer->DrawSolidPolygon(vertices, 3, black);
		float ratio2 = 0.2f;
		GetSectorShape(vertices, m_body->GetPosition(), diff1, diff2, ratio1, ratio2);		
		drawer->DrawSolidPolygon(vertices, 4, grey);

		ratio1 = ratio2;
		ratio2 = 0.85f;
		GetBoxShape(vertices, m_body->GetPosition() + ratio1*diff1, m_body->GetPosition() + ratio2*diff1, 0.02);
		drawer->DrawSolidPolygon(vertices, 4, grey);

		ratio1 = ratio2;
		ratio2 = 0.9f;
		GetSectorShape(vertices, m_body->GetPosition(), diff1, diff2, ratio1, ratio2);
		drawer->DrawSolidPolygon(vertices, 4, grey);
		
		ratio1 = ratio2;
		ratio2 = 1.0f;
		GetSectorShape(vertices, m_body->GetPosition(), diff1, diff2, ratio1, ratio2);
		drawer->DrawSolidPolygon(vertices, 4, black);

		GetBoxShape(vertices, m_body->GetPosition(), b2Vec2(XMVectorGetX(point),XMVectorGetY(point)), 0.02);
		drawer->DrawSolidPolygon(vertices, 4, grey);
		point = point2;
	}
}

void Wheel::GetSectorShape(b2Vec2* vertices, const b2Vec2& center, const b2Vec2& r1, const b2Vec2& r2, 
		float ratio1, float ratio2) const
{
	vertices[0].Set(center.x + ratio1*r1.x, center.y + ratio1*r1.y);
	vertices[1].Set(center.x + ratio2*r1.x, center.y + ratio2*r1.y);
	vertices[2].Set(center.x + ratio2*r2.x, center.y + ratio2*r2.y);
	vertices[3].Set(center.x + ratio1*r2.x, center.y + ratio1*r2.y);

}


void Wheel::Reset(const b2Vec2& pos)
{
	b2Vec2 wheelPos(pos.x + m_fork->GetLocalAxisA().x, pos.y);
	m_body->SetTransform(wheelPos, 0.0f);
	m_body->SetAngularVelocity(0.0f);
	m_body->SetLinearVelocity(b2Vec2_zero);
}


void Wheel::ConnectTo(BicyclePart* other, b2WheelJointDef* jd)
{
	b2Vec2 wheelPos(other->GetPosition().x + jd->localAxisA.x, other->GetPosition().y);
	m_body->SetTransform(wheelPos, m_body->GetAngle());
	BicyclePart::ConnectTo(other, jd);
	jd->localAnchorA = jd->bodyA->GetLocalPoint(wheelPos);
	jd->localAnchorB = jd->bodyB->GetLocalPoint(wheelPos);
	SuspensionParameters suspension;
	jd->frequencyHz = suspension.m_hz;
	jd->dampingRatio = suspension.m_zeta;
	m_fork = (b2WheelJoint*)m_body->GetWorld()->CreateJoint(jd);
}

void Wheel::SetMotor(float speed, float torque, bool enable)
{ 
	m_fork->SetMaxMotorTorque(torque);
	m_fork->SetMotorSpeed(speed);
	m_fork->EnableMotor(enable);
}

Pedals::Pedals(b2World* world, const b2BodyDef& bd, const Parameters& parameters, float radius): 
	BicyclePart(world, bd), m_leftPedal(radius, 0.0f), m_rightPedal(-radius, 0.0f)
{
	b2CircleShape circleShape;
	circleShape.m_radius = radius;
	Init(&circleShape, parameters);
}


void Pedals::Draw(DebugDraw* drawer) const
{

}

void Pedals::ConnectTo(Frame* other, b2RevoluteJointDef* jd)
{
	m_body->SetTransform(other->GetHub(), m_body->GetAngle());
	BicyclePart::ConnectTo(other, jd);
	b2Vec2 pos = m_body->GetPosition();
	jd->localAnchorA = jd->bodyA->GetLocalPoint(m_body->GetPosition());
	jd->localAnchorB = jd->bodyB->GetLocalPoint(m_body->GetPosition());
	m_hub = (b2RevoluteJoint*)m_body->GetWorld()->CreateJoint(jd);
}

void Pedals::SetMotor(float speed, float torque, bool enable)
{
	m_hub->SetMaxMotorTorque(torque);
	m_hub->SetMotorSpeed(speed);
	m_hub->EnableMotor(enable);
}



