#pragma once
#include "Box2D\Box2D.h"
#include "SimpleMath.h"

class Physics
{
public:
	static const int16 GroundCategory = 1;
	static const int16 SensorCategory = 2;
	static const int16 WaterCategory = 4;
	static const int16 WallCategory = 8;
	static const int16 PlayerCategory = -1;
	static const int16 ParticleCategory = -2;

	struct Parameters {
		b2BodyType m_bodyType;
		float m_density;
		float m_friction;
		float m_restitution;
		int16 m_category;
	};
	static const Parameters DefaultGroundParameters;

//	Physics(b2World* world): m_world(world) {}
	static b2Fixture* Touching(b2Contact* c, b2Body* body, int16 category = Physics::GroundCategory);
	static b2Vec2 ContactVelocity(b2Contact* c, b2Body* body);
	static void EvaluateContact(b2Contact* c);
	static bool Touching(b2Body* body, int16 category = Physics::GroundCategory, bool debug = false);
	static bool Touching(b2Body* body, b2Vec2& point, b2Vec2& normal, int16 category = Physics::GroundCategory, bool debug = false);
	static bool TouchingGround(b2Body* body, bool debug = false);
	static bool TouchingGround(b2Body* body, b2Vec2& point, b2Vec2& normal, bool debug = false);
	static void ResetBody(b2Body* body, const b2Vec2& pos);
	static void ResetBody(b2Body* body, const b2Vec2& pos, float angle, const b2Vec2& vel, float avel);
	static void Serialize(b2Body* body, Windows::Storage::Streams::DataWriter^ stream);
	static void Serialize(const b2Vec2& v, Windows::Storage::Streams::DataWriter^ stream);
	static void DeSerialize(b2Body* body, Windows::Storage::Streams::DataReader^ stream);
	static void DeSerialize(b2Vec2& v, Windows::Storage::Streams::DataReader^ stream);
	static void SetMotor(b2WheelJoint* joint, float speed, float torque, bool enabled);
	static void SetMotor(b2RevoluteJoint* joint, float speed, float torque, bool enabled);
//	b2Body* CreateCircle(const Parameters& params, float radius, const b2Vec2& pos);
	static b2Body* CreateLine(b2World* world, const Parameters& params, const b2Vec2& start, const b2Vec2& end, float ratio, const b2Vec2& pos);
	static b2Body* CreateLineFromCorners(b2World* world, const Parameters& params, const b2Vec2& start, const b2Vec2& end, float ratio, const b2Vec2& pos);
	static b2Body* CreateLine(b2World* world, const Parameters& params, const b2Vec2& end, float ratio, const b2Vec2& pos);
//	b2Body* CreatePolygon(const Parameters& params, const b2Vec2* points, int pointcount, const b2Vec2& pos);
//	b2Body* CreateChain(const Parameters& params, const b2Vec2* points, int pointcount, const b2Vec2& pos);
	static b2Body* CreateCircle(b2World* world, const Parameters& params, float radius, const b2Vec2& pos);
	static b2Body* CreateHalfCircle(b2World* world, const Parameters& params, const b2Vec2& perimeter, const b2Vec2& pos);
	static b2Body* CreateChain(b2World* world, const Parameters& params, const b2Vec2* points, int pointcount, const b2Vec2& pos);
	static b2Body* CreatePolygon(b2World* world, const Parameters& params, const b2Vec2* points, int pointcount, const b2Vec2& pos);
	static void AddPolygon(b2Body* body, const Parameters& params, const b2Vec2* points, int pointcount);
	static b2Body* CreateBoxAxisAligned(b2World* world, const Parameters& params, const b2Vec2& topLeft, const b2Vec2& bottomRight, const b2Vec2& pos);
	static b2Body* CreateSensor(b2World* world, const Parameters& params, float radius, const b2Vec2& pos);
	static b2Body* CreateSensor(b2World* world, const Parameters& params, const b2Vec2& topLeft, const b2Vec2& bottomRight, const b2Vec2& pos);
	static b2WheelJoint* CreateWheelJoint(b2Body* A, b2Body* B, const b2Vec2& pos, const b2Vec2& axis);
	static b2RevoluteJoint* CreateRevoluteJoint(b2Body* A, b2Body* B, const b2Vec2& pos);
	static void CreateRevoluteJoint(b2RevoluteJoint*& joint, b2Body* A, b2Body* B, const b2Vec2& pos);
	static b2RevoluteJoint* CreateRevoluteJoint(b2Body* A, b2Body* B, const b2Vec2& pos, float hAngle, float lAngle);
	static void CreateRevoluteJoint(b2RevoluteJoint*& joint, b2Body* A, b2Body* B, const b2Vec2& pos, float hAngle, float lAngle);
	static b2DistanceJoint* CreateDistanceJoint(b2Body* A, b2Body* B, const b2Vec2& pos);
	static void CreateDistanceJoint(b2DistanceJoint*& joint, b2Body* A, b2Body* B, const b2Vec2& pos);
	static void DeleteDistanceJoint(b2DistanceJoint*& joint);
	static void DeleteRevoluteJoint(b2RevoluteJoint*& joint);
	static float HorizontalAngle(const b2Vec2& p1, const b2Vec2& p2);
	static float HorizontalAngle(const DirectX::SimpleMath::Vector2& p1, const DirectX::SimpleMath::Vector2& p2);
private:
	static void DeleteJoint(b2Joint* joint);
//	b2Body* CreateBody(const Parameters& params, b2Shape* shape, const b2Vec2& pos);
	static b2Body* CreateBody(b2World* world, const Parameters& params, b2Shape* shape, const b2Vec2& pos, bool isSensor = false);
	static void CreateFixture(b2Body* body, const Parameters& params, b2Shape* shape, bool isSensor = false);
	static void CollisionDebug(b2Contact* c);
	static void CollisionCategoryToString(char*& name, int16 cat);

//	b2World* m_world;
};
