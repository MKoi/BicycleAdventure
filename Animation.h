#pragma once
#include "SimpleMath.h"
#include "AnimatedValue.h"


using std::vector;
using DirectX::XMFLOAT2;

class Animation
{
public:
	Animation(const vector<XMFLOAT2>& v);
	Animation();

	static void Set(AnimatedValue<XMFLOAT2>& scale, XMFLOAT2 min, XMFLOAT2 max, bool continuous, float duration);
	static void Reset(AnimatedValue<XMFLOAT2>& scale, XMFLOAT2 min, XMFLOAT2 max, bool continuous, float duration);

	void Update(float dt);
	void Play();
	void Play(float duration, bool continuous);
	void Pause();
	void Stop();
	void SetRotation(float min, float max);
	void SetScale(float xend, float yend);
	void SetScale(float xmin, float xmax, float ymin, float ymax);
	void SetBend(float min, float max);
	void SetMove(XMFLOAT2 start, XMFLOAT2 end);

	AnimatedValue<DirectX::SimpleMath::Vector2> Position;
	AnimatedValue<std::vector<DirectX::XMFLOAT2> > Vertices;
	AnimatedValue<float> Angle;
	AnimatedValue<float> Xscale;
	AnimatedValue<float> Yscale;

private:
	void InitAnimations();
	float FarthestFromOrigin();
	std::vector<AnimatedBase*> m_animations;
};
