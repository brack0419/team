#pragma once

#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>

class RenderTarget
{
protected:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ShaderResourceView = nullptr;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> RenderTargetView = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> DepthStencilView = nullptr;

	// テクスチャ情報
	D3D11_TEXTURE2D_DESC texture2d_desc;

public:
	RenderTarget::RenderTarget();
	RenderTarget::RenderTarget(u_int width, u_int height, DXGI_FORMAT format);
	RenderTarget::RenderTarget(u_int width, u_int height, u_int level, DXGI_FORMAT format);

	virtual ~RenderTarget()
	{
	}

	UINT GetTextureWidth() const { return texture2d_desc.Width; }
	UINT GetTextureHeight() const { return texture2d_desc.Height; }

	bool Create(u_int width, u_int height, DXGI_FORMAT format);
	bool CreateMipMap(u_int width, u_int height, u_int level, DXGI_FORMAT format);
	bool CreateDepth(u_int width, u_int height);

	bool CreateCubeMap(u_int width, u_int height, u_int level, DXGI_FORMAT format);
	bool CreateCubeDepthStencil(u_int width, u_int height);

	ID3D11RenderTargetView* GetRenderTargetView() const { return RenderTargetView.Get(); }
	ID3D11DepthStencilView* GetDepthStencilView() const { return DepthStencilView.Get(); }

	ID3D11ShaderResourceView* GetShaderResourceView() const { return ShaderResourceView.Get(); }
};
