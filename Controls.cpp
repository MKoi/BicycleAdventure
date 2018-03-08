#include "pch.h"
#include "Controls.h"
#include "Bicycle.h"



Controls::Controls(Bicycle* cycle): m_highAngle(0.2f*b2_pi), m_lowAngle(-0.1f*b2_pi), 
	m_jumpFactor(0.5f), m_landFactor(2.0f), m_stabilizeFactor(2.5f), m_wheelSpeed(-100.0f),
	m_acceleration(1.0f), m_cycle(cycle), m_pressed(false), m_currentState(NULL),
	m_accelerate(&Controls::Accelerate, "accel"),
	m_roll(&Controls::Roll, "roll"),
	m_brake(&Controls::Brake, "brake"),
	m_jump(&Controls::Jump, "jump"),
	m_lowerFront(&Controls::LowerFront, "lfront"),
	m_lowerRear(&Controls::LowerRear, "lrear"),
	m_lowerFrontLand(&Controls::LowerFront, "lfrontland"),
	m_lowerRearLand(&Controls::LowerRear, "lrearland"),
	m_land(&Controls::Land, "land"),
	m_start(&Controls::Default, "start"),
	m_end(&Controls::Default, "end")
{
	m_start.SetNext(&m_accelerate, [&](){ return m_cycle->OnGround() && !m_pressed; }, 
		[&](){ m_cycle->m_audio->Play(GameAudio::eRolling); m_cycle->m_audio->Play(GameAudio::eWheel); });
	m_start.SetNext(&m_brake, [&](){ return m_cycle->OnGround() && m_pressed; },
		[&](){ m_cycle->m_audio->Play(GameAudio::eRolling); });
	m_start.SetNext(&m_jump, [&](){ return !m_cycle->OnGround() && !m_pressed; }, 
		[&](){ m_cycle->m_audio->Play(GameAudio::eWheel); });
	m_start.SetNext(&m_land, [&](){ return !m_cycle->OnGround() && m_pressed; });

	m_accelerate.SetNext(&m_jump, [&](){ return !m_cycle->OnGround(); }, [&](){ m_cycle->m_audio->Pause(GameAudio::eRolling); });
	m_accelerate.SetNext(&m_brake, [&](){ return m_pressed; }, [&](){ m_cycle->m_audio->Pause(GameAudio::eWheel); m_cycle->PlayBrakeSound(); });
	m_accelerate.SetNext(&m_roll, [&](){ return (!m_cycle->FrontWheelGround() && m_cycle->GetAngle() > m_highAngle);});
	m_accelerate.SetNext(&m_accelerate, [&](){ return (m_cycle->m_rearJoint != nullptr) ? (m_cycle->m_rearJoint->GetMotorSpeed() > m_wheelSpeed) : false; });

	m_roll.SetNext(&m_jump, [&](){ return !m_cycle->OnGround(); }, [&](){ m_cycle->m_audio->Pause(GameAudio::eRolling); });
	m_roll.SetNext(&m_brake, [&](){ return m_pressed; }, [&](){ m_cycle->m_audio->Pause(GameAudio::eWheel); m_cycle->PlayBrakeSound(); });
	m_roll.SetNext(&m_accelerate, [&](){ return (!m_cycle->FrontWheelGround() && m_cycle->GetAngle() <= m_highAngle);});

	m_brake.SetNext(&m_accelerate, [&](){ return !m_pressed; }, [&](){ m_cycle->m_audio->Resume(GameAudio::eWheel); });
	m_brake.SetNext(&m_land, [&](){ return !m_cycle->OnGround(); }, [&](){ m_cycle->m_audio->Pause(GameAudio::eRolling); });

	m_jump.SetNext(&m_land, [&](){ return m_pressed; }, [&](){ m_cycle->m_audio->Pause(GameAudio::eWheel); });
	m_jump.SetNext(&m_accelerate, [&](){ return m_cycle->OnGround(); }, [&](){ m_cycle->m_audio->Resume(GameAudio::eRolling); });
	m_jump.SetNext(&m_lowerFront, [&](){ return m_cycle->GetAngle() > m_highAngle; });
	m_jump.SetNext(&m_lowerRear, [&](){ return m_cycle->GetAngle() < m_lowAngle; });

	m_lowerFront.SetNext(&m_accelerate, [&](){ return m_cycle->OnGround(); }, [&](){ m_cycle->m_audio->Resume(GameAudio::eRolling); });
	m_lowerFront.SetNext(&m_jump, [&](){ return m_cycle->GetAngle() < m_highAngle; });
	m_lowerFront.SetNext(&m_lowerFrontLand, [&](){ return m_pressed; }, [&](){ m_cycle->m_audio->Pause(GameAudio::eWheel); });

	m_lowerFrontLand.SetNext(&m_brake, [&](){ return m_cycle->OnGround(); }, [&](){ m_cycle->m_audio->Resume(GameAudio::eRolling); });
	m_lowerFrontLand.SetNext(&m_land, [&](){ return m_cycle->GetAngle() < m_highAngle; });

	m_lowerRear.SetNext(&m_accelerate, [&](){ return m_cycle->OnGround(); }, [&](){ m_cycle->m_audio->Resume(GameAudio::eRolling); });
	m_lowerRear.SetNext(&m_jump, [&](){ return m_cycle->GetAngle() > m_lowAngle; });
	m_lowerRear.SetNext(&m_lowerRearLand, [&](){ return m_pressed; }, [&](){ m_cycle->m_audio->Pause(GameAudio::eWheel); });

	m_lowerRearLand.SetNext(&m_brake, [&](){ return m_cycle->OnGround(); }, [&](){ m_cycle->m_audio->Resume(GameAudio::eRolling); });
	m_lowerRearLand.SetNext(&m_land, [&](){ return m_cycle->GetAngle() > m_lowAngle; });

	m_land.SetNext(&m_brake, [&](){ return m_cycle->OnGround(); }, [&](){ m_cycle->m_audio->Resume(GameAudio::eRolling); });
	m_land.SetNext(&m_lowerFrontLand, [&](){ return m_cycle->GetAngle() > m_highAngle; });
	m_land.SetNext(&m_lowerRearLand, [&](){ return m_cycle->GetAngle() < m_lowAngle; });

	m_currentState = &m_start;
//	Action = &Controls::Land;
}


