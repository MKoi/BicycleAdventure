#pragma once
#include <vector>
#include <string>
#include "Box2D\Box2D.h"
#include "Physics.h"
#include "Graphics.h"
#include "GameAudio.h"
#include "Theme.h"



class GameArea
{
public:
	static void SetStartZoneWidth(float w) { m_startZoneWidth = w; }
	static void SetAudio(GameAudio* audio) { m_audio = audio; }
	static GameAudio* GetAudio() { return m_audio; }


	template <typename Object> static std::unique_ptr<GameArea> Create(b2World* world, const b2Vec2& pos, const Theme* theme, int variant)
	{
		std::unique_ptr<GameArea> p(new Object(world, pos, theme, variant));
		return p;
	}
	GameArea(b2World* world, const b2Vec2& pos, const Theme* theme, int variant);
	b2Vec2 StartPoint() const { return m_groundBody->GetWorldPoint(m_start); }
	b2Vec2 EndPoint() const { return m_groundBody->GetWorldPoint(m_end); }
	b2Vec2 CenterPoint() const { return m_groundBody->GetWorldPoint(0.5f * (m_topLeft + m_bottomRight)); }
	float Height() const { return m_topLeft.y - m_bottomRight.y; }
	float Width() const { return m_bottomRight.x - m_topLeft.x; }
	int Variant() const { return m_variant; }
	const Theme* GetTheme() const { return m_theme; }
	virtual bool IsFullScreen() const { return false; }
	virtual void Update(float dt) {}
	virtual void Draw(Renderer^ renderBatch);
	virtual void SaveState(Windows::Storage::Streams::DataWriter^ state) const;
	virtual void RestoreState(Windows::Storage::Streams::DataReader^ state);
	virtual ~GameArea();
protected:
	void UpdateDimensions(const b2Vec2& point);
	void UpdateDimensions();
	bool IsSegmentInside(const b2Vec2& p1, const b2Vec2& p2, DirectX::BoundingBox* box);
	bool IsVerticesInside(const VerticeBatch& vertices, DirectX::BoundingBox* box);
	void DrawShapes(Renderer^ renderBatch, b2Body* body);
	void DrawDecorations(Renderer^ renderBatch);
	void DrawGround(Renderer^ renderBatch, b2ChainShape* chain, const b2Transform& xf);
	void DrawObject(Renderer^ renderBatch, b2PolygonShape* poly, const b2Transform& xf);
	
	struct DecorationXf
	{
		DecorationXf(const b2Vec2& pos) : m_pos(pos), m_angle(0.0f), m_scale(1.0f), m_maxwidth(0.0f) {}
		DecorationXf(const b2Vec2& pos, float width) : m_pos(pos), m_angle(0.0f), m_scale(1.0f), m_maxwidth(width) {}
		DecorationXf(const b2Vec2& pos, float width, float angle, float scale) :
			m_pos(pos), m_angle(angle), m_scale(scale), m_maxwidth(scale * width) {}
		b2Vec2 m_pos;
		float m_angle;
		float m_scale;
		float m_maxwidth;
	};

	const Theme* m_theme;
	int m_variant;
	b2Body* m_groundBody;
	std::vector<b2Body*> m_bodies;
	std::vector<VerticeBatch> m_graphics;
	std::vector<DecorationXf> m_decorations;
	b2Vec2 m_start;
	b2Vec2 m_end;
    b2Vec2 m_topLeft;
	b2Vec2 m_bottomRight;
	static float m_startZoneWidth;
	static GameAudio* m_audio;
private:
	bool DrawOrNot(int seed);

};
