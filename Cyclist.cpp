#include "pch.h"
#include "Cyclist.h"

using namespace Windows::Storage;

Cyclist::Positions::Positions(const b2Vec2& torso, const b2Vec2& foot)
{
	m_torso = torso;
	m_foot = foot;
	b2Vec2 diff = foot - torso;
	m_neck.Set(foot.x, torso.y - diff.y);
	m_headRadius = 0.25f*(m_neck - m_torso).Length();
	m_head.Set(m_neck.x, m_neck.y + m_headRadius);
	m_knee.Set(torso.x + 0.5f*diff.x - 0.5f*diff.y, torso.y + 0.5f*diff.y + 0.25f*diff.x);
}

Cyclist::Cyclist(b2World* world, Bicycle* cycle, const Physics::Parameters& params, GameAudio* audio) :
	m_cycle(cycle),
	m_coords(cycle->GetSaddlePosition(), cycle->GetHubPosition()),
	m_head(Physics::CreateCircle(world, params, m_coords.m_headRadius, m_coords.m_head)),
	m_torso(Physics::CreateLine(world, params, m_coords.m_neck - m_coords.m_torso, 0.5f, m_coords.m_torso)),
	m_rightUpperLeg(Physics::CreateLine(world, params, m_coords.m_knee - m_coords.m_torso, 0.4f, m_coords.m_torso)),
	m_rightLowerLeg(Physics::CreateLine(world, params, m_cycle->GetRightPedal() - m_coords.m_knee, 0.4f, m_coords.m_knee)),
	m_leftUpperLeg(Physics::CreateLine(world, params, m_coords.m_knee - m_coords.m_torso, 0.4f, m_coords.m_torso)),
	m_leftLowerLeg(Physics::CreateLine(world, params, m_cycle->GetLeftPedal() - m_coords.m_knee, 0.3f, m_coords.m_knee)),
	m_rightHand(Physics::CreateLine(world, params, m_cycle->GetHandleBar() - m_coords.m_neck, 0.15f, m_coords.m_neck)),
	m_leftHand(Physics::CreateLine(world, params, m_cycle->GetHandleBar() - m_coords.m_neck, 0.15f, m_coords.m_neck)),
	m_saddleJoint(NULL),
	m_leftHandleJoint(NULL),
	m_rightHandleJoint(NULL),
	m_leftPedalJoint(NULL),
	m_rightPedalJoint(NULL),
	m_lowerLeftLegGfx(D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST),
	m_upperLeftLegGfx(D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST),
	m_lowerRightLegGfx(D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST),
	m_upperRightLegGfx(D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST),
	m_torsoGfx(D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST),
	m_headGfx(D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST),
	m_handGfx(D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST),
	m_eyeGfx(D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST),
	m_mouthGfx(D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST),
	m_mouthGfx2(D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST),
	m_mouthGfxPtr(&m_mouthGfx),
	m_state(eRiding),
	m_fallTime(0.0f),
	m_distance(0.0f),
	m_distanceDelta(0.0f),
	m_airTime(0.0f),
	m_airDistance(0.0f),
	m_prevX(m_torso->GetPosition().x),
	m_previousOutsTime(10.0f),
	m_notProgressingTime(0.0f),
	m_audio(audio),
	m_maxFallTime(10.0f),
	m_stopped(0.1f)
{
	const float smallAngle = b2_pi/8;
	const float mediumAngle = b2_pi/4;
	Physics::CreateRevoluteJoint(m_torso, m_head, m_coords.m_neck, smallAngle, -smallAngle);
	Physics::CreateRevoluteJoint(m_torso, m_rightHand, m_coords.m_neck, mediumAngle, -mediumAngle);
	

	Physics::CreateRevoluteJoint(m_torso, m_leftHand, m_coords.m_neck, mediumAngle, -mediumAngle);
	

	Physics::CreateRevoluteJoint(m_torso, m_rightUpperLeg, m_coords.m_torso, smallAngle, -smallAngle);
	Physics::CreateRevoluteJoint(m_rightUpperLeg, m_rightLowerLeg, m_coords.m_knee, smallAngle, -mediumAngle);
	

	Physics::CreateRevoluteJoint(m_torso, m_leftUpperLeg, m_coords.m_torso, smallAngle, -smallAngle);
	Physics::CreateRevoluteJoint(m_leftUpperLeg, m_leftLowerLeg, m_coords.m_knee, smallAngle, -mediumAngle);


	ConnectToCycle();

	CreateLowerLegGfx(true);
	CreateLowerLegGfx(false);
	CreateUpperLegGfx(true);
	CreateUpperLegGfx(false);
	CreateBodyGfx();
	CreateHeadGfx();
	CreateHandGfx();

	AddPart(m_head, &m_headGfx);
	AddPart(m_torso, &m_torsoGfx);
	AddPart(m_rightUpperLeg, &m_upperRightLegGfx);
	AddPart(m_rightLowerLeg, &m_lowerRightLegGfx);
	AddPart(m_leftUpperLeg, &m_upperLeftLegGfx);
	AddPart(m_leftLowerLeg, &m_lowerLeftLegGfx);
	AddPart(m_rightHand, &m_handGfx);
	AddPart(m_leftHand, nullptr);
}

