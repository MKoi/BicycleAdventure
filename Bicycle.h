#pragma once
#include "Box2D\Box2D.h"
#include "Controls.h"
#include "Physics.h"
//#include "BicyclePart.h"
#include "Graphics.h"
#include "VerticeBatch.h"
#include "GameObject.h"
#include "Particles.h"
#include "GameAudio.h"



class Bicycle : public GameObject
{
public:

	enum BikeType
	{
		e_normal = 0,
		e_race = 1,
		e_dh = 2
	};

	Bicycle(b2World* world, const b2Vec2& pos, const Physics::Parameters& params, GameAudio* audio);

	void Reset(const b2Vec2& pos);
	void StartSound();
	void SaveState(Windows::Storage::Streams::DataWriter^ state) const;
	void RestoreState(Windows::Storage::Streams::DataReader^ state);
	const b2Vec2& GetPosition() const { return m_frame->GetPosition(); }
	b2Vec2 GetSaddlePosition() const { return m_frame->GetWorldPoint(m_vertices[e_saddle]); }
	b2Vec2 GetHubPosition() const { return m_frame->GetWorldPoint(m_vertices[e_hub]); }
	b2Vec2 GetHandleBar() const { return m_frame->GetWorldPoint(m_vertices[e_handle]); }
	
	b2Vec2 GetLeftPedal() const { return m_pedals->GetWorldPoint(-m_pedalOffset); }
	b2Vec2 GetRightPedal() const { return m_pedals->GetWorldPoint(m_pedalOffset); }

	b2Body* GetPedals() const { return m_pedals; }
	b2Body* GetFrame() const { return m_frame; }
	float GetWidth() const { return m_fWheelCenter.x - m_rWheelCenter.x + 2 * m_wheelRadius; }
	const b2Vec2& GetSpeed() const { return m_frame->GetLinearVelocity(); }
	float GetAngle() const { return DirectX::XMScalarModAngle(m_frame->GetAngle());  }
	bool OnGround() const { return FrontWheelGround() || RearWheelGround(); }
	bool FrontWheelGround() const { return m_frontWheelContacts > 0; }
	bool RearWheelGround() const { return m_rearWheelContacts > 0; }
	bool IsBroken() const { return m_frontJoint == nullptr || m_rearJoint == nullptr; }
	Controls& GetControls() { return m_controls; }
	
	virtual void BeginContact(b2Contact* c);
	virtual void EndContact(b2Contact* c);

	void SetSpeed(float speed, bool rollFree);
	void Draw(Renderer^ renderBatch) const;
	//void DrawDebugData(Graphics* draw) const;

	void Update(float dt);

	friend class Controls;
private:
	void CreateFrameGfx();
	void CreateWheelGfx();
	void CreatePedalsGfx();
	void CreateSpokeGfx(float radius, int spokeCount, float thicknessRatio, b2Color color, VerticeBatch& target);
	void EmitDirt();
	void PlayPedalSound();
	void PlayWheelSound();
	void PlayBrakeSound();
	void CheckJointForces(float dt);
	void CreateWheelJoints();
	void ApplyDamping(float damping = 0.0f);

	enum 
	{
		e_hub = 0,
		e_handle = 1,
		e_saddle = 2,
		e_size = 3
	};
	float m_wheelRadius;
	float m_hubRadius;
	Controls m_controls;
	b2Vec2 m_vertices[e_size];
	b2Vec2 m_fWheelCenter;
	b2Vec2 m_rWheelCenter;
	b2Vec2 m_pedalOffset;
	
	b2Body* m_frame;
	b2Body* m_frontWheel;
	b2Body* m_rearWheel;
	b2Body* m_pedals;

	b2WheelJoint* m_frontJoint;
	b2WheelJoint* m_rearJoint;
	b2RevoluteJoint* m_pedalJoint;

	VerticeBatch m_frameGfx;
	VerticeBatch m_fWheelGfx;
	VerticeBatch m_rWheelGfx;
	VerticeBatch m_pedalsGfx;

	Particles m_dirt;
	int m_rearWheelContacts;
	int m_frontWheelContacts;
	float m_pedalAngle;
	GameAudio* m_audio;
};
