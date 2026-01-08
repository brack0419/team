#include "Misc.h"

#include "Graphics/RenderTarget.h"
#include "Graphics/Graphics.h"

RenderTarget::RenderTarget()
{
}

RenderTarget::RenderTarget(u_int width, u_int height, DXGI_FORMAT format)
{
	Create(width, height, format);
	CreateDepth(width, height);
}

RenderTarget::RenderTarget(u_int width, u_int height, u_int level, DXGI_FORMAT format)
{
	CreateMipMap(width, height, level, format);
	CreateDepth(width, height);
}

bool RenderTarget::Create(u_int width, u_int height, DXGI_FORMAT format)
{
	ID3D11Device* device = Graphics::Instance().GetDevice();

	Microsoft::WRL::ComPtr<ID3D11Texture2D> Texture2D;
	HRESULT hr = S_OK;
	//	テクスチャ作成
	ZeroMemory(&texture2d_desc, sizeof(texture2d_desc));
	texture2d_desc.Width = width;
	texture2d_desc.Height = height;
	texture2d_desc.MipLevels = 1;
	texture2d_desc.ArraySize = 1;
	texture2d_desc.Format = format;
	texture2d_desc.SampleDesc.Count = 1;
	texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
	texture2d_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	texture2d_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	hr = device->CreateTexture2D(&texture2d_desc, NULL, Texture2D.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	//	レンダーターゲットビュー作成
	D3D11_RENDER_TARGET_VIEW_DESC rtvd;
	ZeroMemory(&rtvd, sizeof(rtvd));
	rtvd.Format = format;
	rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvd.Texture2D.MipSlice = 0;
	hr = device->CreateRenderTargetView(Texture2D.Get(), &rtvd, RenderTargetView.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	//	シェーダーリソースビュー作成
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(srvd));
	srvd.Format = format;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MostDetailedMip = 0;
	srvd.Texture2D.MipLevels = 1;
	hr = device->CreateShaderResourceView(Texture2D.Get(), &srvd, ShaderResourceView.GetAddressOf());

	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	return true;
}
bool RenderTarget::CreateMipMap(u_int width, u_int height, u_int level, DXGI_FORMAT format)
{
	ID3D11Device* device = Graphics::Instance().GetDevice();

	Microsoft::WRL::ComPtr<ID3D11Texture2D> Texture2D;
	HRESULT hr = S_OK;
	//	テクスチャ作成
	ZeroMemory(&texture2d_desc, sizeof(texture2d_desc));
	texture2d_desc.Width = width;
	texture2d_desc.Height = height;
	texture2d_desc.MipLevels = level;
	texture2d_desc.ArraySize = 1;
	texture2d_desc.Format = format;
	texture2d_desc.SampleDesc.Count = 1;
	texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
	texture2d_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	texture2d_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texture2d_desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	hr = device->CreateTexture2D(&texture2d_desc, NULL, Texture2D.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	//	レンダーターゲットビュー作成
	D3D11_RENDER_TARGET_VIEW_DESC rtvd;
	ZeroMemory(&rtvd, sizeof(rtvd));
	rtvd.Format = format;
	rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvd.Texture2D.MipSlice = 0;
	hr = device->CreateRenderTargetView(Texture2D.Get(), &rtvd, RenderTargetView.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	//	シェーダーリソースビュー作成
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(srvd));
	srvd.Format = format;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MostDetailedMip = 0;
	srvd.Texture2D.MipLevels = level;
	hr = device->CreateShaderResourceView(Texture2D.Get(), &srvd, ShaderResourceView.GetAddressOf());

	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	return true;
}
bool RenderTarget::CreateCubeMap(u_int width, u_int height, u_int level, DXGI_FORMAT format)
{
	ID3D11Device* device = Graphics::Instance().GetDevice();

	Microsoft::WRL::ComPtr<ID3D11Texture2D> Texture2D;

	HRESULT hr = S_OK;
	//	テクスチャ作成
	ZeroMemory(&texture2d_desc, sizeof(texture2d_desc));
	texture2d_desc.Width = width;
	texture2d_desc.Height = height;
	texture2d_desc.MipLevels = level;
	texture2d_desc.ArraySize = 6;
	texture2d_desc.Format = format;
	texture2d_desc.SampleDesc.Count = 1;
	texture2d_desc.SampleDesc.Quality = 0;
	texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
	texture2d_desc.CPUAccessFlags = 0;
	texture2d_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texture2d_desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS |
		D3D11_RESOURCE_MISC_TEXTURECUBE;

	hr = device->CreateTexture2D(&texture2d_desc, NULL, Texture2D.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	//	シェーダーリソースビュー作成
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(srvd));
	srvd.Format = format;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvd.TextureCube.MostDetailedMip = 0;
	srvd.TextureCube.MipLevels = level;
	hr = device->CreateShaderResourceView(Texture2D.Get(), &srvd, ShaderResourceView.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	//	レンダーターゲットビュー作成
	D3D11_RENDER_TARGET_VIEW_DESC rtvd;
	ZeroMemory(&rtvd, sizeof(rtvd));
	rtvd.Format = format;
	rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;//2Dテクスチャ配列
	rtvd.Texture2DArray.FirstArraySlice = 0;
	rtvd.Texture2DArray.MipSlice = 0;
	rtvd.Texture2DArray.ArraySize = 6;
	hr = device->CreateRenderTargetView(Texture2D.Get(), &rtvd, RenderTargetView.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	return true;
}

bool RenderTarget::CreateDepth(u_int width, u_int height)
{
	ID3D11Device* device = Graphics::Instance().GetDevice();

	Microsoft::WRL::ComPtr<ID3D11Texture2D> Texture2D;
	HRESULT hr = S_OK;
	//	テクスチャ作成
	ZeroMemory(&texture2d_desc, sizeof(texture2d_desc));
	texture2d_desc.Width = width;
	texture2d_desc.Height = height;
	texture2d_desc.MipLevels = 1;
	texture2d_desc.ArraySize = 1;
	texture2d_desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	texture2d_desc.SampleDesc.Count = 1;
	texture2d_desc.SampleDesc.Quality = 0;
	texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
	texture2d_desc.CPUAccessFlags = 0;
	texture2d_desc.MiscFlags = 0;
	texture2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

	hr = device->CreateTexture2D(&texture2d_desc, NULL, Texture2D.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	// 深度ステンシルビュー設定
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory(&dsvd, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	dsvd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvd.Texture2D.MipSlice = 0;
	hr = device->CreateDepthStencilView(Texture2D.Get(), &dsvd, DepthStencilView.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	return true;
}
bool RenderTarget::CreateCubeDepthStencil(u_int width, u_int height)
{
	ID3D11Device* device = Graphics::Instance().GetDevice();

	Microsoft::WRL::ComPtr<ID3D11Texture2D> Texture2D;
	HRESULT hr = S_OK;
	//	テクスチャ作成
	ZeroMemory(&texture2d_desc, sizeof(texture2d_desc));
	texture2d_desc.Width = width;
	texture2d_desc.Height = height;
	texture2d_desc.MipLevels = 1;
	texture2d_desc.ArraySize = 6;
	texture2d_desc.Format = DXGI_FORMAT_D32_FLOAT;
	texture2d_desc.SampleDesc.Count = 1;
	texture2d_desc.SampleDesc.Quality = 0;
	texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
	texture2d_desc.CPUAccessFlags = 0;

	texture2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	texture2d_desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	hr = device->CreateTexture2D(&texture2d_desc, NULL, Texture2D.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	//	デプスステンシルビュー作成
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory(&dsvd, sizeof(dsvd));
	dsvd.Format = DXGI_FORMAT_D32_FLOAT;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;//2Dテクスチャ配列
	dsvd.Texture2DArray.FirstArraySlice = 0;
	dsvd.Texture2DArray.ArraySize = 6;
	dsvd.Texture2DArray.MipSlice = 0;
	dsvd.Flags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	hr = device->CreateDepthStencilView(Texture2D.Get(), NULL, DepthStencilView.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	return true;
}