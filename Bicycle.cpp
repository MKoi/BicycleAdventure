#include "pch.h"
#include "Bicycle.h"
#include "GameArea.h"
#include <vector>

using namespace DirectX;
using namespace std;
using namespace Windows::Storage;

Bicycle::Bicycle(b2World* world, const b2Vec2& pos, const Physics::Parameters& params, GameAudio* audio) :
	m_wheelRadius(0.5f), m_hubRadius(m_wheelRadius/3.0f), m_controls(this),
	m_fWheelCenter(2 * m_wheelRadius, 0.0f), m_rWheelCenter(-2 * m_wheelRadius, 0.0f), m_pedalOffset(m_hubRadius, 0.0f),
	m_frame(nullptr), m_frontWheel(nullptr), m_rearWheel(nullptr), m_pedals(nullptr),
	m_frontJoint(nullptr), m_rearJoint(nullptr), m_pedalJoint(nullptr),
	m_frameGfx(D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST),
	m_fWheelGfx(D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST),
	m_rWheelGfx(D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST),
	m_pedalsGfx(D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST),
	m_dirt(world),
	m_rearWheelContacts(0),
	m_frontWheelContacts(0),
	m_pedalAngle(0.0f),
	m_audio(audio)
{
	b2Vec2 framePos = pos + b2Vec2(2 * m_wheelRadius, m_wheelRadius);
	m_vertices[e_hub].Set(-0.2f, 0.0f);
	m_vertices[e_handle].Set(0.8f, 1.0f);
	m_vertices[e_saddle].Set(-0.5f, 1.0f);
//	m_fWheelCenter.Set(2*m_wheelRadius, 0.0f);
//	m_rWheelCenter.Set(-2*m_wheelRadius, 0.0f);
//	m_pedalOffset.Set(m_hubRadius, 0.0f);
	
	m_frame = Physics::CreatePolygon(world, params, m_vertices, e_size, framePos);
	m_frontWheel = Physics::CreateCircle(world, params, m_wheelRadius, m_frame->GetWorldPoint(m_fWheelCenter));
	m_rearWheel = Physics::CreateCircle(world, params, m_wheelRadius, m_frame->GetWorldPoint(m_rWheelCenter));
	m_pedals = Physics::CreateCircle(world, params, m_hubRadius, m_frame->GetWorldPoint(m_vertices[e_hub]));
	
	CreateWheelJoints();

	m_pedalJoint = Physics::CreateRevoluteJoint(m_frame, m_pedals, m_pedals->GetPosition());
	CreateFrameGfx();
	CreateWheelGfx();
	CreatePedalsGfx();

	AddPart(m_frontWheel, &m_fWheelGfx);
	AddPart(m_rearWheel, &m_rWheelGfx);
	AddPart(m_frame, &m_frameGfx);
	AddPart(m_pedals, &m_pedalsGfx);

	m_pedalAngle = m_pedals->GetAngle();
}


void Bicycle::Reset(const b2Vec2& pos)
{
	b2Vec2 framePos = pos + b2Vec2(2 * m_wheelRadius, m_wheelRadius);
	Physics::ResetBody(m_frame, framePos);
	Physics::ResetBody(m_frontWheel, m_frame->GetWorldPoint(m_fWheelCenter));
	Physics::ResetBody(m_rearWheel, m_frame->GetWorldPoint(m_rWheelCenter));
	Physics::ResetBody(m_pedals, m_frame->GetWorldPoint(m_vertices[e_hub]));
	CreateWheelJoints();
	m_controls.Reset();
	m_dirt.Clear();
	Update(0.0f);
	m_rearWheelContacts = 0;
	m_frontWheelContacts = 0;
}

void Bicycle::StartSound()
{
	if (OnGround())
	{
		m_audio->Play(GameAudio::eRolling);
	}
}


void Bicycle::SaveState(Streams::DataWriter^ state) const
{
	GameObject::SaveState(state);
	state->WriteInt32(m_rearWheelContacts);
	state->WriteInt32(m_frontWheelContacts);
}


void Bicycle::RestoreState(Streams::DataReader^ state)
{
	GameObject::RestoreState(state);
	m_rearWheelContacts = state->ReadInt32();
	m_frontWheelContacts = state->ReadInt32();
}

/*
void Bicycle::DrawDebugData(Graphics* draw) const
{
	if (!RearWheelGround())
		draw->DrawSolidCircle(m_rearWheel->GetPosition(), m_wheelRadius*1.2f, b2Vec2_zero, Graphics::red);
	if (!FrontWheelGround())
		draw->DrawSolidCircle(m_frontWheel->GetPosition(), m_wheelRadius*1.2f, b2Vec2_zero, Graphics::red);
}
*/


void Bicycle::Draw(Renderer^ renderBatch) const
{
	m_dirt.Draw(renderBatch);
	GameObject::Draw(renderBatch);
}

