#pragma once
#include "Box2D\Box2D.h"
#include <functional>
#include <vector>
#include "State.h"

class Bicycle;

class Controls
{
public:
	Controls(Bicycle* cycle);
	void ControlPressed(bool pressed) { m_pressed = pressed; }
	void Disable();

	void Reset();
	void Update(float dt);

private:
	float m_highAngle;
	float m_lowAngle;
	float m_jumpFactor;
	float m_landFactor;
	float m_stabilizeFactor;
	float m_wheelSpeed;
	float m_acceleration;

	bool m_pressed;
	Bicycle* m_cycle;
	State<Controls>* m_currentState;

	State<Controls> m_accelerate;
	State<Controls> m_roll;
	State<Controls> m_brake;
	State<Controls> m_jump;
	State<Controls> m_lowerFront;
	State<Controls> m_lowerRear;
	State<Controls> m_lowerFrontLand;
	State<Controls> m_lowerRearLand;
	State<Controls> m_land;
	State<Controls> m_start;
	State<Controls> m_end;

	void Accelerate();
	void Roll();
	void Brake();
	void Jump();
	void LowerFront();
	void LowerRear();
	void Land();
	void Default();

	void AdjustWeight(float factor);
	void PlayWheelSound(float speed);
};
