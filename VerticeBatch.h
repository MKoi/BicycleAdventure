#pragma once
#include <vector>
#include <tuple>
#include <DirectXCollision.h>
#include "SimpleMath.h"
#include "VertexTypes.h"
#include "Box2D/Box2D.h"
#include "Animation.h"



class VerticeBatch
{
public:
	enum ShapeHint
	{
		eConvex,
		eSphere
	};

	
	VerticeBatch(D3D_PRIMITIVE_TOPOLOGY type = D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) : 
		m_vertices(new std::vector<DirectX::XMFLOAT2>()), m_indices(new std::vector<uint16_t>()), 
		m_colors(new std::vector<std::tuple<uint16_t, DirectX::XMFLOAT4> >()), m_animation(), m_type(type), 
		m_boundingBox(), m_boundingBoxSet(false), m_localCenter(), m_angle(0.0f), m_pos(), m_scale(1.0f) {}
	void Add(const b2Vec2* vertices, int32 vertexCount, DirectX::XMFLOAT4 color, ShapeHint hint = eConvex, bool CCW = true);
	void Add(std::vector<std::pair<const b2Vec2*, size_t>>& vertices, DirectX::XMFLOAT4 color, ShapeHint hint = eConvex, bool CCW = true);
	void Clear();
	void SetBoundingBox();
	void Update(float dt, float angle, const Vector2& pos, float scale = 1.0f);
	void Update(float dt, float angle, const b2Vec2& pos, float scale = 1.0f);
	void CreateAnimation();
	bool IsAnimated() const { return m_animation != nullptr; }
	std::shared_ptr<Animation> GetAnimation() { return m_animation; }
	DirectX::XMMATRIX GetTransform() const;
	D3D_PRIMITIVE_TOPOLOGY Type() const { return m_type; }
	size_t Size() const { return m_vertices->size(); }
	bool Empty() const { return m_vertices->empty(); }
	size_t IndexCount() const { return m_indices->size(); }
	const uint16_t* IndexData() const { return m_indices->data(); }
	const std::tuple<uint16_t, DirectX::XMFLOAT4>* ColorData() const { return m_colors->data(); }
	size_t ColorCount() const { return m_colors->size(); }
	const XMFLOAT2* Data() const;
	const DirectX::BoundingBox* GetBoundingBox() const;
	bool HasBoundingBox() const { return m_boundingBoxSet; }

private:
	void PointListToLineBatch(const b2Vec2* vertices, int32 vertexCount);
	void PointListToTriangleBatch(const b2Vec2* vertices, int32 vertexCount, ShapeHint hint = eConvex, bool CCW = true);
	int PointListToTriangleBatch(vector<std::pair<const b2Vec2*, size_t>>& vertices, ShapeHint hint = eConvex, bool CCW = true);
	void PointListToPointBatch(const b2Vec2* vertices, int32 vertexCount);
	void SphereToTriangleBatch(const b2Vec2* vertices, int32 vertexCount);
	float FarthestFromOrigin() const;

	std::shared_ptr<std::vector<DirectX::XMFLOAT2> > m_vertices;
	std::shared_ptr<std::vector<uint16_t> > m_indices;
	std::shared_ptr<std::vector<std::tuple<uint16_t, DirectX::XMFLOAT4> > > m_colors;
	std::shared_ptr<Animation> m_animation;
	D3D_PRIMITIVE_TOPOLOGY m_type;
	DirectX::BoundingBox m_boundingBox;
	bool m_boundingBoxSet;
	DirectX::SimpleMath::Vector2  m_localCenter;


	float m_angle;
	DirectX::SimpleMath::Vector2 m_pos;
	float m_scale;

};
