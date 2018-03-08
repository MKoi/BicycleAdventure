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

#include "pch.h"
#include "Graphics.h"
#include "Renderer.h"
#include "SimpleMath.h"

#include <vector>
#include <stdio.h>
#include <stdarg.h>

using namespace DirectX;
using namespace std;
using SimpleMath::Vector2;
#if 0
Graphics::Graphics() : m_clip(false), m_cliptopLeft(), m_clipbottomRight(),
	m_linebatch(D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST), 
	m_trianglebatch(D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST),
	m_pointbatch(D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_POINTLIST)
{

}
#endif

b2Color Graphics::red;
b2Color Graphics::yellow;
b2Color Graphics::green;
b2Color Graphics::black;
b2Color Graphics::grey;
b2Color Graphics::darkGrey;
b2Color Graphics::brown;
b2Color Graphics::blue;
b2Color Graphics::skin;
b2Color Graphics::orange;

void Graphics::InitColors()
{
	red.Set(1.0f, 0.0f, 0.0f);
	yellow.Set(1.0f, 1.0f, 0.0f);
	green.Set(0.0f, 1.0f, 0.0f);
	black.Set(0.0f, 0.0f, 0.0f);
	grey.Set(0.9f, 0.9f, 0.9f);
	darkGrey.Set(0.5f, 0.5f, 0.5f);
	brown.Set(0.5f, 0.3f, 0.1f);
	blue.Set(0.3f, 0.5f, 0.8f);
	skin.Set(255, 165, 0);
	orange.Set(255, 140, 0);
}

void Graphics::SpliceToConvexes(const b2Vec2* verts, int count, vector<pair<const b2Vec2*, size_t>>& result)
{
	if (count < 2)
	{
		return;
	}
	const b2Vec2* last = verts + count - 1;
	const b2Vec2* p1 = verts;
	const b2Vec2* p2 = verts + 1;
	const b2Vec2* p3;

	while (p2 != last)
	{
		p3 = p2 + 1;
		// check if p3 is clockwise to p2 when looking from p1 
		if ((p2->x - p1->x) * (p3->y - p1->y) - (p2->y - p1->y) * (p3->x - p1->x) > 0.0f)	
		{
			result.emplace_back(p1, (p2 - p1) + 1);
			p1 = p2;
		}
		p2 = p3;
	}
	result.emplace_back(p1, (p2 - p1) + 1);
}

void Graphics::CreateCurve(const b2Vec2& start, const b2Vec2& end, size_t count, vector<b2Vec2>& result, bool includeEnd)
{
	if (count == 0)
	{
		return;
	}
	Vector2 p1(start.x, start.y);
	Vector2 p2(end.x, end.y);
	Vector2 t(0.0f);
	float d = 1.0f / count;
	float s = d;
	t.x = p2.x - p1.x;
	Vector2 p3;
	while (s < 1.0f)
	{
		p3 = Vector2::Hermite(p1, t, p2, t, s);
		result.emplace_back(p3.x, p3.y);
		s += d;
	}
	if (includeEnd && p2 != p3)
	{
		result.emplace_back(p2.x, p2.y);
	}
	else if (!includeEnd && p2 == p3)
	{
		result.pop_back();
	}
}

void Graphics::CreateLineFromEndpoint(const b2Vec2& start, const b2Vec2& end, float ratio, b2Vec2* verts)
{
	b2Vec2 diff = 0.5f * (end - start);
	verts[0].Set(start.x + ratio*diff.y, start.y - ratio*diff.x);
	verts[1].Set(end.x + ratio*diff.y, end.y - ratio*diff.x);
	verts[2].Set(end.x - ratio*diff.y, end.y + ratio*diff.x);
	verts[3].Set(start.x - ratio*diff.y, start.y + ratio*diff.x);
}

void Graphics::CreateLineFromCornerPoints(const b2Vec2& start, const b2Vec2& end, float ratio, b2Vec2* verts)
{
	b2Vec2 diff = (end - start);
	verts[0].Set(start.x, start.y);
	verts[1].Set(start.x + ratio*diff.y, start.y - ratio*diff.x);
	verts[2].Set(end.x + ratio*diff.y, end.y - ratio*diff.x);
	verts[3].Set(end.x, end.y);
}

void Graphics::CreateBoxFromEndpoint(const b2Vec2& start, const b2Vec2& end, float width, b2Vec2* verts)
{
	b2Vec2 diff = end - start;
	float ratio = width / diff.Length();
	CreateLineFromEndpoint(start, end, ratio, verts);
}

