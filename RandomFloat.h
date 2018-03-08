#pragma once
#include <random>

class RandomFloat
{
public:
	float Get(float min, float max)
	{
		std::uniform_real_distribution<float> dis(min, max);
		return dis(m_eng);
	}

private:
	std::mt19937 m_eng{ std::random_device{}() };
};