void Cyclist::Reset()
{
	m_coords = Positions(m_cycle->GetSaddlePosition(), m_cycle->GetHubPosition());
	Physics::ResetBody(m_head, m_coords.m_head);
	Physics::ResetBody(m_torso, m_coords.m_torso);
	Physics::ResetBody(m_rightUpperLeg, m_coords.m_torso);
	Physics::ResetBody(m_rightLowerLeg, m_coords.m_knee);
	Physics::ResetBody(m_leftUpperLeg, m_coords.m_torso);
	Physics::ResetBody(m_leftLowerLeg, m_coords.m_knee);
	Physics::ResetBody(m_rightHand, m_coords.m_neck);
	Physics::ResetBody(m_leftHand, m_coords.m_neck);
	ConnectToCycle();
	Update(0.0f);
	m_distance = 0.0f;
	m_distanceDelta = 0.0f;
	m_airTime = 0.0f;
	m_airDistance = 0.0f;
	m_fallTime = 0.0f;
	m_notProgressingTime = 0.0f;
	m_prevX = m_torso->GetPosition().x;
}

void Cyclist::Update(float dt)
{
	GameObject::Update(dt);
	float eyeScale = 1.0f;
	m_mouthGfxPtr = nullptr;

	float xPos = m_torso->GetPosition().x;
	m_distanceDelta = 0.0f;
	m_previousOutsTime += dt;
	if (xPos > m_prevX)
	{
		m_notProgressingTime = 0.0f;
		m_distanceDelta = xPos - m_prevX;
		m_distance += m_distanceDelta;
		if (m_airTime > 0.0f)
		{
			const float threshold1 = 30.0f;
			if (m_airDistance < threshold1 && m_airDistance + m_distanceDelta >= threshold1)
			{
				PlayJahuuSound();
			}

			m_airDistance += m_distanceDelta;
		}
		m_prevX = xPos;
	}
	else
	{
		m_notProgressingTime += dt;
	}
	if (m_state == eRiding)
	{
		if (!m_cycle->OnGround())
		{
			m_airTime += dt;
		}
		else
		{
			m_airTime = 0.0f;
			m_airDistance = 0.0f;
		}
		if (m_airTime > 0.5f)
		{
			m_mouthGfxPtr = &m_mouthGfx;
			m_mouthGfxPtr->Update(dt, m_head->GetAngle(), m_head->GetPosition());
		}
		if (m_cycle->IsBroken() || m_notProgressingTime > 20.0f)
		{
			m_state = eHit;
		}

	}
	else if (m_state == eHit)
	{
		DisconnectFromCycle();
		m_airTime = 0.0f;
		m_airDistance = 0.0f;
		m_fallTime += dt;
		if (m_fallTime > m_maxFallTime || GetSpeed().Length() < m_stopped)
		{
			m_state = eDead;

		}
		m_state = eFalling;
	}
	else if (m_state == eFalling)
	{
		m_mouthGfxPtr = &m_mouthGfx2;
		m_mouthGfxPtr->Update(dt, m_head->GetAngle(), m_head->GetPosition());
		eyeScale = 2.0f;
		m_fallTime += dt;
		if (m_fallTime > m_maxFallTime || GetSpeed().Length() < m_stopped)
		{
			m_state = eDead;
		}
	}
	m_eyeGfx.Update(dt, m_head->GetAngle(), m_head->GetPosition(), eyeScale);
	
}

void Cyclist::SaveState(Streams::DataWriter^ state) const
{
	GameObject::SaveState(state);
	state->WriteInt32(static_cast<int>(m_state));
	state->WriteSingle(m_distance);
	state->WriteSingle(m_airTime);
	state->WriteSingle(m_airDistance);
}

