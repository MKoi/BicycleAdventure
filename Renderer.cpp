#include "pch.h"
#include "Renderer.h"
#include <math.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace Microsoft::WRL;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace std;

Renderer::Renderer()
{
	m_ready = false;
	m_minwidth = 10.0f;
	SetDeadZone();
}


void Renderer::CreateRenderTarget(RenderTarget& rt, int w, int h, ID3D11Device* device)
{
	rt.m_renderTargetView = nullptr;
	rt.m_shaderResourceView = nullptr;
	rt.m_texture = nullptr;
	// Create the render target texture.
	CD3D11_TEXTURE2D_DESC rtDesc(DXGI_FORMAT_B8G8R8X8_UNORM, w, h, 1, 1, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);

	DX::ThrowIfFailed(
		device->CreateTexture2D(&rtDesc, nullptr, &rt.m_texture)
		);

	// Create the render target view.
	CD3D11_RENDER_TARGET_VIEW_DESC rtvDesc(rt.m_texture.Get(), D3D11_RTV_DIMENSION_TEXTURE2D);

	DX::ThrowIfFailed(
		device->CreateRenderTargetView(rt.m_texture.Get(), &rtvDesc, &rt.m_renderTargetView)
		);

	// Create the shader resource view.
	CD3D11_SHADER_RESOURCE_VIEW_DESC viewDesc(rt.m_texture.Get(), D3D_SRV_DIMENSION_TEXTURE2D);

	DX::ThrowIfFailed(
		device->CreateShaderResourceView(rt.m_texture.Get(), &viewDesc, &rt.m_shaderResourceView)
		);
}

void Renderer::CreateDeviceResources()
{
	Direct3DBase::CreateDeviceResources();

	 auto device = m_d3dDevice.Get();
	 auto context = m_d3dContext.Get();
	 m_batch.reset( new PrimitiveBatch<VertexPositionColor>( context, m_maxSize * 3, m_maxSize ) );
	 m_spriteBatch.reset(new SpriteBatch( context ));
	 m_batchEffect.reset( new BasicEffect( device ) );
	 m_spriteFont.reset(new SpriteFont(device, L"assets\\gamefont.spritefont"));
	 m_states.reset(new CommonStates( device ));
     m_batchEffect->SetVertexColorEnabled(true);

     {
        void const* shaderByteCode;
        size_t byteCodeLength;

        m_batchEffect->GetVertexShaderBytecode( &shaderByteCode, &byteCodeLength );

        DX::ThrowIfFailed(
            device->CreateInputLayout( VertexPositionColor::InputElements,
                                        VertexPositionColor::InputElementCount,
                                        shaderByteCode, byteCodeLength,
                                        m_batchInputLayout.ReleaseAndGetAddressOf() ) 
                         );
     }
	 auto loadPSTask = DX::ReadDataAsync("SimplePixelShader.cso");
	 auto createPSTask = loadPSTask.then([this](Platform::Array<byte>^ fileData) {
		 DX::ThrowIfFailed(
			 m_d3dDevice->CreatePixelShader(
			 fileData->Data,
			 fileData->Length,
			 nullptr,
			 m_pixelShader.ReleaseAndGetAddressOf() //&m_pixelShader
			 )
			 );
		 m_ready = true;
	 });
}

void Renderer::HandleDeviceLost()
{
	m_ready = false;
	Direct3DBase::HandleDeviceLost();
}

void Renderer::ReleaseResourcesForSuspending()
{
	m_ready = false;
	Direct3DBase::ReleaseResourcesForSuspending();
}

void Renderer::CreateWindowSizeDependentResources()
{
	Direct3DBase::CreateWindowSizeDependentResources();
	CreateRenderTarget(m_renderTarget2, (int)m_windowBounds.Width, (int)m_windowBounds.Height, m_d3dDevice.Get());

	m_aspectRatio = m_windowBounds.Height / m_windowBounds.Width;
	m_width = m_minwidth;
	m_dy = 0.0f;
	m_ready = true;
}

void Renderer::SetTransitionTime(float time)
{
	m_targetTime = time;
}

void Renderer::SetCameraTarget(float x, float y, float width)
{
	m_targetCenter.x = x;
	m_targetCenter.y = y;
	m_targetWidth = width;
}

