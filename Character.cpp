#include "pch.h"
#include "Character.h"

/*
class MyRayCastCallback : public b2RayCastCallback
{
public:
    MyRayCastCallback(b2Body* body): m_fixture(NULL), m_discardBody(body)
    {
    }

    float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction)
    {
		if (m_fixture->GetBody() == m_discardBody)
		{
			return -1.0f;
		}
        m_fixture = fixture;
        m_point = point;
        m_normal = normal;
        m_fraction = fraction;
        return fraction;
    }

	b2Body* m_discardBody;
    b2Fixture* m_fixture;
    b2Vec2 m_point;
    b2Vec2 m_normal;
    float32 m_fraction;
};
*/

Character::Character(b2World* world, const b2Vec2& position): k_headDensity(0.1f), k_BodyDensity(1.0f), m_direction(Direction::STOP), m_timeSinceLastMove(0.0f)
{
	CreateTorso(world, position);
	CreateHead(world);
	CreateArms(world);
	CreateFeet(world);
	m_mass = GetMass();
}

void Character::Update(float timeDelta)
{
	if (m_direction == Direction::STOP || m_direction == Direction::FALLEN)
		return;
	if (m_timeSinceLastMove > 0.5f && FeetOnGround())
	{
		float fy = abs(m_head->GetWorld()->GetGravity().y) * m_mass;
		fy *= 0.5f;
		float fx = m_direction == Direction::LEFT ? -fy : fy;
		fx *= 0.5f;
		b2Vec2 force(fx, fy);
		m_head->ApplyLinearImpulse(force, m_head->GetPosition(), true);
		//m_head->ApplyForceToCenter(force, true);
		m_timeSinceLastMove = 0.0f;
	}
	else
	{
		m_timeSinceLastMove += timeDelta;
	}

}

void Character::Move(Direction direction)
{
	m_direction = direction;
}

void Character::FallDown(Direction direction)
{
	if (m_direction == Direction::FALLEN)
		return;
	m_torso->SetFixedRotation(false);
	float fy = abs(m_head->GetWorld()->GetGravity().y) * m_mass;
	fy *= 0.5f;
	float fx = m_direction == Direction::LEFT ? -fy : fy;
	fx *= 0.5f;
	b2Vec2 force(fx, fy);
	if (direction == Direction::LEFT)
		m_rightFoot->ApplyLinearImpulse(force, m_rightFoot->GetPosition(), true);
	if (direction == Direction::RIGHT)
		m_leftFoot->ApplyLinearImpulse(force, m_leftFoot->GetPosition(), true);
	m_direction = Direction::FALLEN;
}

const b2Vec2& Character::GetPosition()
{
	return m_torso->GetPosition();
}

void Character::CreateTorso(b2World* world, const b2Vec2& pos)
{
	b2BodyDef bd;
	bd.type = b2_dynamicBody;
	bd.position = pos;
	m_torso = world->CreateBody(&bd);

	b2CircleShape circleShape;
	circleShape.m_radius = 1.0f;
	SetFixture(m_torso, &circleShape, k_BodyDensity, k_CollisionCategory);
	m_torso->SetFixedRotation(true);
}

void Character::CreateHead(b2World* world)
{
	b2Vec2 headPos(m_torso->GetPosition().x, m_torso->GetPosition().y + 2.0f);
	b2BodyDef bd;
	bd.type = b2_dynamicBody;
	bd.position = headPos;
	m_head = world->CreateBody(&bd);

	b2CircleShape circleShape;
	circleShape.m_radius = 2.0f;
	SetFixture(m_head, &circleShape, k_headDensity, k_CollisionCategory);

//	SetDistanceJoint(world, m_torso, m_head);
	SetRevoluteJoint(world, m_torso, m_head, -0.1f * b2_pi, 0.1f * b2_pi);
}

void Character::CreateArms(b2World* world)
{
	b2Vec2 rightArmPos(m_torso->GetPosition().x + 2.0f, m_torso->GetPosition().y);
	b2Vec2 leftArmPos(m_torso->GetPosition().x - 2.0f, m_torso->GetPosition().y);
	b2BodyDef bd;
	bd.type = b2_dynamicBody;
	bd.position = rightArmPos;
	m_rightHand = world->CreateBody(&bd);
	bd.position = leftArmPos;
	m_leftHand = world->CreateBody(&bd);

	b2CircleShape circleShape;
	circleShape.m_radius = 0.5f;
	SetFixture(m_rightHand, &circleShape, k_BodyDensity, k_CollisionCategory);
	SetFixture(m_leftHand, &circleShape, k_BodyDensity, k_CollisionCategory);

//	SetDistanceJoint(world, m_torso, m_rightHand);
//	SetDistanceJoint(world, m_torso, m_leftHand);
	SetRevoluteJoint(world, m_torso, m_rightHand, -0.1f * b2_pi, 0.1f * b2_pi);
	SetRevoluteJoint(world, m_torso, m_leftHand, -0.1f * b2_pi, 0.1f * b2_pi);
}

