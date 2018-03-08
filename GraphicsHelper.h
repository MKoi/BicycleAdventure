#pragma once
#include "pch.h"
#include "VertexTypes.h"
#include "VerticeBatch.h"
#include <vector>
#include "Box2D/Box2D.h"

class GraphicsHelper
{
public:

	static void DrawSolidLine(const b2Vec2& start, const b2Vec2& end, float ratio, const b2Color& color, VerticeBatch& target);
	static void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Color& color, VerticeBatch& target);
	static void DrawSphere(const b2Vec2& center, float32 radius, float32 ratio, const b2Color& color, VerticeBatch& target);
	static void ApplyTransform(b2Body* body, VerticeBatch& src, VerticeBatch& target);
private:
	static const int k_circleSegments = 16;
	static XMMATRIX CircleTransform(const b2Vec2& center, int segments);
};