void Graphics::CreateBoxAxisAligned(const b2Vec2& topLeft, const b2Vec2& bottomRight, b2Vec2* verts)
{
	verts[0] = topLeft;
	verts[1].Set(topLeft.x, bottomRight.y);
	verts[2] = bottomRight;
	verts[3].Set(bottomRight.x, topLeft.y);
}

void Graphics::CreateSolidHalfCircle(const b2Vec2& center, const b2Vec2& perimeter, b2Vec2* verts, int count)
{
	HalfCircleToPointList(center, perimeter, verts, count);
}

void Graphics::CreateBoxAxisAligned(const b2Vec2& topLeft, const b2Vec2& bottomRight, const b2Color& color, VerticeBatch& target)
{
	b2Vec2 verts[4];
	CreateBoxAxisAligned(topLeft, bottomRight, verts);
	CreateSolidQuadilateral(verts, color, target);
}


void Graphics::CreateGround(vector<b2Vec2>& points, float bottomY, const b2Color& color, VerticeBatch& target)
{
	vector<std::pair<const b2Vec2*, size_t>> convexes;
	SpliceToConvexes(points.data(), points.size(), convexes);

	b2Vec2 bottom[2];
	vector<std::pair<const b2Vec2*, size_t>> convex;
	for (const auto& i : convexes)
	{
		convex.clear();
		bottom[0].Set(i.first[i.second - 1].x, min(bottomY, i.first[i.second - 1].y));
		bottom[1].Set(i.first[0].x, min(bottomY, i.first[0].y));

		convex.emplace_back(i.first, i.second);
		convex.emplace_back(bottom, 2);

		CreateSolidConvex(convex, color, target, 1.0f, false);
	}
}

void Graphics::CreateSolidQuadilateral(const b2Vec2* verts, const b2Color& color, VerticeBatch& target)
{
	target.Add(verts, 4, XMFLOAT4(color.r, color.g, color.b, 1.0f));
}

void Graphics::CreateSolidConvex(const b2Vec2* verts, int count, const b2Color& color, VerticeBatch& target, float alpha, bool CCW)
{
	target.Add(verts, count, XMFLOAT4(color.r, color.g, color.b, alpha), VerticeBatch::eConvex, CCW);
}

void Graphics::CreateSolidConvex(vector<pair<const b2Vec2*, size_t>>& vertices, const b2Color& color, VerticeBatch& target, float alpha, bool CCW)
{
	target.Add(vertices, XMFLOAT4(color.r, color.g, color.b, alpha), VerticeBatch::eConvex, CCW);
}

void Graphics::CreateTriangle(const b2Vec2* verts, const b2Color& color, VerticeBatch& target, float alpha)
{
	target.Add(verts, 3, XMFLOAT4(alpha * color.r, alpha * color.g, alpha * color.b, alpha));
}

void Graphics::CreateSolidLine(const b2Vec2& start, const b2Vec2& end, float ratio, const b2Color& color, VerticeBatch& target, float alpha)
{
	b2Vec2 vertices[4];
	CreateLineFromEndpoint(start, end, ratio, vertices);
	target.Add(vertices, 4, XMFLOAT4(alpha * color.r, alpha * color.g, alpha * color.b, alpha));
}

void Graphics::CreateSolidLineRoundEnd(const b2Vec2& start, const b2Vec2& end, float ratio, const b2Color& color, VerticeBatch& target, Rounding round, float alpha)
{
	if (ratio > 1.0f)
	{
		CreateSolidLine(start, end, ratio, color, target, alpha);
		return;
	}
	b2Vec2 line = end - start;
	b2Vec2 end2 = (round == eRoundEnd || round == eRoundBoth) ? (1 - 0.5f * ratio) * line : line;
	b2Vec2 start2 = (round == eRoundStart || round == eRoundBoth) ? (0.5f * ratio) * line : b2Vec2_zero;
	float ratio2 = (round == eRoundStart || round == eRoundEnd) ? ratio / (1.0f - 0.5f * ratio) : ratio / (1.0f - ratio);
	CreateSolidLine(start + start2, start + end2, ratio2, color, target, alpha);
	b2Vec2 perpendicular = b2Cross(line, 0.5f * ratio);
	
	b2Vec2 center;
	if (round == eRoundEnd || round == eRoundBoth)
	{
		center = start + end2;
		CreateSolidHalfCircle(center, perpendicular, color, target, alpha);
	}
	if (round == eRoundStart || round == eRoundBoth)
	{
		center = start + start2;
		CreateSolidHalfCircle(center, -perpendicular, color, target, alpha);
	}
	
}