void Controls::Disable()
{
	m_currentState = &m_end;
	m_cycle->m_audio->Stop(GameAudio::eRolling);
	m_cycle->m_audio->Stop(GameAudio::eWheel);
	m_currentState->DoAction(this);
}

void Controls::Reset()
{
	m_cycle->ApplyDamping(0.0f);
	m_currentState = &m_start;
	m_currentState->DoAction(this);
}


void Controls::Update(float dt)
{
	if (dt > 0.0f)
	{
		m_currentState = m_currentState->DoNext(this);
	}
}

void Controls::Accelerate()
{
//	OutputDebugString(TEXT("Accelerate\n"));
	//m_cycle->m_frame.m_body->SetFixedRotation(false);
	m_cycle->ApplyDamping(0.0f);
	AdjustWeight(1.0f);
	if (m_cycle->m_rearJoint != nullptr)
	{
		float speed = m_cycle->m_rearJoint->GetMotorSpeed();
		if (speed > m_wheelSpeed)
		{
			m_cycle->SetSpeed(speed - m_acceleration, false);
		}
		else
		{
			m_cycle->SetSpeed(m_wheelSpeed, false);
		}
		PlayWheelSound(speed);

	}
	/*
	char buf[32];
	sprintf_s(buf, "Accelerate: %.2f\n", m_cycle->m_rearJoint->GetMotorSpeed());
	OutputDebugStringA(buf);
	*/
}

void Controls::Roll()
{
	//	m_cycle->m_frontWheel->SetGravityScale(m_stabilizeFactor);
	//	OutputDebugString(TEXT("Roll\n"));
	if (m_cycle->m_rearJoint != nullptr)
	{
		float speed = m_cycle->m_rearJoint->GetMotorSpeed();
		if (speed < 0.0f)
		{
			m_cycle->SetSpeed(speed + m_acceleration, true);
		}
		else
		{
			m_cycle->SetSpeed(0.0f, true);
		}
		PlayWheelSound(speed);
	}
	/*
	char buf[32];
	sprintf_s(buf, "Roll: %.2f\n", m_cycle->m_rearJoint->GetMotorSpeed());
	OutputDebugStringA(buf);
	*/
}

void Controls::Brake()
{
	//OutputDebugString(TEXT("Brake\n"));
	AdjustWeight(1.0f);
	m_cycle->SetSpeed(0.0f, false);
	m_cycle->ApplyDamping(1.0f);
}

void Controls::Jump()
{
	//OutputDebugString(TEXT("Jump\n"));
	AdjustWeight(m_jumpFactor);
	if (m_cycle->m_rearJoint != nullptr)
	{
		m_cycle->SetSpeed(m_cycle->m_rearJoint->GetMotorSpeed(), true);
	}
}

void Controls::LowerFront()
{
//	OutputDebugString(TEXT("LowerFront\n"));
//	m_cycle->m_frame.m_body->SetAngularVelocity(0.0f);
//	float rearWheelWeight = m_cycle->m_rearWheel.m_body->GetGravityScale();
//	m_cycle->m_frontWheel.m_body->SetGravityScale(3 * rearWheelWeight);
	m_cycle->m_frontWheel->SetGravityScale(m_stabilizeFactor);
}

void Controls::LowerRear()
{
//	OutputDebugString(TEXT("LowerRear\n"));
//	m_cycle->m_frame.m_body->SetAngularVelocity(0.0f);
//	float frontWheelWeight = m_cycle->m_frontWheel.m_body->GetGravityScale();
	m_cycle->m_rearWheel->SetGravityScale(m_stabilizeFactor);
}

void Controls::Land()
{
//	OutputDebugString(TEXT("Land\n"));
	m_cycle->ApplyDamping(0.0f);
	AdjustWeight(m_landFactor);
	m_cycle->SetSpeed(0.0f, true);
//	m_cycle->m_frame.m_body->SetFixedRotation(true);
}

void Controls::Default()
{
	AdjustWeight(1.0f);
	m_cycle->SetSpeed(0.0f, true);
}

void Controls::AdjustWeight(float factor)
{
	m_cycle->m_rearWheel->SetGravityScale(factor);
	m_cycle->m_frontWheel->SetGravityScale(factor);
}

void Controls::PlayWheelSound(float speed)
{
	if (m_wheelSpeed != 0.0f)
	{
		float soundpitch = -0.5f + abs(speed / (2.0f * m_wheelSpeed));
		m_cycle->m_audio->SetPitch(GameAudio::eWheel, soundpitch);
		m_cycle->m_audio->SetVolume(GameAudio::eWheel, 0.1f);
	}
}