void Bicycle::Update(float dt)
{
	m_controls.Update(dt);
	GameObject::Update(dt);
	CheckJointForces(dt);
	m_dirt.Update(dt);
	EmitDirt();
	PlayPedalSound();
	PlayWheelSound();
}


void Bicycle::BeginContact(b2Contact* c)
{
	if (Physics::Touching(c, m_rearWheel, Physics::GroundCategory))
	{
		m_rearWheelContacts++;
	}
	else if (Physics::Touching(c, m_rearWheel, Physics::WallCategory))
	{
		m_rearWheelContacts++;
	}
	else if (Physics::Touching(c, m_frontWheel, Physics::GroundCategory))
	{
		m_frontWheelContacts++;
	}
	else if (Physics::Touching(c, m_frontWheel, Physics::WallCategory))
	{
		m_frontWheelContacts++;
	}
}

void Bicycle::EndContact(b2Contact* c)
{
	if (Physics::Touching(c, m_rearWheel, Physics::GroundCategory))
	{
		m_rearWheelContacts--;
	}
	else if (Physics::Touching(c, m_rearWheel, Physics::WallCategory))
	{
		m_rearWheelContacts--;
	}
	else if (Physics::Touching(c, m_frontWheel, Physics::GroundCategory))
	{
		m_frontWheelContacts--;
	}
	else if (Physics::Touching(c, m_frontWheel, Physics::WallCategory))
	{
		m_frontWheelContacts--;
	}
}

void Bicycle::SetSpeed(float speed, bool rollFree)
{
	const float pedalToWheelRatio = 5.0f;
	const float maxTorque = 20.0f;
	
	if (m_rearJoint != nullptr)
	{
		Physics::SetMotor(m_rearJoint, speed, maxTorque, !rollFree);
	}
	Physics::SetMotor(m_pedalJoint, speed/pedalToWheelRatio, maxTorque, true);
}

void Bicycle::CreateFrameGfx()
{
	const float ratio = 0.06f;
	Graphics::CreateSolidLine(m_vertices[e_hub], m_vertices[e_handle], ratio, Graphics::red, m_frameGfx);
	Graphics::CreateSolidLine(m_vertices[e_handle], m_vertices[e_saddle], ratio, Graphics::red, m_frameGfx);
	Graphics::CreateSolidLine(m_vertices[e_saddle], m_vertices[e_hub], ratio, Graphics::red, m_frameGfx);
	Graphics::CreateSolidLine(m_vertices[e_saddle], m_rWheelCenter, ratio, Graphics::red, m_frameGfx);
	Graphics::CreateSolidLine(m_vertices[e_hub], m_rWheelCenter, ratio, Graphics::red, m_frameGfx);
	Graphics::CreateSolidLine(m_vertices[e_handle], m_fWheelCenter, ratio, Graphics::red, m_frameGfx);
}


void Bicycle::CreateWheelGfx()
{
	const int spokeCount = 7;
	const float thicknessRatio = 0.02f;
	const float inner_r1 = 0.15f;
	const float inner_r2 = 0.2f;
	const float outer_r1 = 0.85f;
	const float outer_r2 = 0.9f;
	CreateSpokeGfx(outer_r1*m_wheelRadius, spokeCount, thicknessRatio, Graphics::black, m_fWheelGfx);
	//Graphics::CreateSolidCircle(b2Vec2_zero, m_wheelRadius * inner_r2, Graphics::black, m_wheelGfx);
	Graphics::CreateSolidCircle(b2Vec2_zero, m_wheelRadius * inner_r2, Graphics::grey, m_fWheelGfx);
	//Graphics::CreateSphere(b2Vec2_zero, m_wheelRadius * outer_r2, outer_r1/outer_r2, Graphics::grey, m_wheelGfx);
	Graphics::CreateSphere(b2Vec2_zero, m_wheelRadius, outer_r2, Graphics::black, m_fWheelGfx);
	m_rWheelGfx = m_fWheelGfx;
}


void Bicycle::CreatePedalsGfx()
{
	const int spokeCount = 5;
	const float spokeThickness = 0.2f;
	const float pedalThickness = 0.2f;
	const float inner_r = 0.3f;
	const float outer_r = 0.6f;
	CreateSpokeGfx(outer_r*m_hubRadius, spokeCount, spokeThickness, Graphics::grey, m_pedalsGfx);
	Graphics::CreateSolidCircle(b2Vec2_zero, inner_r*m_hubRadius, Graphics::grey, m_pedalsGfx);
	Graphics::CreateSphere(b2Vec2_zero, m_hubRadius, outer_r, Graphics::grey, m_pedalsGfx);
	Graphics::CreateSolidLine(b2Vec2_zero, m_pedalOffset, pedalThickness, Graphics::darkGrey, m_pedalsGfx);
}

