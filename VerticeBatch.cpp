#include "pch.h"
#include "VerticeBatch.h"
#include "Graphics.h"
#include "DirectXMath.h"
#include <limits>

using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace std;

void VerticeBatch::Add(const b2Vec2* vertices, int32 vertexCount, XMFLOAT4 color, ShapeHint hint, bool CCW)
{
	switch (m_type)
	{
	case D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
		PointListToTriangleBatch(vertices, vertexCount, hint, CCW);
		break;
	case D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST:
		PointListToLineBatch(vertices, vertexCount);
		break;
	case D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_POINTLIST:
		PointListToPointBatch(vertices, vertexCount);
	default:
		break;
	}
	m_colors->emplace_back(vertexCount, color);
}

void VerticeBatch::Add(vector<std::pair<const b2Vec2*, size_t>>& vertices, DirectX::XMFLOAT4 color, ShapeHint hint, bool CCW)
{
	if (m_type == D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
	{
		int r = PointListToTriangleBatch(vertices, hint, CCW);
		m_colors->emplace_back(r, color);
	}
}

void VerticeBatch::SetBoundingBox()
{
	vector<XMFLOAT3> vec3;
	vec3.reserve(m_vertices->size());
	for (auto v = m_vertices->cbegin(); v != m_vertices->cend(); v++)
	{
		vec3.emplace_back(v->x, v->y, 0.0f);
	}

	BoundingBox::CreateFromPoints(m_boundingBox, vec3.size(), vec3.data(), sizeof(XMFLOAT3));
	m_boundingBoxSet = true;
	m_localCenter.x = m_boundingBox.Center.x;
	m_localCenter.y = m_boundingBox.Center.y;
}

void VerticeBatch::CreateAnimation()
{
	m_animation.reset(new Animation(*m_vertices.get()));
	//m_animation->Vertices.SetStart();
}
/*
void VerticeBatch::SetAnimation(Animation type, float duration)
{
	m_animation = type;
	m_duration = duration;
	if (m_animation == eBend)
	{
		m_distFromOrigin = FarthestFromOrigin();
		m_vertices2.resize(m_vertices->size());
		vector<VertexPositionColor>::const_iterator in;
		vector<VertexPositionColor>::iterator out;
		for (in = m_vertices->cbegin(), out = m_vertices2.begin(); in != m_vertices->cend() && out != m_vertices2.end(); in++, out++)
		{
			out->color = in->color;
		}
	}
}
*/

void VerticeBatch::Update(float dt, float angle, const Vector2& pos, float scale)
{
	m_angle = angle;
	m_pos = pos;
	m_scale = scale;
	if (m_boundingBoxSet)
	{
		m_boundingBox.Center.x = m_localCenter.x + pos.x;
		m_boundingBox.Center.y = m_localCenter.y + pos.y;
	}
	if (m_animation)
	{
		m_animation->Update(dt);
	}
}

void VerticeBatch::Update(float dt, float angle, const b2Vec2& pos, float scale)
{
	Vector2 pos2(pos.x, pos.y);
	Update(dt, angle, pos2, scale);

	/*
	if (dt > 0.0f && m_animation == eBend)
	{
		// update transformed vertices
		XMVECTOR rec = XMVectorReciprocalEst(XMVectorReplicate(m_distFromOrigin));
		float theta = m_minAngle + GetProgress() * (m_maxAngle - m_minAngle);
		XMVECTOR rot = XMQuaternionRotationRollPitchYaw(0.0f, 0.0f, -theta);
		vector<VertexPositionColor>::const_iterator in;
		vector<VertexPositionColor>::iterator out;
		for (in = m_vertices->cbegin(), out = m_vertices2.begin(); in != m_vertices->cend() && out != m_vertices2.end(); in++, out++)
		{
			XMVECTOR v = XMLoadFloat3(&(in->position));
			XMVECTOR v2 = XMVector3Rotate(v, rot);
			v2 = XMVectorLerpV(v, v2, XMVectorMultiply(XMVector2Length(v), rec));
			out->position.x = XMVectorGetX(v2);
			out->position.y = XMVectorGetY(v2);
			out->position.z = 0.0f;
		}
	}
	*/
}
/*
void VerticeBatch::Draw(Graphics* gfx, float scale, const b2Vec2& origin, float angle, const b2Vec2& pos)
{
	if (m_radius != 0.0f && !gfx->Inside(pos + m_origin, m_radius))
	{
		return;
	}
	XMMATRIX xf = GetTransform(scale, origin, angle, pos);
	const VertexPositionColor* input = m_vertices.data();
	VertexPositionColor* output = gfx->GetBatch(m_type)->Modify(this);

	XMVector3TransformCoordStream((XMFLOAT3*)output, sizeof(VertexPositionColor), (XMFLOAT3*)input, sizeof(VertexPositionColor), Size(), xf);
#if 1
	if (m_animation == eBend)
	{
		XMVECTOR rec = XMVectorReciprocalEst(XMVectorReplicate(m_distFromOrigin));
		for (unsigned int i = 0; i < Size(); ++i)
		{
			XMVECTOR iv = XMLoadFloat3(&input->position);
			XMVECTOR px = XMVectorSet(pos.x, pos.y, 0.0f, 0.0f);
			XMVECTOR ov = XMLoadFloat3(&output->position);
			XMVECTOR rv = XMVectorMultiply(XMVector2Length(iv), rec);
			iv = XMVectorAdd(iv, px);
			XMVECTOR dv = XMVectorSubtract(ov, iv);
			ov = XMVectorMultiplyAdd(dv, rv, iv);
			output->position.x = XMVectorGetX(ov);
			output->position.y = XMVectorGetY(ov);
			output++;
			input++;
		}
	}
#endif
}
*/

void VerticeBatch::Clear()
{
	m_vertices->clear();
	m_indices->clear();
	m_colors->clear();

	m_boundingBoxSet = false;
	m_localCenter.x = 0.0f;
	m_localCenter.y = 0.0f;
}

const BoundingBox* VerticeBatch::GetBoundingBox() const
{
	return &m_boundingBox;
}

const XMFLOAT2* VerticeBatch::Data() const
{
	 return (m_animation) ? m_animation->Vertices.Get().data() : m_vertices->data();
}

void VerticeBatch::PointListToLineBatch(const b2Vec2* vertices, int32 vertexCount)
{
	if (vertexCount < 2)
	{
		return;
	}
	int base = m_vertices->size();
	m_vertices->emplace_back(vertices[0].x, vertices[0].y);
	m_vertices->emplace_back(vertices[1].x, vertices[1].y);
	m_indices->push_back(base);
	m_indices->push_back(base + 1);
	for (int i = 2; i < vertexCount; ++i)
	{
		m_vertices->emplace_back(vertices[i].x, vertices[i].y);
		m_indices->push_back(base + i - 1);
		m_indices->push_back(base + i);
	}
}

void VerticeBatch::PointListToTriangleBatch(const b2Vec2* vertices, int32 vertexCount, ShapeHint hint, bool CCW)
{
	if (vertexCount < 3)
	{
		return;
	}
	if (hint == eSphere)
	{
		SphereToTriangleBatch(vertices, vertexCount);
		return;
	}
	int base = m_vertices->size();
	if (CCW)
	{
		m_vertices->emplace_back(vertices[0].x, vertices[0].y);
		m_vertices->emplace_back(vertices[1].x, vertices[1].y);
		m_vertices->emplace_back(vertices[2].x, vertices[2].y);
		m_indices->push_back(base);
		m_indices->push_back(base + 2);
		m_indices->push_back(base + 1);
		for (int i = 3; i < vertexCount; ++i)
		{
			m_vertices->emplace_back(vertices[i].x, vertices[i].y);
			m_indices->push_back(base);
			m_indices->push_back(base + i);
			m_indices->push_back(base + i - 1);
		}
	}
	else
	{
		int i;
		for (i = 0; i < vertexCount; ++i)
		{
			m_vertices->emplace_back(vertices[i].x, vertices[i].y);
		}
		while (i - 2 > 0)
		{
			m_indices->push_back(base);
			m_indices->push_back(base + i - 1);
			m_indices->push_back(base + i - 2);
			i--;
		}
	}
}


int VerticeBatch::PointListToTriangleBatch(vector<std::pair<const b2Vec2*, size_t>>& vertices, ShapeHint hint, bool CCW)
{
	int base = m_vertices->size();
	int j = 0;
	for (const auto& p : vertices)
	{
		for (size_t i = 0; i < p.second; ++i, ++j)
		{
			m_vertices->emplace_back(p.first[i].x, p.first[i].y);
		}
	}
	int ret = j;
	while (j - 2 > 0)
	{
		m_indices->push_back(base);
		if (CCW)
		{
			m_indices->push_back(base + j - 1);
			m_indices->push_back(base + j - 2);
		}
		else
		{
			m_indices->push_back(base + j - 2);
			m_indices->push_back(base + j - 1);
		}
		j--;
	}
	return ret;
}

void VerticeBatch::SphereToTriangleBatch(const b2Vec2* vertices, int32 vertexCount)
{
	int base = m_vertices->size();
	m_vertices->emplace_back(vertices[0].x, vertices[0].y);
	m_vertices->emplace_back(vertices[1].x, vertices[1].y);
	for (int i = 2; i + 1 < vertexCount; i += 2)
	{
		m_vertices->emplace_back(vertices[i].x, vertices[i].y);
		m_vertices->emplace_back(vertices[i + 1].x, vertices[i + 1].y);
		m_indices->push_back(base + i);
		m_indices->push_back(base + i - 2);
		m_indices->push_back(base + i - 1);
		m_indices->push_back(base + i + 1);
		m_indices->push_back(base + i);
		m_indices->push_back(base + i - 1);
	}
	m_indices->push_back(base);
	m_indices->push_back(base + vertexCount - 2);
	m_indices->push_back(base + vertexCount - 1);
	m_indices->push_back(base + 1);
	m_indices->push_back(base);
	m_indices->push_back(base + vertexCount - 1);
}

void VerticeBatch::PointListToPointBatch(const b2Vec2* vertices, int32 vertexCount)
{
	int base = m_vertices->size();
	for (int i = 0; i < vertexCount; ++i)
	{
		m_vertices->emplace_back(vertices[i].x, vertices[i].y);
		m_indices->push_back(base + i);
	}
}


XMMATRIX VerticeBatch::GetTransform() const
{
	float xscale = m_scale;
	float yscale = m_scale;
	float theta = m_angle;
	Vector2 pos = m_pos;
	if (m_animation)
	{
		xscale *= m_animation->Xscale.Get();
		yscale *= m_animation->Yscale.Get();
		theta += m_animation->Angle.Get();
		pos += m_animation->Position.Get();
	}

	XMVECTOR s = XMVectorSet(xscale, yscale, 1.0f, 1.0f);
	XMVECTOR translation = XMVectorSet(pos.x, pos.y, 0.0f, 0.0f);
	XMVECTOR o = XMVectorZero();
	return XMMatrixAffineTransformation2D(s, o, theta, translation);
}

/*
float VerticeBatch::FarthestFromOrigin() const
{
	XMVECTOR max = XMVectorZero();
	for_each(m_vertices->begin(), m_vertices->end(), [&max](VertexPositionColor v) {
		XMVECTOR dist = XMLoadFloat3(&v.position);
		max = XMVectorMax(max, XMVector2Length(dist));
	});
	return XMVectorGetX(max);
}
*/