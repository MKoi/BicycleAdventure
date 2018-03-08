#pragma once
#include <vector>
#include <algorithm>
#include "SimpleMath.h"

template<typename T> void Linear(const T& v1, const T& v2, T& res, float scale)
{
	res =  v1 + scale * (v2 - v1);
}

template<typename T> void Curve(const T& v1, const T& v2, const T& v3, T& res, float scale)
{
	Linear(v1, v2, res, scale);
}

using std::vector;
using DirectX::XMFLOAT2;
using DirectX::SimpleMath::Vector2;

template<> void Linear<XMFLOAT2>(const XMFLOAT2& v1, const XMFLOAT2& v2, XMFLOAT2& res, float scale);
template<> void Curve<XMFLOAT2>(const XMFLOAT2& v1, const XMFLOAT2& v2, const XMFLOAT2& v3, XMFLOAT2& res, float scale);
template<> void Linear<Vector2>(const Vector2& v1, const Vector2& v2, Vector2& res, float scale);
template<> void Curve<Vector2>(const Vector2& v1, const Vector2& v2, const Vector2& v3, Vector2& res, float scale);
template<> void Linear<vector<XMFLOAT2> >(const vector<XMFLOAT2>& v1, const vector<XMFLOAT2>& v2, vector<XMFLOAT2>& res, float scale);
template<> void Curve<vector<XMFLOAT2> >(const vector<XMFLOAT2>& v1, const vector<XMFLOAT2>& v2, const vector<XMFLOAT2>& v3, vector<XMFLOAT2>& res, float scale);

class AnimatedBase
{
public:
	enum AnimationType
	{
		eCurve,
		eLinear,
		eUndefined
	};
	AnimatedBase() : m_time(0.0f), m_duration(0.0f), Play(false), m_continuous(false) {}
	void SetDuration(float duration) { m_duration = duration; }
	void SetContinuous(bool continuous) { m_continuous = continuous; }

	void Stop()
	{
		Play = false;
		m_time = 0.0f;
	}

	void Update(float dt)
	{
		if (Play)
		{
			m_time += dt;
			while (m_time > m_duration && m_continuous)
			{
				m_time -= m_duration;
			}
		}
	}

	bool Play;
protected:
	float GetProgress() const
	{
		return m_continuous ? GetProgressContinuous() : (std::min)(1.0f, m_time / m_duration);
	}

private:
	float GetProgressContinuous() const
	{
		float progress = 2.0f * m_time / m_duration;
		if (progress > 1.0)
		{
			progress = 2.0f - progress;
		}
		return progress;
	}

	float m_time;
	float m_duration;
	bool m_continuous;
};

template<typename T> class AnimatedValue: public AnimatedBase
{
public:
	AnimatedValue() : AnimatedBase(), m_start(), m_end(), m_mid(), m_current(), m_type(eUndefined) {}
	AnimatedValue(const T& start) : AnimatedBase(), m_start(start), m_end(), m_mid(), m_current(), m_type(eUndefined) {}
	void SetStart(const T& start) { m_start = start; }
	void SetEnd(const T& end)
	{ 
		m_end = end;
		SetType(eLinear);
	}
	void SetMid(const T& mid) 
	{ 
		m_mid = mid;
		SetType(eCurve);
	}
	void SetType(AnimationType type) { m_type = type; }
	bool AtEnd() const { return (GetProgress() >= 1.0f); }

	const T& Get() 
	{
		if (m_type == eUndefined)
		{
			return m_start;
		}
		else if (m_type == eLinear)
		{
			Linear(m_start, m_end, m_current, GetProgress());
		}
		else if (m_type == eCurve)
		{
			Curve(m_start, m_end, m_mid, m_current, GetProgress());
		}
		return m_current;
	}

private:
	T m_start;
	T m_end;
	T m_mid;
	T m_current;
	AnimationType m_type;
};