void Bicycle::CreateSpokeGfx(float radius, int spokeCount, float thicknessRatio, b2Color color, VerticeBatch& target)
{
	XMMATRIX xf = Graphics::CircleTransform(b2Vec2_zero, spokeCount);
	XMVECTOR point = XMVectorSet(radius, 0.0f, 0.0f, 0.0f);
	b2Vec2 point2d;
	for (int i = 0; i < spokeCount; ++i)
	{
		point2d.Set(XMVectorGetX(point), XMVectorGetY(point));
		Graphics::CreateSolidLine(b2Vec2_zero, point2d, thicknessRatio, Graphics::grey, target);
		point = XMVector2Transform(point, xf);
	}

}

void Bicycle::EmitDirt()
{
	const unsigned int interval = 10;
	static unsigned int count = 0;
	b2Vec2 point;
	b2Vec2 normal;

	if (RearWheelGround())
	{
		Physics::TouchingGround(m_rearWheel, point, normal);
		float angular = -m_rearWheel->GetAngularVelocity() * m_wheelRadius;
		b2Vec2 linear = m_rearWheel->GetLinearVelocity();
		float diff = abs(angular - linear.Length());
		bool lockedBreaks = angular < 1.0f  && linear.Length() > 5.0f;
		bool spinningOnSpot = angular > 5.0f && linear.Length() < 2.0f;
		++count;
		if (lockedBreaks || spinningOnSpot)
		{
			m_dirt.Emit(1, point - 0.2f * normal, -0.0001f * diff * normal);
			if (count % interval == 0)
			{
				m_audio->Play(GameAudio::eGravel, 0.7f);
				count = 0;
			}
#if 0
			char buf[64];
			sprintf_s(buf, "dirt %d\n", (int)diff);
			OutputDebugStringA(buf);
#endif
		}
	}
}

void Bicycle::PlayPedalSound()
{
	float newAngle = m_pedals->GetAngle();
	float a1 = DirectX::XMScalarModAngle(m_pedalAngle);
	float a2 = DirectX::XMScalarModAngle(newAngle);
	if (a1 > 0.0f && a2 < 0.0f)
	{
		m_audio->Play(GameAudio::ePedalUp, 0.7f);
	}
	if (a1 < 0.0f && a2 > 0.0f)
	{
		m_audio->Play(GameAudio::ePedalDown, 0.7f);
	}
	m_pedalAngle = newAngle;
}

void Bicycle::PlayWheelSound()
{
	float v1 = (m_rearJoint && m_rearWheelContacts) ? abs(m_rearWheel->GetAngularVelocity()) : 0.0f;
	float v2 = (m_frontJoint && m_frontWheelContacts) ? abs(m_rearWheel->GetAngularVelocity()) : 0.0f;
	float v = max(v1, v2);
	float vol = 0.01f * v;
	m_audio->SetVolume(GameAudio::eRolling, vol);

}

void Bicycle::PlayBrakeSound()
{
	int ipos = static_cast<int>(GetPosition().x);
	float vel = GetSpeed().Length();
	float vol = min(1.0f, vel / 40.0f);
	m_audio->Play((vol >= 1.0) ? GameAudio::eBrakeLong : GameAudio::eBrakeShort, vol);

}

void Bicycle::CheckJointForces(float dt)
{
	if (dt == 0.0f)
	{
		return;
	}
	const float maxForceSq = 1000000;
	if (m_rearJoint != nullptr)
	{
		b2Vec2 reactionForce = m_rearJoint->GetReactionForce(1.0f / dt);
		float forceModuleSq = reactionForce.LengthSquared();
		if (forceModuleSq > maxForceSq)
		{
			m_frame->GetWorld()->DestroyJoint(m_rearJoint);
			m_rearJoint = nullptr;
		}
	}
	if (m_frontJoint != nullptr)
	{
		b2Vec2 reactionForce = m_frontJoint->GetReactionForce(1.0f / dt);
		float forceModuleSq = reactionForce.LengthSquared();
		if (forceModuleSq > maxForceSq)
		{
			m_frame->GetWorld()->DestroyJoint(m_frontJoint);
			m_frontJoint = nullptr;
		}
	}
}

void Bicycle::CreateWheelJoints()
{
	b2Vec2 axis = m_fWheelCenter - m_vertices[e_handle];
	if (m_frontJoint == nullptr)
	{
		m_frontJoint = Physics::CreateWheelJoint(m_frame, m_frontWheel, m_frontWheel->GetPosition(), axis);
	}
	axis = m_rWheelCenter - m_vertices[e_saddle];
	if (m_rearJoint == nullptr)
	{
		m_rearJoint = Physics::CreateWheelJoint(m_frame, m_rearWheel, m_rearWheel->GetPosition(), axis);
	}
}

void Bicycle::ApplyDamping(float damping)
{
	m_frame->SetLinearDamping(damping);
	m_frontWheel->SetLinearDamping(damping);
	m_rearWheel->SetLinearDamping(damping);
	m_pedals->SetLinearDamping(damping);
}