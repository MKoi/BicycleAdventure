#include "pch.h"
#include "GameArea.h"

using namespace DirectX;

float GameArea::m_startZoneWidth;
GameAudio* GameArea::m_audio;

GameArea::GameArea(b2World* world, const b2Vec2& pos, const Theme* theme, int variant) :
m_theme(theme), m_variant(variant), m_start(b2Vec2_zero), m_end(b2Vec2_zero), m_topLeft(b2Vec2_zero), m_bottomRight(b2Vec2_zero) 
{
}

void GameArea::SaveState(Windows::Storage::Streams::DataWriter^ state) const
{
	Physics::Serialize(m_groundBody, state);
	for_each(m_bodies.begin(), m_bodies.end(), [&state](b2Body* b) {
		Physics::Serialize(b, state);
	});
}

void GameArea::RestoreState(Windows::Storage::Streams::DataReader^ state)
{
	Physics::DeSerialize(m_groundBody, state);
	for_each(m_bodies.begin(), m_bodies.end(), [&state](b2Body* b) {
		Physics::DeSerialize(b, state);
	});
}


void GameArea::Draw(Renderer^ renderBatch)
{
	m_graphics.clear();
	DrawDecorations(renderBatch);
	DrawShapes(renderBatch, m_groundBody);
	for (auto body : m_bodies)
	{
		DrawShapes(renderBatch, body);
	}
	for (unsigned int i = 0; i < m_graphics.size(); ++i)
	{
		renderBatch->AddToBatch(&m_graphics[i]);
	}

}

GameArea::~GameArea()
{
	m_groundBody->GetWorld()->DestroyBody(m_groundBody);
	for (auto body : m_bodies)
	{
		body->GetWorld()->DestroyBody(body);
	}
}

void GameArea::UpdateDimensions(const b2Vec2& point)
{
	if (point.x < m_topLeft.x) m_topLeft.x = point.x;
	if (point.x > m_bottomRight.x) m_bottomRight.x = point.x;
	if (point.y > m_topLeft.y) m_topLeft.y = point.y;
	if (point.y < m_bottomRight.y) m_bottomRight.y = point.y;
}

void GameArea::UpdateDimensions()
{
	float midy = 0.5f * (m_end.y + m_start.y);
	m_topLeft.Set(m_start.x, midy + m_end.x);
	m_bottomRight.Set(m_end.x, midy - m_end.x);
}

bool GameArea::IsSegmentInside(const b2Vec2& p1, const b2Vec2& p2, DirectX::BoundingBox* box)
{
	float minx = box->Center.x - box->Extents.x;
	float maxx = box->Center.x + box->Extents.x;
	if ((p1.x < minx && p2.x < minx) || (p1.x > maxx && p2.x > maxx))
	{
		return false;
	}
	float miny = box->Center.y - box->Extents.y;
	if (p1.y < miny && p2.y < miny)
	{
		return false;
	}
	return true;
}

bool GameArea::IsVerticesInside(const VerticeBatch& vertices, DirectX::BoundingBox* box)
{
	return !vertices.HasBoundingBox() || (vertices.HasBoundingBox() && box->Contains(*vertices.GetBoundingBox()));
}

void GameArea::DrawShapes(Renderer^ renderBatch, b2Body* body)
{
	float minx = renderBatch->GetViewBox()->Center.x - renderBatch->GetViewBox()->Extents.x;
	float maxx = renderBatch->GetViewBox()->Center.x + renderBatch->GetViewBox()->Extents.x;
	if ((StartPoint().x > maxx || EndPoint().x < minx) && body->GetType() != b2_dynamicBody)
	{
		return;
	}
	b2Fixture* f = body->GetFixtureList();
	b2Transform xf = body->GetTransform();
	while (f && !f->IsSensor())
	{
		if (f->GetType() == b2Shape::e_chain)
		{
			DrawGround(renderBatch, static_cast<b2ChainShape*>(f->GetShape()), xf);
		}
		else if (f->GetType() == b2Shape::e_polygon)
		{
			DrawObject(renderBatch, static_cast<b2PolygonShape*>(f->GetShape()), xf);
		}

		f = f->GetNext();
	}

}