void Cyclist::RestoreState(Streams::DataReader^ state)
{
	GameObject::RestoreState(state);
	m_state = static_cast<State>(state->ReadInt32());
	m_distance = state->ReadSingle();
	m_airTime = state->ReadSingle();
	m_airDistance = state->ReadSingle();
	m_prevX = m_torso->GetPosition().x;
	if (m_state == eFalling || m_state == eDead || m_state == eHit)
	{
		DisconnectFromCycle();
	}
}

void Cyclist::BeginContact(b2Contact* c)
{
	if (m_state != eRiding && m_state != eFalling)
	{
		return;
	}
	if (Physics::Touching(c, m_head, Physics::GroundCategory))
	{
		PlayAutsSound();
		m_state = eHit;
	}
}

void Cyclist::EndContact(b2Contact* contact)
{

}

void Cyclist::DrawRight(Renderer^ renderBatch) const
{
	renderBatch->AddToBatch(&m_headGfx);
	renderBatch->AddToBatch(&m_eyeGfx);
	renderBatch->AddToBatch(m_mouthGfxPtr);
	renderBatch->AddToBatch(&m_upperRightLegGfx);
	renderBatch->AddToBatch(&m_lowerRightLegGfx);
	renderBatch->AddToBatch(&m_torsoGfx);
	renderBatch->AddToBatch(&m_handGfx);
}


void Cyclist::DrawLeft(Renderer^ renderBatch) const
{
	renderBatch->AddToBatch(&m_upperLeftLegGfx);
	renderBatch->AddToBatch(&m_lowerLeftLegGfx);
}

void Cyclist::ConnectToCycle()
{
	const float angle = b2_pi / 4;

	Physics::CreateRevoluteJoint(m_rightHandleJoint, m_rightHand, m_cycle->GetFrame(), m_cycle->GetHandleBar(), angle, -angle);
	Physics::CreateRevoluteJoint(m_leftHandleJoint, m_leftHand, m_cycle->GetFrame(), m_cycle->GetHandleBar(), angle, -angle);
	Physics::CreateDistanceJoint(m_saddleJoint, m_torso, m_cycle->GetFrame(), m_coords.m_torso);
	Physics::CreateRevoluteJoint(m_rightPedalJoint, m_rightLowerLeg, m_cycle->GetPedals(), m_cycle->GetRightPedal());
	Physics::CreateRevoluteJoint(m_leftPedalJoint, m_leftLowerLeg, m_cycle->GetPedals(), m_cycle->GetLeftPedal());
	m_state = eRiding;
}

void Cyclist::DisconnectFromCycle()
{
	// already disconnected
	if (m_saddleJoint == NULL)
	{
		return;
	}
	b2World* world = m_torso->GetWorld();
	Physics::DeleteDistanceJoint(m_saddleJoint);
	Physics::DeleteRevoluteJoint(m_rightHandleJoint);
	Physics::DeleteRevoluteJoint(m_leftHandleJoint);
	Physics::DeleteRevoluteJoint(m_rightPedalJoint);
	Physics::DeleteRevoluteJoint(m_leftPedalJoint);
	m_cycle->GetControls().Disable();
}

void Cyclist::CreateLowerLegGfx(bool isRightLeg)
{
	const float widthToLength = isRightLeg ? 0.4f : 0.3f;
	b2Vec2 foot_pos = isRightLeg ? m_cycle->GetRightPedal() : m_cycle->GetLeftPedal();
	b2Vec2 leg = foot_pos - m_coords.m_knee;
	b2Vec2 leg_perp = b2Cross(leg, 0.5f * widthToLength);
	b2Vec2 sock_start = 0.7f*leg;
	b2Vec2 sock_end = 0.9f*leg;
	b2Vec2 shoe_start = sock_end + leg_perp;
	b2Vec2 shoe_end = sock_end - 1.5f * leg_perp;
	b2Color leg_color = isRightLeg ? Graphics::blue : Graphics::blue.Lighter();
	b2Color sock_color = isRightLeg ? Graphics::black : Graphics::black.Lighter();
	b2Color shoe_color = isRightLeg ? Graphics::brown : Graphics::brown.Lighter();
	VerticeBatch& batch = isRightLeg ? m_lowerRightLegGfx : m_lowerLeftLegGfx;
	Graphics::CreateSolidLine(sock_start, sock_end, 1.0f, sock_color, batch);
	Graphics::CreateSolidLineRoundEnd(b2Vec2_zero, sock_start, widthToLength, leg_color, batch, Graphics::eRoundStart);
	Graphics::CreateSolidLine(shoe_start, shoe_end, widthToLength, shoe_color, batch);
	
}

