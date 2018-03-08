#pragma once
#include <Box2D/Box2D.h>
#include "GameObject.h"
#include "Bicycle.h"
#include "GameAudio.h"

class Cyclist : public GameObject
{
public:
	enum State
	{
		eRiding,
		eFalling,
		eDead,
		eHit
	};

	Cyclist(b2World* world, Bicycle* cycle, const Physics::Parameters& params, GameAudio* audio);
	void SaveState(Windows::Storage::Streams::DataWriter^ state) const;
	void RestoreState(Windows::Storage::Streams::DataReader^ state);
	const b2Vec2& GetPosition() const { return m_torso->GetPosition(); }
	const b2Vec2& GetSpeed() const { return m_torso->GetLinearVelocity(); }
	float DistanceTravelled() const { return m_distance; }
	float DistanceDelta() const { return m_distanceDelta; }
	float AirDistanceTravelled() const { return m_airDistance; }
	b2Body* GetBody() { return m_torso; }
	virtual void BeginContact(b2Contact* c);
	virtual void EndContact(b2Contact* c);

	void DrawRight(Renderer^ renderBatch) const;
	void DrawLeft(Renderer^ renderBatch) const;

	void Update(float dt);
	bool IsRiding() const { return m_state == eRiding;  }
	bool IsDead() const { return m_state == eDead; }
	bool IsAir() const { return m_airDistance > 1.0f; }

	void Reset();

private:
	void CreateLowerLegGfx(bool isRightLeg);
	void CreateUpperLegGfx(bool isRightLeg);
	void CreateBodyGfx();
	void CreateHeadGfx();
	void CreateHandGfx();

	void ConnectToCycle();
	void DisconnectFromCycle();

	void PlayJahuuSound();
	void PlayAutsSound();

	Bicycle* m_cycle;
	struct Positions {
		Positions(const b2Vec2& torso, const b2Vec2& foot);
		float m_headRadius;
		b2Vec2 m_torso;
		b2Vec2 m_foot;
		b2Vec2 m_neck;
		b2Vec2 m_head;
		b2Vec2 m_knee;
	} m_coords;

	b2Body* m_head;
	b2Body* m_torso;
	b2Body* m_rightUpperLeg;
	b2Body* m_rightLowerLeg;
	b2Body* m_leftUpperLeg;
	b2Body* m_leftLowerLeg;
	b2Body* m_rightHand;
	b2Body* m_leftHand;

	b2DistanceJoint* m_saddleJoint;
	b2RevoluteJoint* m_leftHandleJoint;
	b2RevoluteJoint* m_rightHandleJoint;
	b2RevoluteJoint* m_leftPedalJoint;
	b2RevoluteJoint* m_rightPedalJoint;

	VerticeBatch m_lowerLeftLegGfx;
	VerticeBatch m_upperLeftLegGfx;
	VerticeBatch m_lowerRightLegGfx;
	VerticeBatch m_upperRightLegGfx;
	VerticeBatch m_torsoGfx;
	VerticeBatch m_headGfx;
	VerticeBatch m_handGfx;
	VerticeBatch m_eyeGfx;
	VerticeBatch m_mouthGfx;
	VerticeBatch m_mouthGfx2;
	VerticeBatch* m_mouthGfxPtr;

	State m_state;
	float m_fallTime;
	float m_distance;
	float m_distanceDelta;
	float m_airTime;
	float m_airDistance;
	float m_prevX;
	GameAudio* m_audio;
	float m_previousOutsTime;
	float m_notProgressingTime;

	const float m_maxFallTime;
	const float m_stopped;
};
