#include "pch.h"
#include "Camera.h"


Camera::Camera():
	m_maxdelta(0.5f), m_screenwidth(480), m_screenheight(800), m_x(0.0f), m_y(0.0f), m_width(10.0), m_targetx(m_x),
	m_targety(m_y), m_targetwidth(m_width), m_minwidth(10.0f), m_aspectratio(1.0f),
	m_time(0.0f), m_tracking(NULL)
{

}

void Camera::Reset()
{
	UpdateTracking();
	m_x = m_targetx;
	m_y = m_targety;
	m_width = m_targetwidth;
}

void Camera::SetView(float x, float y, float width, float time)
{
	m_tracking = NULL;
	m_targetx = x;
	m_targety = y;
	m_targetwidth = width;
	m_time = time;
}

void Camera::SetAspectRatio(float ratio)
{
	m_aspectratio = ratio;
}

void Camera::SetScreenDimensions(float height, float width)
{
	m_screenwidth = width;
	m_screenheight = height;
}

void Camera::Track(b2Body* body, float time)
{
	m_tracking = body; 
	m_time = time;
}

void Camera::Update(float dt)
{
	UpdateTracking();
	float ratio = m_time > 0.0f ? min(dt / m_time, 1.0f) : 1.0f;
	float dx = GetDelta(m_targetx, m_x, 0.0f);
	float dy = GetDelta(m_targety, m_y, 0.5f * m_targetwidth);
	float dw = GetDelta(m_targetwidth, m_width, 0.0f);
	//ApplyInertia(dx);
	//ApplyInertia(dy);
	//ApplyInertia(dw);
	m_x += ratio * dx * 0.9f;
	m_y += ratio * dy * 0.9f;
	m_width += ratio * dw * 0.9f;
	//m_width += Clamp(ratio * dw);
	if (m_time > 0.0f)
	{
		m_time -= dt;
	}
}

float Camera::GetWorldX(float screenx) const
{
	float x = screenx - 0.5f * m_screenwidth;
	x /= m_screenwidth;
	x *= m_width;
	x += m_x;
	return x;
}

float Camera::GetWorldY(float screeny) const
{
	float y = 0.5f * m_screenheight - screeny;
	y /= m_screenheight;
	y *= GetHeight();
	y += m_y;
	return y;
}

void Camera::UpdateTracking()
{
	if (m_tracking)
	{
		m_targetx = m_tracking->GetPosition().x + 0.3f*m_tracking->GetLinearVelocity().x;
		m_targety = m_tracking->GetPosition().y + 0.3f*m_tracking->GetLinearVelocity().y;
		m_targetwidth = (m_targetwidth + abs(m_tracking->GetLinearVelocity().x)) / 2;
		if (m_targetwidth < m_minwidth)
		{
			m_targetwidth = m_minwidth;
		}
		//m_time = 0.0f;
	}
}

void Camera::ApplyInertia(float& v)
{
	if (v > 0.0f && v < 1.0f)
	{
		v *= v;
	}
	if (v < 0.0f && v > -1.0f)
	{
		v *= -v;
	}
}

float Camera::Clamp(float v)
{
	if (v > m_maxdelta)
	{
		return m_maxdelta;
	}
	if (v < -m_maxdelta)
	{
		return -m_maxdelta;
	}
	return v;
}

float Camera::GetDelta(float target, float current, float deadzone)
{
	float delta = target - current;
	if (delta < -deadzone)
	{
		delta += deadzone;
	}
	else if (delta > deadzone)
	{
		delta -= deadzone;
	}
	else
	{
		delta = 0.0f;
	}
	return delta;

		/*
	if (target + deadzone < current)
	{
		return target + deadzone - current;
	}
	else if (target - deadzone > current)
	{
		return target - deadzone - current;
	}
	else
	{
		return 0.0f;
	}
	*/
}

void Camera::SaveState(Windows::Storage::Streams::DataWriter^ stream) const
{
	stream->WriteSingle(m_x);
	stream->WriteSingle(m_y);
	stream->WriteSingle(m_width);
	stream->WriteSingle(m_targetx);
	stream->WriteSingle(m_targety);
	stream->WriteSingle(m_targetwidth);
	stream->WriteSingle(m_time);
}

void Camera::RestoreState(Windows::Storage::Streams::DataReader^ stream)
{
	m_x = stream->ReadSingle();
	m_y = stream->ReadSingle();
	m_width = stream->ReadSingle();
	m_targetx = stream->ReadSingle();
	m_targety = stream->ReadSingle();
	m_targetwidth = stream->ReadSingle();
	m_time = stream->ReadSingle();
}