void Character::CreateFeet(b2World* world)
{
	b2Vec2 rightFootPos(m_torso->GetPosition().x + 0.5f, m_torso->GetPosition().y - 1.75f);
	b2Vec2 leftFootPos(m_torso->GetPosition().x - 0.5f, m_torso->GetPosition().y - 1.75f);
	b2BodyDef bd;
	bd.type = b2_dynamicBody;
	bd.position = rightFootPos;
	m_rightFoot = world->CreateBody(&bd);
	bd.position = leftFootPos;
	m_leftFoot = world->CreateBody(&bd);

	b2PolygonShape boxShape;
	boxShape.SetAsBox(1.0f, 0.5f, b2Vec2_zero, 0.0f);
	SetFixture(m_rightFoot, &boxShape, k_BodyDensity, k_CollisionCategory);
	SetFixture(m_leftFoot, &boxShape, k_BodyDensity, k_CollisionCategory);
	m_rightFoot->GetFixtureList()->SetFriction(0.9f);
	m_leftFoot->GetFixtureList()->SetFriction(0.9f);

//	SetDistanceJoint(world, m_torso, m_rightFoot);
//	SetDistanceJoint(world, m_torso, m_leftFoot);
	SetRevoluteJoint(world, m_torso, m_rightFoot, 0.0f, 0.25f * b2_pi);
	SetRevoluteJoint(world, m_torso, m_leftFoot, -0.25f * b2_pi, 0.0f);
}

void Character::SetFixture(b2Body* body, b2Shape* shape, float density, int16 collisionGroup)
{
	b2FixtureDef characterShapeDef;
	characterShapeDef.density = density;
	characterShapeDef.shape = shape;
	characterShapeDef.filter.groupIndex = collisionGroup;
	body->CreateFixture(&characterShapeDef);
}


void Character::SetDistanceJoint(b2World* world, b2Body* bodyA, b2Body* bodyB)
{
	b2DistanceJointDef distJointDef;
	distJointDef.Initialize(bodyA, bodyB, bodyA->GetPosition(), bodyB->GetPosition());
	distJointDef.collideConnected = false;
	distJointDef.frequencyHz = 4.0f;
	distJointDef.dampingRatio = 0.5f;
	b2DistanceJoint* joint = (b2DistanceJoint*)world->CreateJoint(&distJointDef);
}

void Character::SetRevoluteJoint(b2World* world, b2Body* bodyA, b2Body* bodyB, float lower, float upper)
{
	b2RevoluteJointDef rjd;
	rjd.Initialize(bodyA, bodyB, bodyA->GetPosition());
	rjd.enableMotor = false;
	rjd.collideConnected = false;
	rjd.lowerAngle = lower;
	rjd.upperAngle = upper;
	rjd.enableLimit = true;
	b2RevoluteJoint* joint = (b2RevoluteJoint*)world->CreateJoint(&rjd);
}

bool Character::FeetOnGround()
{
	/*
	MyRayCastCallback callback(m_leftFoot);
	b2Vec2 start = m_leftFoot->GetPosition();
	b2Vec2 end = start - b2Vec2(0.0f, 0.3f);
	m_leftFoot->GetWorld()->RayCast(&callback, start, end);
	bool leftOnGround = (callback.m_fixture->GetBody()->GetType() == b2BodyType::b2_staticBody);
	*/
	return BodyTouchesGround(m_rightFoot) && BodyTouchesGround(m_leftFoot);
}


bool Character::BodyTouchesGround(b2Body* body)
{
	for (b2ContactEdge* ce = body->GetContactList(); ce; ce = ce->next)
	{
		b2Contact* c = ce->contact;
		if (!c->IsEnabled()) continue;
		if (c->IsTouching())
		{
			if (c->GetFixtureA()->GetBody()->GetType() == b2BodyType::b2_staticBody
				|| c->GetFixtureB()->GetBody()->GetType() == b2BodyType::b2_staticBody)
			{
				return true;
			}
		}
	}
	return false;
}

float Character::GetMass()
{
	return m_head->GetMass() + m_torso->GetMass() + m_rightHand->GetMass() * 2 + m_rightFoot->GetMass() * 2;
}
