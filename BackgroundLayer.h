#include <vector>
#include <map>
#include "Renderer.h"

class BackgroundLayer
{
public:
	enum Style
	{
		e_undefined,
		e_linear,
		e_discrete,
		e_curve
	};
	BackgroundLayer(float dx, const float* dy, size_t ycount, const b2Color& color, float parallax);
	void Draw(Renderer^ renderer, float baseY = 0.0f);
	Style GetStyle(float point) const;

	void Clear();
	void SetStyle(Style style, float point);

private:
	float DrawConvexTile(Renderer^ renderer, Style style, float start_x, float end_x);
	void CreateLinearPoints(float start_x, float end_x, vector<b2Vec2>& points);
	void CreateDiscretePoints(float start_x, float end_x, vector<b2Vec2>& points);
	void CreateCurvePoints(float start_x, float end_x, vector<b2Vec2>& points);
	float ParallaxTransform(float x);

	class RingBuffer
	{
	public:
		RingBuffer(const float* dy, size_t ycount) : m_data(dy, dy + ycount), m_basei(0), m_offset(0.0f) {}
		float At(size_t index) const;
		void SetOffset(float offset) { m_offset = offset; }
		void SetBaseIndex(size_t i) { m_basei = i % m_data.size(); }
		void UpdateBaseIndex(size_t i = 1) { m_basei = (m_basei + i) % m_data.size(); }
		size_t Size() const { return m_data.size(); }
	private:
		std::vector<float> m_data;
		size_t m_basei;
		float m_offset;
	};

	std::map<float, Style> m_styles;
	RingBuffer m_dy;
	float m_dx;
	float m_parallax;
	b2Color m_color;
	VerticeBatch m_graphics;
};