void Graphics::CreateSolidCircle(const b2Vec2& center, float32 radius, const b2Color& color, VerticeBatch& target, float alpha)
{
	b2Vec2 vertices[k_circleSegments];
	CircleToPointList(center, radius, vertices, k_circleSegments);
	target.Add(vertices, k_circleSegments, XMFLOAT4(alpha * color.r, alpha * color.g, alpha * color.b, alpha));
}

void Graphics::CreateSolidEllipse(const b2Vec2& center, float32 radius, float32 radius2, const b2Color& color, VerticeBatch& target, float alpha)
{
	b2Vec2 vertices[k_circleSegments];
	float yscale = radius2 / radius;
	CircleToPointList(center, radius, vertices, k_circleSegments, yscale);
	target.Add(vertices, k_circleSegments, XMFLOAT4(alpha * color.r, alpha * color.g, alpha * color.b, alpha));
}

void Graphics::CreateSolidHalfCircle(const b2Vec2& center, const b2Vec2& perimeter, const b2Color& color, VerticeBatch& target, float alpha)
{
	const int count = k_circleSegments / 2 + 1;
	b2Vec2 vertices[count];
	HalfCircleToPointList(center, perimeter, vertices, count - 1);
	vertices[count - 1] = center - perimeter;
	target.Add(vertices, count, XMFLOAT4(alpha * color.r, alpha * color.g, alpha * color.b, alpha));
}

void Graphics::CreateSphere(const b2Vec2& center, float32 radius, float32 ratio, const b2Color& color, VerticeBatch& target, float alpha)
{
	b2Vec2 vertices[k_circleSegments * 2];
	XMMATRIX xf = CircleTransform(b2Vec2_zero, k_circleSegments);
	XMVECTOR point = XMVectorSet(radius, 0.0f, 0.0f, 0.0f);
	XMVECTOR point2;
	for (int i = 0; i + 1 < k_circleSegments * 2; i += 2)
	{
		point2 = XMVector2Transform(point, xf);
		vertices[i].Set(center.x + XMVectorGetX(point), center.y + XMVectorGetY(point));
		vertices[i + 1].Set(center.x + ratio*XMVectorGetX(point), center.y + ratio*XMVectorGetY(point));
		point = point2;
		
	}
	target.Add(vertices, k_circleSegments * 2, XMFLOAT4(alpha * color.r, alpha * color.g, alpha * color.b, alpha), VerticeBatch::eSphere);
}

XMMATRIX Graphics::CircleTransform(const b2Vec2& center, int segments)
{
	return RotationTransform(center, segments, XM_2PI);
	/*
	float theta = XM_2PI / (float)segments;
	XMVECTOR scale = XMVectorReplicate(1.0f);
	XMVECTOR origin = XMVectorSet(center.x, center.y, 0.0f, 0.0f);
	XMVECTOR translation =  XMVectorZero();
	return XMMatrixAffineTransformation2D(scale, origin, theta, translation);
	*/
}

#if 0
void Graphics::ApplyTransformAndDraw(float s, const b2Vec2& o, float angle, const b2Vec2& pos, const VerticeBatch& src, VerticeBatch& target)
{
	XMVECTOR scale = XMVectorReplicate(s);
	XMVECTOR translation = XMVectorSet(pos.x, pos.y, 0.0f, 0.0f);
	XMVECTOR origin = XMVectorSet(o.x, o.y, 0.0f, 0.0f);
	if (src.Size() == 0)
	{
		return;
	}
	
	const VertexPositionColor* input = src.Data();
	VertexPositionColor* output = target.Modify(&src);
	XMMATRIX xf = XMMatrixAffineTransformation2D(scale, origin, angle, translation);
	XMVector3TransformCoordStream((XMFLOAT3*)output, sizeof(VertexPositionColor), (XMFLOAT3*)input, sizeof(VertexPositionColor), src.Size(), xf);
}

void Graphics::ApplyTransformAndDraw(const b2Body* body, const VerticeBatch& src, VerticeBatch& target)
{
	ApplyTransformAndDraw(1.0f, b2Vec2_zero, body->GetAngle(), body->GetPosition(), src, target);
}