void GameArea::DrawDecorations(Renderer^ renderBatch)
{
	float minx = renderBatch->GetViewBox()->Center.x - renderBatch->GetViewBox()->Extents.x;
	float maxx = renderBatch->GetViewBox()->Center.x + renderBatch->GetViewBox()->Extents.x;
	if (StartPoint().x > maxx || EndPoint().x < minx)
	{
		return;
	}
	for (size_t i = 0; i < m_decorations.size(); ++i)
	{
		if (DrawOrNot(i) && m_theme->HasDecorations())
		{
			b2Vec2 pos = b2Mul(m_groundBody->GetTransform(), m_decorations[i].m_pos);
			XMFLOAT3 tmp = m_theme->GetDecoration(i).GetBoundingBox()->Center;
			b2Vec2 center(pos.x + tmp.x, pos.y + tmp.y);
			XMFLOAT3 extents = m_theme->GetDecoration(i).GetBoundingBox()->Extents;

			if (m_decorations[i].m_maxwidth > extents.x * 2.0f)
			{
				continue;
			}
			// add some safety margin because bounding box is not recalculated in animations
			if (m_theme->GetDecoration(i).IsAnimated())
			{
				extents.x *= 2.0f;
				extents.y *= 2.0f;
			}
			b2Vec2 left(center.x - extents.x, center.y + extents.y);
			b2Vec2 right(center.x + extents.x, left.y);
			if (IsSegmentInside(left, right, renderBatch->GetViewBox()) 
				&& left.x > StartPoint().x && right.x < EndPoint().x)
			{
				m_graphics.push_back(m_theme->GetDecoration(i));
				m_graphics.back().Update(0.0f, m_decorations[i].m_angle, pos, m_decorations[i].m_scale);
			}

		}		
	}
}

/*
void GameArea::DrawGround(Renderer^ renderBatch, b2ChainShape* chain, const b2Transform& xf)
{
	int count = chain->m_count;
	const b2Vec2* vertices = chain->m_vertices;

	b2Vec2 v[4];
	v[0] = b2Mul(xf, vertices[0]);
	for (int32 i = 1; i < count; ++i)
	{
		v[3] = b2Mul(xf, vertices[i]);
		if (v[3].x > v[0].x && IsSegmentInside(v[0], v[3], renderBatch->GetViewBox()))
		{
			float bottomY = renderBatch->GetViewBox()->Center.y - renderBatch->GetViewBox()->Extents.y;
			v[1] = b2Vec2(v[0].x, min(bottomY, v[0].y));
			v[2] = b2Vec2(v[3].x, min(bottomY, v[3].y));
			m_graphics.emplace_back();
			Graphics::CreateSolidConvex(v, 4, m_theme->Color, m_graphics.back());
		}
		v[0] = v[3];
	}
}
*/
void GameArea::DrawGround(Renderer^ renderBatch, b2ChainShape* chain, const b2Transform& xf)
{
	int max = chain->m_count;
	const b2Vec2* start = chain->m_vertices;
	const b2Vec2* end = start + chain->m_count - 1;
	
	float leftx = renderBatch->GetViewLeft() - m_groundBody->GetPosition().x;
	float rightx = renderBatch->GetViewRight() - m_groundBody->GetPosition().x;
	while (start->x < leftx && start != end)
	{
		start++;
	}
	if (start != chain->m_vertices)
	{
		start--;
	}
	while (end->x > rightx && start != end)
	{
		end--;
	}
	if (end != chain->m_vertices + chain->m_count - 1)
	{
		end++;
	}
	int count = end - start;
	
	vector<b2Vec2> points;
	b2Vec2 p;
	while (start != end)
	{
		p = b2Mul(xf, *start);
		points.push_back(p);
		start++;
	}
	p = b2Mul(xf, *end);
	points.push_back(p);

	float bottomY = renderBatch->GetViewBox()->Center.y - renderBatch->GetViewBox()->Extents.y;
	m_graphics.emplace_back();
	Graphics::CreateGround(points, bottomY, m_theme->Color, m_graphics.back());
	
}


void GameArea::DrawObject(Renderer^ renderBatch, b2PolygonShape* poly, const b2Transform& xf)
{
	b2Vec2 vertices[b2_maxPolygonVertices];
	int vertexCount = poly->m_count;
	for (int32 i = 0; i < vertexCount; ++i)
	{
		vertices[i] = b2Mul(xf, poly->m_vertices[i]);
	}
	m_graphics.emplace_back();
	Graphics::CreateSolidConvex(vertices, vertexCount, m_theme->Color, m_graphics.back());
	if (!IsVerticesInside(m_graphics.back(), renderBatch->GetViewBox()))
	{
		m_graphics.pop_back();
	}
}


bool GameArea::DrawOrNot(int seed)
{
	int i = seed + (int)m_groundBody->GetPosition().x;
	if (i % 5 == 0 || i % 7 == 0 || i % 11 == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}


