#include "pch.h"
#include "GraphicsHelper.h"

using namespace DirectX;

void GraphicsHelper::DrawSolidLine(const b2Vec2& start, const b2Vec2& end, float ratio, const b2Color& color, VerticeBatch& target)
{
	b2Vec2 diff = end - start;
	b2Vec2 vertices[4];
	vertices[0] = b2Vec2(start.x + ratio*diff.y, start.y - ratio*diff.x);
	vertices[1] = b2Vec2(end.x + ratio*diff.y, end.y - ratio*diff.x);
	vertices[2] = b2Vec2(end.x - ratio*diff.y, end.y + ratio*diff.x);
	vertices[3] = b2Vec2(start.x - ratio*diff.y, start.y + ratio*diff.x);
	target.Add(vertices, 4, XMFLOAT4(color.r, color.g, color.b, 1.0f));
}

void GraphicsHelper::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Color& color, VerticeBatch& target)
{
	b2Vec2 vertices[k_circleSegments];
	XMMATRIX xf = CircleTransform(center, k_circleSegments);
	XMVECTOR point = XMVectorSet(center.x + radius, center.y, 0.0f, 0.0f);
	for (int i = 0; i < k_circleSegments; ++i)
	{
		vertices[i].Set(XMVectorGetX(point), XMVectorGetY(point));
		point = XMVector2Transform(point, xf);
	}
	target.Add(vertices, k_circleSegments, XMFLOAT4(color.r, color.g, color.b, 1.0f));
}

void GraphicsHelper::DrawSphere(const b2Vec2& center, float32 radius, float32 ratio, const b2Color& color, VerticeBatch& target)
{
	b2Vec2 vertices[4];
	XMMATRIX xf = CircleTransform(b2Vec2_zero, k_circleSegments);
	XMVECTOR point = XMVectorSet(radius, 0.0f, 0.0f, 0.0f);
	XMVECTOR point2;
	for (int i = 0; i < k_circleSegments; ++i)
	{
		point2 = XMVector2Transform(point, xf);
		vertices[0].Set(center.x + ratio*XMVectorGetX(point), center.y + ratio*XMVectorGetY(point));
		vertices[1].Set(center.x + XMVectorGetX(point), center.y + XMVectorGetY(point));
		vertices[2].Set(center.x + XMVectorGetX(point2), center.y + XMVectorGetY(point2));
		vertices[3].Set(center.x + ratio*XMVectorGetX(point2), center.y + ratio*XMVectorGetY(point2));
		point = point2;
		target.Add(vertices, 4, XMFLOAT4(color.r, color.g, color.b, 1.0f));
	}
}

void GraphicsHelper::ApplyTransform(b2Body* body, VerticeBatch& src, VerticeBatch& target)
{
	XMVECTOR scale = XMVectorReplicate(1.0f);
	XMVECTOR translation = XMVectorSet(body->GetPosition().x, body->GetPosition().y, 0.0f, 0.0f);
	XMVECTOR origin = XMVectorZero();
	float theta = body->GetAngle();
	int count = src.Data().size();
	VertexPositionColor* input = src.Data().data();
	VertexPositionColor* output = target.Modify(count);
	XMMATRIX xf = XMMatrixAffineTransformation2D(scale, origin, theta, translation);
	XMVector3TransformCoordStream((XMFLOAT3*)output, sizeof(VertexPositionColor), (XMFLOAT3*)input, sizeof(VertexPositionColor), count, xf);
	for(int i = 0; i < count; ++i)
	{
		output->color = input->color;
		input++;
		output++;
	}
}

XMMATRIX GraphicsHelper::CircleTransform(const b2Vec2& center, int segments)
{
	float theta = XM_2PI / (float)segments;
	XMVECTOR scale = XMVectorReplicate(1.0f);
	XMVECTOR origin = XMVectorSet(center.x, center.y, 0.0f, 0.0f);
	XMVECTOR translation =  XMVectorZero();
	return XMMatrixAffineTransformation2D(scale, origin, theta, translation);
}