void Renderer::SetDeadZone(float z)
{
	m_deadzone = z;
}

void Renderer::SetCameraToTarget()
{
	m_center = m_targetCenter;
	m_width = m_targetWidth;
	m_targetTime = 0.0f;
}

void Renderer::Update(float timeTotal, float dt)
{

	float ratio = m_targetTime > 0.0f ? min(dt / m_targetTime, 1.0f) : 1.0f;
	Vector3 m_next = Vector3::Lerp(m_center, m_targetCenter, 0.9f*ratio);
	m_dy = ApplyCameraDeadZone(m_next.y - m_center.y, m_deadzone * m_width);
	m_center.y += m_dy;
	m_center.x = m_next.x;
	m_width = max(m_minwidth, m_width + 0.6f*ratio * (m_targetWidth - m_width));
	m_targetTime -= min(m_targetTime, dt);
	m_viewBox.Center = m_center;
	m_viewBox.Extents.x = 0.5f * m_width;
	m_viewBox.Extents.y = m_aspectRatio * 0.5f * m_width;
}

void Renderer::AddToBatch(const wchar_t* text, const XMFLOAT2& pos, const XMFLOAT2& scale, const DirectX::XMVECTOR& color)
{
	m_texts.push_back(ScreenText());
	m_texts.back().m_text = text;
	m_texts.back().m_pos = pos;
	m_texts.back().m_scale = scale;
	m_texts.back().m_color = color;
//	m_texts.emplace_back(ScreenText{ text, pos, scale });
}

void Renderer::AddToBatch(const VerticeBatch* batch, int z)
{
	if (batch != nullptr)
	{
		m_batchesToProcess[z].push_back(batch);
	}
}

Vector2 Renderer::GetViewTopLeft(float offset) const
{
	return Vector2(m_center.x - 0.5f * m_width + offset, m_center.y + 0.5f * m_width * m_aspectRatio - offset);
}

Vector2 Renderer::GetViewTopRight(float offset) const
{
	return Vector2(m_center.x + 0.5f * m_width - offset, m_center.y + 0.5f * m_width * m_aspectRatio - offset);
}

Vector2  Renderer::GetViewCenter() const
{
	return Vector2(m_center.x, m_center.y);
}

