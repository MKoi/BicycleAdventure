#include "pch.h"
#include "Animation.h"

using namespace DirectX;
using namespace std;

Animation::Animation(const vector<XMFLOAT2>& v) : Position(), Vertices(v), Angle(0.0f), Xscale(1.0f), Yscale(1.0f)
{
	InitAnimations();
}

Animation::Animation() : Position(), Vertices(), Angle(0.0f), Xscale(1.0f), Yscale(1.0f)
{
	InitAnimations();
}

void Animation::Set(AnimatedValue<XMFLOAT2>& scale, XMFLOAT2 min, XMFLOAT2 max, bool continuous, float duration)
{
	scale.SetStart(min);
	scale.SetEnd(max);
	scale.SetDuration(duration);
	scale.SetContinuous(continuous);
}

void Animation::Reset(AnimatedValue<XMFLOAT2>& scale, XMFLOAT2 min, XMFLOAT2 max, bool continuous, float duration)
{
	scale.Stop();
	Set(scale, min, max, continuous, duration);
	scale.Play = true;
}

void Animation::Update(float dt)
{
	for (auto i : m_animations)
	{
		i->Update(dt);
	}
}

void Animation::Play()
{
	for (auto i : m_animations)
	{
		i->Play = true;
	}
}

void Animation::Play(float duration, bool continuous)
{
	for (auto i : m_animations)
	{
		i->SetDuration(duration);
		i->SetContinuous(continuous);
		i->Play = true;
	}
}

void Animation::Pause()
{
	for (auto i : m_animations)
	{
		i->Play = false;
	}
}

void Animation::Stop()
{
	for (auto i : m_animations)
	{
		i->Stop();
	}
}

void Animation::SetRotation(float min, float max)
{
	Angle.SetStart(min);
	Angle.SetEnd(max);
}

void Animation::SetScale(float xend, float yend)
{
	SetScale(1.0f, xend, 1.0f, yend);
}

void Animation::SetScale(float xmin, float xmax, float ymin, float ymax)
{
	Xscale.SetStart(xmin);
	Xscale.SetEnd(xmax);
	Yscale.SetStart(ymin);
	Yscale.SetEnd(ymax);
}

float bisectAngle(float a1, float a2)
{
	float mina = min(a1, a2);
	float maxa = max(a1, a2);
	return mina + 0.5f * (maxa - mina);
}

void Animation::SetBend(float min, float max)
{
	vector<XMFLOAT2> minvec;
	vector<XMFLOAT2> maxvec;
	vector<XMFLOAT2> midvec;
	float maxLength = FarthestFromOrigin();
	XMVECTOR lengthReciprocal = XMVectorReciprocalEst(XMVectorReplicate(maxLength));
	XMVECTOR rotMin = XMQuaternionRotationRollPitchYaw(0.0f, 0.0f, min);
	XMVECTOR rotMax = XMQuaternionRotationRollPitchYaw(0.0f, 0.0f, max);
	XMVECTOR rotMid = XMQuaternionRotationRollPitchYaw(0.0f, 0.0f, bisectAngle(min, max));
	for (auto v : Vertices.Get())
	{
		minvec.emplace_back();
		maxvec.emplace_back();
		midvec.emplace_back();
		XMVECTOR rotatedMin = XMVector3Rotate(XMLoadFloat2(&v), rotMin);
		XMVECTOR rotatedMax = XMVector3Rotate(XMLoadFloat2(&v), rotMax);
		XMVECTOR rotatedMid = XMVector3Rotate(XMLoadFloat2(&v), rotMid);
		XMVECTOR translatedMin = XMVectorLerpV(rotatedMid, rotatedMin, XMVectorMultiply(XMVector2Length(rotatedMid), lengthReciprocal));
		XMVECTOR translatedMax = XMVectorLerpV(rotatedMid, rotatedMax, XMVectorMultiply(XMVector2Length(rotatedMid), lengthReciprocal));
		XMStoreFloat2(&minvec.back(), XMVector3Rotate(translatedMin, XMQuaternionInverse(rotMin)));
		XMStoreFloat2(&maxvec.back(), XMVector3Rotate(translatedMax, XMQuaternionInverse(rotMax)));
		//XMStoreFloat2(&midvec.back(), rotatedMid);
	}
	Vertices.SetStart(minvec);
	Vertices.SetEnd(maxvec);
	//Vertices.SetMid(midvec);
	Angle.SetStart(min);
	Angle.SetEnd(max);
//	Vertices.SetType(AnimatedBase::AnimationType::eCurve);
}

void Animation::SetMove(XMFLOAT2 start, XMFLOAT2 end)
{
	Position.SetStart(Vector2(start.x, start.y));
	Position.SetEnd(Vector2(end.x, end.y));
}

void Animation::InitAnimations()
{
	m_animations.clear();
	m_animations.push_back(&Position);
	m_animations.push_back(&Vertices);
	m_animations.push_back(&Angle);
	m_animations.push_back(&Xscale);
	m_animations.push_back(&Yscale);
}


float Animation::FarthestFromOrigin()
{
	XMVECTOR max = XMVectorZero();
	for (auto v : Vertices.Get())
	{
		XMVECTOR dist = XMLoadFloat2(&v);
		max = XMVectorMax(max, XMVector2Length(dist));
	}
	return XMVectorGetX(max);	
}