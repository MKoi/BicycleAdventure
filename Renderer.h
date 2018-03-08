#pragma once

#include <vector>
#include <map>
#include <string>
#include "Direct3DBase.h"
// directxtk stuff
#include "CommonStates.h"
#include "Effects.h"
#include "GeometricPrimitive.h"
#include "Model.h"
#include "PrimitiveBatch.h"
#include "SpriteBatch.h"
#include "SpriteFont.h"
#include "VertexTypes.h"
#include "VerticeBatch.h"
#include "SimpleMath.h"



// This class renders a simple spinning cube.
ref class Renderer sealed : public Direct3DBase
{
public:

	Renderer();

	// Direct3DBase methods.
	virtual void CreateDeviceResources() override;
	virtual void CreateWindowSizeDependentResources() override;
	virtual void HandleDeviceLost() override;
	virtual void ReleaseResourcesForSuspending() override;
	virtual void Render() override;
	
//	void SetCenter(float x, float y);
//	void SetWidth(float width);
//	void BeginDraw();
	void SetTransitionTime(float time);
	void SetCameraTarget(float x, float y, float width);
	void SetCameraToTarget();
	bool Ready() { return m_ready; }
//	float GetAspectRatio() { return m_aspectRatio; }
	float GetScale() { return m_width / m_minwidth; }
	float GetWorldToScreenRatio() { return m_windowBounds.Width / m_minwidth; }
	float GetScreenHeight() { return m_windowBounds.Height; }
	float GetScreenWidth() { return m_windowBounds.Width; }
//	float GetHeight() { return m_aspectRatio * m_width; }// m_windowBounds.Height; }
//	float GetWidth() { return m_width; } //m_windowBounds.Width; }
//	float GetCenterX() { return m_center.x; }
//	float GetCenterY() { return m_center.y; }
	// Method for updating time-dependent objects.
	void Update(float timeTotal, float timeDelta);
internal:
	static const int Background = -1;
	static const int Middleground = 0;
	static const int Foreground = 1;
	static const int Foremost = 2;
	void SetDeadZone(float z = 0.5f);
	void AddToBatch(const wchar_t* text, const DirectX::XMFLOAT2& pos, const DirectX::XMFLOAT2& scale, const DirectX::XMVECTOR& color = DirectX::Colors::White);
	void AddToBatch(const VerticeBatch* batch, int z = Middleground);
	DirectX::BoundingBox* GetViewBox() { return &m_viewBox; }
	DirectX::SimpleMath::Vector2 GetViewTopLeft(float offset = 0.0f) const;
	DirectX::SimpleMath::Vector2 GetViewTopRight(float offset = 0.0f) const;
	float GetViewLeft() const { return GetViewTopLeft().x; }
	float GetViewRight() const { return GetViewTopRight().x; }
	float GetDeltaY() const { return m_dy; }
	float GetTransitionTime() const { return m_targetTime; }
	DirectX::SimpleMath::Vector2 GetViewCenter() const;
	void SaveState(Windows::Storage::Streams::DataWriter^ state) const;
	void RestoreState(Windows::Storage::Streams::DataReader^ state);

private:
	struct RenderTarget
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_texture;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;
	};

	struct ScreenText
	{
		const wchar_t* m_text;
		DirectX::XMFLOAT2 m_pos;
		DirectX::XMVECTOR m_color;
		DirectX::XMFLOAT2 m_scale;
	};
	void CreateRenderTarget(RenderTarget& rt, int w, int h, ID3D11Device* device);
	float ApplyCameraDeadZone(float val, float deadzone);

	RenderTarget m_renderTarget2;
	static const int m_maxSize = 16384;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_batchInputLayout;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;

	std::unique_ptr<DirectX::SpriteFont> m_font;
	std::unique_ptr<DirectX::BasicEffect> m_batchEffect;
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_batch;
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
	std::unique_ptr<DirectX::SpriteFont> m_spriteFont;
	std::map<int, std::vector<const VerticeBatch*>> m_batchesToProcess;
	std::unique_ptr<DirectX::CommonStates> m_states;

//    DirectX::SimpleMath::Matrix m_view;
//	DirectX::SimpleMath::Matrix m_projection;

	bool m_ready;
	float m_aspectRatio;
	float m_minwidth;
	float m_deadzone;
	DirectX::SimpleMath::Vector3 m_targetCenter;
	DirectX::SimpleMath::Vector3 m_center;
	DirectX::BoundingBox m_viewBox;
	float m_targetWidth;
	float m_targetTime;
	float m_width;
	float m_dy;
	std::vector<ScreenText> m_texts;
};