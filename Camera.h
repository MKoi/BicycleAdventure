#pragma once
#include <Cyclist.h>

class Camera
{
public:
	Camera();
	void SetAspectRatio(float ratio);
	void SetScreenDimensions(float height, float width);
	void SetView(float x, float y, float width, float time = 0.0f);
	void Update(float dt);
	void Reset();
	void Track(b2Body* body, float time = 0.0f);
	float GetX() const { return m_x; }
	float GetWorldX(float screenx) const; 
	float GetY() const { return m_y; }
	float GetWorldY(float screeny) const;
	float GetHeight(bool inPixels = false) const { return inPixels ? m_screenheight : m_aspectratio * m_width; }
	float GetWidth(bool inPixels = false) const { return inPixels ? m_screenwidth : m_width; }
	float GetScale() const { return m_width / m_minwidth; }
	float GetMinWidth() const { return m_minwidth; }
	
	void SaveState(Windows::Storage::Streams::DataWriter^ state) const;
	void RestoreState(Windows::Storage::Streams::DataReader^ state);


private:
	void UpdateTracking();
	void ApplyInertia(float& v);
	float GetDelta(float target, float current, float deadzone);
	float Clamp(float v);
	const float m_maxdelta;
	float m_screenwidth;
	float m_screenheight;
	float m_x;
	float m_y;
	float m_width;
	float m_targetx;
	float m_targety;
	float m_targetwidth;
	const float m_minwidth;
	float m_aspectratio;
	float m_time;
	b2Body* m_tracking;
};