void Graphics::ApplyTransformAndDraw(const b2Body* body, const VerticeBatch& src)
{
	ApplyTransformAndDraw(1.0f, b2Vec2_zero, body->GetAngle(), body->GetPosition(), src);
}

void Graphics::ApplyTransformAndDraw(float scale, const b2Vec2& origin, float angle, const b2Vec2& pos, const VerticeBatch& src)
{
	b2Vec2 boundingsphere;
	float radius;
	if (src.GetBoundingSphere(boundingsphere, radius))
	{
		if (!Inside(pos + boundingsphere, radius))
		{
			return;
		}
	}
	switch (src.Type())
	{
	case D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
		ApplyTransformAndDraw(scale, origin, angle, pos, src, m_trianglebatch);
		break;
	case D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST:
		ApplyTransformAndDraw(scale, origin, angle, pos, src, m_linebatch);
		break;
	case D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_POINTLIST:
		ApplyTransformAndDraw(scale, origin, angle, pos, src, m_pointbatch);
	default:
		break;
	}
}


void Graphics::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
{
	VerticeBatch b(D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	b2Vec2 vertices[k_circleSegments];
	if (!Inside(center, radius))
	{
		return;
	}
	CircleToPointList(center, radius, vertices);
	b.Add(vertices, k_circleSegments, XMFLOAT4(color.r, color.g, color.b, 1.0f));
	b.Draw(this);
}


void Graphics::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
{
	VerticeBatch lines(D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	VerticeBatch tris(D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	b2Vec2 vertices[k_circleSegments];
	if (!Inside(center, radius))
	{
		return;
	}
	CircleToPointList(center, radius, vertices);
	float alpha = 0.5f;
	tris.Add(vertices, k_circleSegments, XMFLOAT4(alpha*color.r, alpha*color.g, alpha*color.b, alpha));
	lines.Add(vertices, k_circleSegments, XMFLOAT4(color.r, color.g, color.b, 1.0f));
	DrawSegment(vertices[k_circleSegments - 1], vertices[0], color);
	DrawSegment(center, center + radius * axis, color);
	lines.Draw(this);
	tris.Draw(this);
}

void Graphics::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
	VerticeBatch lines(D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	b2Vec2 segment[2];
	segment[0] = p1;
	segment[1] = p2;
	if (Inside(segment, 2))
	{
		lines.Add(segment, 2, XMFLOAT4(color.r, color.g, color.b, 1.0f));
	}
	lines.Draw(this);
}

void Graphics::DrawTransform(const b2Transform& xf)
{
	b2Vec2 p1 = xf.p, p2;
	const float32 k_axisScale = 0.4f;
	p2 = p1 + k_axisScale * xf.q.GetXAxis();
	DrawSegment(p1, p2, Graphics::red);
	p2 = p1 + k_axisScale * xf.q.GetYAxis();
	DrawSegment(p1, p2, Graphics::green);
}

void Graphics::DrawPoint(const b2Vec2& p, float32 size, const b2Color& color)
{
	VerticeBatch b(D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	b.Add(&p, 1, XMFLOAT4(color.r, color.g, color.b, 1.0f));
	b.Draw(this);
}

void Graphics::DrawString(int x, int y, const char *string, ...)
{
#if 0
	char buffer[128];

	va_list arg;
	va_start(arg, string);
	vsprintf(buffer, string, arg);
	va_end(arg);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	int w = glutGet(GLUT_WINDOW_WIDTH);
	int h = glutGet(GLUT_WINDOW_HEIGHT);
	gluOrtho2D(0, w, h, 0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glColor3f(0.9f, 0.6f, 0.6f);
	glRasterPos2i(x, y);
	int32 length = (int32)strlen(buffer);
	for (int32 i = 0; i < length; ++i)
	{
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, buffer[i]);
		//glutBitmapCharacter(GLUT_BITMAP_9_BY_15, buffer[i]);
	}

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
#endif
}

void Graphics::DrawString(const b2Vec2& p, const char *string, ...)
{
#if 0
	char buffer[128];

	va_list arg;
	va_start(arg, string);
	vsprintf(buffer, string, arg);
	va_end(arg);

	glColor3f(0.5f, 0.9f, 0.5f);
	glRasterPos2f(p.x, p.y);

	int32 length = (int32)strlen(buffer);
	for (int32 i = 0; i < length; ++i)
	{
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, buffer[i]);
	}

	glPopMatrix();
#endif
}

void Graphics::DrawAABB(b2AABB* aabb, const b2Color& c)
{
	b2Vec2 vertices[4];
	vertices[0].Set(aabb->lowerBound.x, aabb->lowerBound.y);
	vertices[1].Set(aabb->upperBound.x, aabb->lowerBound.y);
	vertices[2].Set(aabb->upperBound.x, aabb->upperBound.y);
	vertices[3].Set(aabb->lowerBound.x, aabb->upperBound.y);
	VerticeBatch b(D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	b.Add(vertices, 4, XMFLOAT4(c.r, c.g, c.b, 1.0f));
	b.Draw(this);
}
/*
XMMATRIX Graphics::CircleTransform(const b2Vec2& center, int segments)
{
	float theta = XM_2PI / (float)segments;
	XMVECTOR scale = XMVectorReplicate(1.0f);
	XMVECTOR origin = XMVectorSet(center.x, center.y, 0.0f, 0.0f);
	XMVECTOR translation =  XMVectorZero();
	return XMMatrixAffineTransformation2D(scale, origin, theta, translation);
}
*/
#endif
DirectX::XMMATRIX Graphics::RotationTransform(const b2Vec2& center, int segments, float angle)
{
	float theta = angle / (float)segments;
	XMVECTOR scale = XMVectorReplicate(1.0f);
	XMVECTOR origin = XMVectorSet(center.x, center.y, 0.0f, 0.0f);
	XMVECTOR translation = XMVectorZero();
	return XMMatrixAffineTransformation2D(scale, origin, theta, translation);
}

void Graphics::CircleToPointList(const b2Vec2& center, float32 radius, b2Vec2* points, int count, float yscale)
{
	b2Vec2 perimeter(radius, 0.0f);
	CircleSegmentsToPointList(center, perimeter, points, count, XM_2PI, yscale);
}

void Graphics::HalfCircleToPointList(const b2Vec2& center, const b2Vec2& perimeter, b2Vec2* points, int count)
{
	CircleSegmentsToPointList(center, perimeter, points, count, XM_PI);
}

void Graphics::CircleSegmentsToPointList(const b2Vec2& center, const b2Vec2& perimeter, b2Vec2* points, int count, float angle, float yscale)
{
	XMMATRIX xf = RotationTransform(center, count, angle);
	XMVECTOR point = XMVectorSet(center.x + perimeter.x, center.y + perimeter.y, 0.0f, 0.0f);
	for (int i = 0; i < count; ++i)
	{
		float yy = (XMVectorGetY(point) - center.y) * yscale + center.y;
		points[i].Set(XMVectorGetX(point), yy);
		point = XMVector2Transform(point, xf);
	}
}

#if 0
void Graphics::Render(Renderer^ renderer)
{
	renderer->AddToBatch(&m_trianglebatch);
	renderer->AddToBatch(&m_linebatch);
	renderer->AddToBatch(&m_pointbatch);
	renderer->Render();
}


void Graphics::EnableClip(const b2Vec2& topLeft, const b2Vec2& bottomRight)
{
	m_clip = true;
	m_cliptopLeft = topLeft;
	m_clipbottomRight = bottomRight;
}

void Graphics::DisableClip()
{
	m_clip = false;
}

VerticeBatch* Graphics::GetBatch(D3D_PRIMITIVE_TOPOLOGY type)
{
	switch (type)
	{
	case D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
		return &m_trianglebatch;
		break;
	case D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST:
		return &m_linebatch;
		break;
	case D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_POINTLIST:
		return &m_pointbatch;
	default:
		break;
	}
	return 0;
}

bool Graphics::Inside(const b2Vec2* vertices, int32 vertexCount)
{
	if (!m_clip)
	{
		return true;
	}
	for (int i = 0; i < vertexCount; ++i)
	{
		if (TestPoint(vertices[i]))
		{
			return true;
		}
	}
	return false;
}

bool Graphics::Inside(const b2Vec2& center, float32 radius) const
{
	if (!m_clip)
	{
		return true;
	}
	if (TestPoint(center, radius))
	{
		return true;
	}
	return false;
}

bool Graphics::TestPoint(const b2Vec2& point, float margin) const
{
	if (point.x < m_clipbottomRight.x + margin && point.x > m_cliptopLeft.x - margin)
	{
		if (point.y < m_cliptopLeft.y + margin && point.y > m_clipbottomRight.y - margin)
		{
			return true;
		}
	}
	return false;
}
#endif
