#pragma once
#include "Box2D\Box2D.h"
#include "DebugDraw.h"

class BicyclePart //: public b2ContactListener
{
public:
	struct Parameters {
		Parameters(): m_category(-1), m_density(1.0f), m_friction(0.9f), m_restitution(0.1f) {}
		int16 m_category;
		float m_density;
		float m_friction;
		float m_restitution;
	};

	BicyclePart(b2World* world, const b2BodyDef& bd);
	virtual void Reset(const b2Vec2& pos);
	virtual void ConnectTo(BicyclePart* other, b2JointDef* jd);
	virtual void Draw(DebugDraw* drawer) const = 0;
	float GetAngle() const;
	const b2Vec2& GetPosition() const { return m_body->GetPosition(); }
	const b2Vec2& GetSpeed() const { return m_body->GetLinearVelocity(); }
	bool TouchingGround() const;
	b2Body* GetBody() const { return m_body; }

	friend class Controls;
protected:
	void GetBoxShape(b2Vec2* points, const b2Vec2& start, const b2Vec2& end, float ratio) const;
	void Init(b2Shape* shape, const Parameters& parameters);
	bool IsGroundContact(b2Contact* c) const;

	b2Body* m_body;

};


class Frame: public BicyclePart
{
public:

	Frame(b2World* world, const b2BodyDef& bd, const Parameters& parameters, const FramePoints& framepoints);
	virtual void Draw(DebugDraw* drawer) const;
	b2Vec2 GetHub() const;
	b2Vec2 GetSaddle() const;
	b2Vec2 GetHandleBar() const;
private:
	FramePoints m_points;
};


class Wheel: public BicyclePart
{
public:
	struct SuspensionParameters {
		SuspensionParameters(): m_hz(4.0), m_zeta(0.7f) {}
		float m_hz;
		float m_zeta;
	};
	Wheel(b2World* world, const b2BodyDef& bd, const Parameters& parameters, float radius = 0.5f);
	virtual void Reset(const b2Vec2& pos);
	virtual void ConnectTo(BicyclePart* other, b2WheelJointDef* jd);
	virtual void Draw(DebugDraw* drawer) const;
	float GetRadius() const { return m_body->GetFixtureList()->GetShape()->m_radius; }
	void SetMotor(float speed, float torque = 20.0f, bool enable = true);
private:
	void GetSectorShape(b2Vec2* vertices, const b2Vec2& center, const b2Vec2& r1, const b2Vec2& r2, 
		float ratio1, float ratio2) const;
	b2WheelJoint* m_fork;
};

class Pedals: public BicyclePart
{
public:
	Pedals(b2World* world, const b2BodyDef& bd, const Parameters& parameters, float radius = 0.15f);
	virtual void ConnectTo(Frame* other, b2RevoluteJointDef* jd);
	void SetMotor(float speed, float torque = 20.0f, bool enable = true);
	virtual void Draw(DebugDraw* drawer) const;
	b2Vec2 GetLeftPedal() const { return m_body->GetWorldPoint(m_leftPedal); }
	b2Vec2 GetRightPedal() const { return m_body->GetWorldPoint(m_rightPedal); }
private:
	b2RevoluteJoint* m_hub;
	b2Vec2 m_leftPedal;
	b2Vec2 m_rightPedal;
};