void Renderer::Render()
{
	if (!m_ready)
	{
		m_batchesToProcess.clear();
		m_texts.clear();
		return;
	}
	const float midnightBlue[] = { 0.098f, 0.098f, 0.439f, 1.000f };
	const float azure[] = { 0.0f, 0.5f, 1.0f, 1.000f };
	m_d3dContext->ClearRenderTargetView(
		m_renderTarget2.m_renderTargetView.Get(),
		azure
		);

	m_d3dContext->ClearDepthStencilView(
		m_depthStencilView.Get(),
		D3D11_CLEAR_DEPTH,
		1.0f,
		0
		);

	m_d3dContext->OMSetRenderTargets(
		1,
		m_renderTarget2.m_renderTargetView.GetAddressOf(),
		m_depthStencilView.Get()
		);

	auto context = m_d3dContext.Get();
	m_batchEffect->SetProjection(Matrix::CreateOrthographic(m_width, m_aspectRatio * m_width, 0.0f, 100.0f));
	XMFLOAT3 viewExtents(0.5f*m_width, 0.5f*m_aspectRatio*m_width, 0.0f);
	XMVECTOR eye = XMVectorSet(m_center.x, m_center.y, 10.0f, 0.0f);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	m_batchEffect->SetView(Matrix::CreateLookAt(eye, m_center, up));

	context->IASetInputLayout(m_batchInputLayout.Get());
	context->OMSetBlendState(m_states->AlphaBlend(), Colors::White, 0xFFFFFFFF);
	context->OMSetDepthStencilState(m_states->DepthRead(), 0);

	m_batchEffect->Apply(context);

	m_batch->Begin();

	for (auto batchlevel : m_batchesToProcess)
	{
		for (auto batch : batchlevel.second)
		{
			if (!batch->Empty())
			{
				const XMFLOAT2* data_array = batch->Data();
				const uint16_t* index_array = batch->IndexData();
				const tuple<uint16_t, XMFLOAT4>* color_array = batch->ColorData();
				XMMATRIX xf = batch->GetTransform();
				m_batch->DrawIndexed2DXf(
					batch->Type(),
					index_array,
					batch->IndexCount(),
					data_array,
					batch->Size(),
					color_array,
					batch->ColorCount(),
					xf);
			}
		}
	}
	m_batchesToProcess.clear();
	m_batch->End();
	
	m_d3dContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), nullptr);
	m_d3dContext->DiscardView(m_renderTargetView.Get());
	//m_d3dContext->RSSetViewports(1, &fullVP);

	m_spriteBatch->Begin(SpriteSortMode_Immediate, nullptr, nullptr, nullptr, nullptr, [=]
//	m_spriteBatch->Begin(SpriteSortMode_Immediate, states.AlphaBlend(), nullptr, nullptr, nullptr, [=]
	{
		//Custom pixel shader for picture filter
		m_d3dContext->PSSetShader(m_pixelShader.Get(), nullptr, 0);
	});
	m_spriteBatch->Draw(m_renderTarget2.m_shaderResourceView.Get(), XMFLOAT2(0.0f, 0.0f));
	m_spriteBatch->End();
	m_spriteBatch->Begin();
	for (ScreenText& text : m_texts)
	{
		XMFLOAT2 textDimensions;
		textDimensions.x = XMVectorGetX(m_spriteFont->MeasureString(text.m_text));
		textDimensions.y = XMVectorGetY(m_spriteFont->MeasureString(text.m_text));

		XMFLOAT2 textpos = text.m_pos;
		textpos.x -= 0.5f * text.m_scale.x * textDimensions.x;

		const float outlineWidth = 3.0f;
		XMFLOAT2 shadowPos = textpos;
		shadowPos.x -= outlineWidth;
		shadowPos.y -= outlineWidth;
		XMVECTOR shadowColor = DirectX::Colors::Black;
		XMFLOAT2 shadowScale = text.m_scale;
		shadowScale.x *= (textDimensions.x + 2.0f * outlineWidth) / textDimensions.x;
		shadowScale.y *= (textDimensions.y + 2.0f * outlineWidth) / textDimensions.y;
		const XMFLOAT2 origin(0.0f, 0.0f);
		m_spriteFont->DrawString(m_spriteBatch.get(), text.m_text, shadowPos, shadowColor, 0.0f, origin, shadowScale);

	//	textpos = text.m_pos;
	//	textpos.x -= 0.5f * text.m_scale.x * XMVectorGetX(m_spriteFont->MeasureString(text.m_text));
	//	textpos.y -= 0.5f * text.m_scale.y * XMVectorGetY(m_spriteFont->MeasureString(text.m_text));
		m_spriteFont->DrawString(m_spriteBatch.get(), text.m_text, textpos, text.m_color, 0.0f, origin, text.m_scale);
	}
	m_spriteBatch->End();
	m_texts.clear();
	
	ID3D11ShaderResourceView* nullView = nullptr;
	m_d3dContext->PSSetShaderResources(0, 1, &nullView);
}

float Renderer::ApplyCameraDeadZone(float val, float deadzone)
{
	if (val > deadzone)
	{
		return val - deadzone;
	}
	else if (val < -deadzone)
	{
		return val + deadzone;
	}
	else
	{
		return 0.0f;
	}
}

void Renderer::SaveState(Windows::Storage::Streams::DataWriter^ stream) const
{
	stream->WriteSingle(m_center.x);
	stream->WriteSingle(m_center.y);
	stream->WriteSingle(m_width);
	stream->WriteSingle(m_targetTime);
	stream->WriteSingle(m_deadzone);
	stream->WriteSingle(m_targetCenter.x);
	stream->WriteSingle(m_targetCenter.y);
	stream->WriteSingle(m_targetWidth);
}

void Renderer::RestoreState(Windows::Storage::Streams::DataReader^ stream)
{
	m_center.x = stream->ReadSingle();
	m_center.y = stream->ReadSingle();
	m_width = stream->ReadSingle();
	m_targetTime = stream->ReadSingle();
	m_deadzone = stream->ReadSingle();
	m_targetCenter.x = stream->ReadSingle();
	m_targetCenter.y = stream->ReadSingle();
	m_targetWidth = stream->ReadSingle();
}