void Cyclist::CreateUpperLegGfx(bool isRightLeg)
{
	const float widthToLength = 0.4f;
	b2Vec2 leg = m_coords.m_knee - m_coords.m_torso;
	VerticeBatch& batch = isRightLeg ? m_upperRightLegGfx : m_upperLeftLegGfx;
	b2Color color = isRightLeg ? Graphics::blue : Graphics::blue.Lighter();
	Graphics::CreateSolidLineRoundEnd(b2Vec2_zero, leg, widthToLength, color, batch, Graphics::eRoundBoth);
}

void Cyclist::CreateBodyGfx()
{
	const float widthToLength = 0.5f;
	b2Vec2 torso = m_coords.m_neck - m_coords.m_torso;
	Graphics::CreateSolidLineRoundEnd(b2Vec2_zero, torso, widthToLength, Graphics::red, m_torsoGfx, Graphics::eRoundEnd);
}

void Cyclist::CreateHeadGfx()
{
	b2Vec2 hair[5];
	hair[0].Set(-m_coords.m_headRadius, 0.0f);
	hair[1].Set(-0.5f * m_coords.m_headRadius, 0.0f);
	hair[2].Set(0.0f, m_coords.m_headRadius);
	hair[3].Set(-0.3f * m_coords.m_headRadius, m_coords.m_headRadius);
	hair[4].Set(-m_coords.m_headRadius, 0.3f * m_coords.m_headRadius);
	Graphics::CreateSolidCircle(b2Vec2_zero, m_coords.m_headRadius, Graphics::skin, m_headGfx);
	Graphics::CreateSolidConvex(hair, 5, Graphics::black, m_headGfx);
	b2Vec2 eyePos(0.5f * m_coords.m_headRadius, 0.3f * m_coords.m_headRadius);
//	b2Vec2 eyePos2 = eyePos + b2Vec2(0.1f * m_coords.m_headRadius, 0.0f);
	// skin.Set(255, 165, 0);
	Graphics::CreateSolidCircle(eyePos, 0.05f * m_coords.m_headRadius, b2Color(250, 160, 0), m_eyeGfx);
//	Graphics::CreateSolidCircle(eyePos2, 0.1f * m_coords.m_headRadius, Graphics::blue , m_eye);
	b2Vec2 mouthPos(0.5f * m_coords.m_headRadius, -0.3f * m_coords.m_headRadius);
	b2Vec2 perimeter(-0.1f * m_coords.m_headRadius, 0.0f);
	Graphics::CreateSolidHalfCircle(mouthPos, perimeter, Graphics::black, m_mouthGfx);
	Graphics::CreateSolidCircle(mouthPos, 0.2f * m_coords.m_headRadius, Graphics::black, m_mouthGfx2);
}

void Cyclist::CreateHandGfx()
{
	const float widthToLength = 0.15f;
	b2Vec2 hand = m_cycle->GetHandleBar() - m_coords.m_neck;
	Graphics::CreateSolidCircle(0.9f*hand, 0.5f*widthToLength * hand.Length(), Graphics::skin, m_handGfx);
	Graphics::CreateSolidCircle(0.9f*hand, 0.25f*widthToLength * hand.Length(), Graphics::black, m_handGfx);
	Graphics::CreateSolidLineRoundEnd(b2Vec2_zero, 0.85f*hand, widthToLength, Graphics::red.Darker(), m_handGfx, Graphics::eRoundStart);
}

void Cyclist::PlayJahuuSound()
{
	int i = (std::rand() % 3);
	switch (i)
	{
	case 0: 
		break;
	case 1: 
		m_audio->Play(GameAudio::eJahuu1); 
		break;
	case 2: 
		m_audio->Play(GameAudio::eJahuu3); 
		break;
	default: 
		break;
	}
}

void Cyclist::PlayAutsSound()
{
	if (m_previousOutsTime < 0.5f)
	{
		return;
	}
	int i = (std::rand() % 3);
	switch (i)
	{
	case 0:
		m_audio->Play(GameAudio::eOuch1);
		break;
	case 1:
		m_audio->Play(GameAudio::eOuch2);
		break;
	case 2:
		m_audio->Play(GameAudio::eOuch3);
		break;
	default:
		break;
	}
	m_previousOutsTime = 0.0f;

}
