/*
* Copyright (c) 2006-2007 Erin Catto http://www.box2d.org
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

#pragma once

#include <vector>
#include <Box2D/Box2D.h>
#include "Renderer.h"
#include "VerticeBatch.h"


//struct b2AABB;

// This class implements debug drawing callbacks that are invoked
// inside b2World::Step.
class Graphics //: public b2Draw
{
public:
	//Graphics() {}

	static b2Color red;
	static b2Color yellow;
	static b2Color green;
	static b2Color black;
	static b2Color grey;
	static b2Color darkGrey;
	static b2Color brown;
	static b2Color blue;
	static b2Color skin;
	static b2Color orange;

	static void InitColors();

	enum Rounding
	{
		eRoundStart,
		eRoundEnd,
		eRoundBoth
	};

	static DirectX::XMMATRIX CircleTransform(const b2Vec2& center, int segments);

	static void SpliceToConvexes(const b2Vec2* verts, int count, vector<std::pair<const b2Vec2*, size_t>>& result);
	
	static void CreateCurve(const b2Vec2& start, const b2Vec2& end, size_t count, vector<b2Vec2>& result, bool includeEnd = true);

	static void CreateLineFromEndpoint(const b2Vec2& start, const b2Vec2& end, float ratio, b2Vec2* verts);

	static void CreateLineFromCornerPoints(const b2Vec2& start, const b2Vec2& end, float ratio, b2Vec2* verts);

	static void CreateBoxFromEndpoint(const b2Vec2& start, const b2Vec2& end, float width, b2Vec2* verts);

	static void CreateBoxAxisAligned(const b2Vec2& topLeft, const b2Vec2& bottomRight, const b2Color& color, VerticeBatch& target);

	static void CreateBoxAxisAligned(const b2Vec2& topLeft, const b2Vec2& bottomRight, b2Vec2* verts);

	static void CreateSolidHalfCircle(const b2Vec2& center, const b2Vec2& perimeter, b2Vec2* verts, int count);

	static void CreateGround(vector<b2Vec2>& points, float bottomY, const b2Color& color, VerticeBatch& target);

	static void CreateSolidQuadilateral(const b2Vec2* verts, const b2Color& color, VerticeBatch& target);

	static void CreateSolidConvex(const b2Vec2* verts, int count, const b2Color& color, VerticeBatch& target, float alpha = 1.0f, bool CCW = true);
	
	static void CreateSolidConvex(vector<std::pair<const b2Vec2*, size_t>>& vertices, const b2Color& color, VerticeBatch& target, float alpha = 1.0f, bool CCW = true);

	static void CreateTriangle(const b2Vec2* verts, const b2Color& color, VerticeBatch& target, float alpha = 1.0f);
	
	static void CreateSolidLine(const b2Vec2& start, const b2Vec2& end, float ratio, const b2Color& color, VerticeBatch& target, float alpha = 1.0f);

	static void CreateSolidLineRoundEnd(const b2Vec2& start, const b2Vec2& end, float ratio, const b2Color& color, VerticeBatch& target, Rounding round, float alpha = 1.0f);

	static void CreateSolidCircle(const b2Vec2& center, float32 radius, const b2Color& color, VerticeBatch& target, float alpha = 1.0f);

	static void CreateSolidEllipse(const b2Vec2& center, float32 radius, float32 radius2, const b2Color& color, VerticeBatch& target, float alpha = 1.0f);

	static void CreateSolidHalfCircle(const b2Vec2& center, const b2Vec2& perimeter, const b2Color& color, VerticeBatch& target, float alpha = 1.0f);

	static void CreateSphere(const b2Vec2& center, float32 radius, float32 ratio, const b2Color& color, VerticeBatch& target, float alpha = 1.0f);
#if 0
	static void ApplyTransformAndDraw(const b2Body* body, const VerticeBatch& src, VerticeBatch& target);

	static void ApplyTransformAndDraw(float scale, const b2Vec2& origin, float angle, const b2Vec2& pos, const VerticeBatch& src, VerticeBatch& target);

	void ApplyTransformAndDraw(const b2Body* body, const VerticeBatch& src);

	void ApplyTransformAndDraw(float scale, const b2Vec2& origin, float angle, const b2Vec2& pos, const VerticeBatch& src);

	void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);

	void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);

	void DrawSolidPolygon2(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);

	void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color);

	void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color);

	void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color);

	void DrawTransform(const b2Transform& xf);

	void DrawPoint(const b2Vec2& p, float32 size, const b2Color& color);

	void DrawString(int x, int y, const char* string, ...); 

	void DrawString(const b2Vec2& p, const char* string, ...);

	void DrawAABB(b2AABB* aabb, const b2Color& color);	

	void Render(Renderer^ renderer);

	void EnableClip(const b2Vec2& topLeft, const b2Vec2& bottomRight);

	void DisableClip();

	std::vector<DirectX::VertexPositionColor>* GetBatch(D3D_PRIMITIVE_TOPOLOGY type = D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	bool Inside(const b2Vec2& center, float32 radius) const;
	const b2Vec2& GetBottomRight() const { return m_clipbottomRight; }
	const b2Vec2& GetTopLeft() const { return m_cliptopLeft; }
#endif
	private:
	static const int k_circleSegments = 24;
	static DirectX::XMMATRIX RotationTransform(const b2Vec2& center, int segments, float angle);
	static void CircleToPointList(const b2Vec2& center, float32 radius, b2Vec2* points, int count, float yscale = 1.0f);
	static void HalfCircleToPointList(const b2Vec2& center, const b2Vec2& perimeter, b2Vec2* points, int count);
	static void CircleSegmentsToPointList(const b2Vec2& center, const b2Vec2& perimeter, b2Vec2* points, int count, float angle, float yscale = 1.0f);
#if 0
	bool Inside(const b2Vec2* vertices, int32 vertexCount);
	//bool Inside(const b2Vec2& center, float32 radius);
	bool TestPoint(const b2Vec2& point, float margin = 0.0f) const;

	bool m_clip;
	b2Vec2 m_cliptopLeft;
	b2Vec2 m_clipbottomRight;
	VerticeBatch m_linebatch;
	VerticeBatch m_trianglebatch;
	VerticeBatch m_pointbatch;
#endif
	
};
