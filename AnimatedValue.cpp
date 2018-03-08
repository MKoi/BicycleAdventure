#include "pch.h"
#include "AnimatedValue.h"

using DirectX::SimpleMath::Vector2;
using DirectX::XMFLOAT2;

template<> void Linear<XMFLOAT2>(const XMFLOAT2& v1, const XMFLOAT2& v2, XMFLOAT2& res, float scale)
{
	Vector2 result;
	Linear(Vector2(v1.x, v1.y), Vector2(v2.x, v2.y), result, scale);
	res.x = result.x;
	res.y = result.y;
}

template<> void Curve<XMFLOAT2>(const XMFLOAT2& v1, const XMFLOAT2& v2, const XMFLOAT2& v3, XMFLOAT2& res, float scale)
{
	Vector2 result;
	Curve(Vector2(v1.x, v1.y), Vector2(v2.x, v2.y), Vector2(v3.x, v3.y), result, scale);
	res.x = result.x;
	res.y = result.y;
}

template<> void Linear<Vector2>(const Vector2& v1, const Vector2& v2, Vector2& res, float scale)
{

	res = (v1 != v2) ? Vector2::Lerp(v1, v2, scale) : v1;
}

template<> void Curve<Vector2>(const Vector2& v1, const Vector2& v2, const Vector2& v3, Vector2& res, float scale)
{
	Vector2 t1 = v3 - v1;
	Vector2 t2 = v2 - v3;
	res = (v1 != v2) ? Vector2::Hermite(v1, t1, v2, t2, scale) : v1;
}

using std::vector;

template<> void Linear<vector<XMFLOAT2> >(const vector<XMFLOAT2>& v1, const vector<XMFLOAT2>& v2, vector<XMFLOAT2>& res, float scale)
{
	if (v1.size() != v2.size())
	{
		res = v1;
	}
	vector<XMFLOAT2>::const_iterator i1;
	vector<XMFLOAT2>::const_iterator i2;
	res.clear();
	for (i1 = v1.cbegin(), i2 = v2.cbegin(); i1 != v1.cend() && i2 != v2.cend(); i1++, i2++)
	{
		
		DirectX::XMVECTOR xv1 = DirectX::XMLoadFloat2(&*i1);
		DirectX::XMVECTOR xv2 = DirectX::XMLoadFloat2(&*i2);
		XMFLOAT2 val;
		DirectX::XMStoreFloat2(&val, DirectX::XMVectorLerp(xv1, xv2, scale));
		res.push_back(val);
	}
}

template<> void Curve<vector<XMFLOAT2> >(const vector<XMFLOAT2>& v1, const vector<XMFLOAT2>& v2, const vector<XMFLOAT2>& v3, vector<XMFLOAT2>& res, float scale)
{
	if (v1.size() != v2.size())
	{
		res = v1;
	}
	vector<XMFLOAT2>::const_iterator i1 = v1.cbegin();
	vector<XMFLOAT2>::const_iterator i2 = v2.cbegin();
	vector<XMFLOAT2>::const_iterator i3 = v3.cbegin();
	res.clear();
	for (; i1 != v1.cend() && i2 != v2.cend() && i3 != v3.cend(); i1++, i2++, i3++)
	{
		res.emplace_back();
		DirectX::XMVECTOR xv1 = DirectX::XMLoadFloat2(&*i1);
		DirectX::XMVECTOR xv2 = DirectX::XMLoadFloat2(&*i2);
		DirectX::XMVECTOR xv3 = DirectX::XMLoadFloat2(&*i3);
		DirectX::XMVECTOR t3 = DirectX::XMVector2Orthogonal(xv3);
		if (scale < 0.5f)
		{
			DirectX::XMVECTOR t1 = DirectX::XMVector2Orthogonal(xv1);			
			DirectX::XMStoreFloat2(&res.back(), DirectX::XMVectorHermite(xv1, t1, xv3, t3, 2.0f * scale));
		}
		else
		{
			DirectX::XMVECTOR t2 = DirectX::XMVector2Orthogonal(xv2);
			DirectX::XMStoreFloat2(&res.back(), DirectX::XMVectorHermite(xv3, t3, xv2, t2, 2.0f * scale - 1.0f));
		}
	}
}
