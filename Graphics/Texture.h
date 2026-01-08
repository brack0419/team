#pragma once
#include <wrl.h>
#include <d3d11.h>

// テクスチャ
class Texture
{
public:
	Texture(const char* filename);
	~Texture() {}

	// シェーダーリソースビュー取得
	inline const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetShaderResourceView() const { return shaderResourceView; }

	// テクスチャの情報取得
	inline const D3D11_TEXTURE2D_DESC& GetTexture2dDesc() const { return texture2dDesc; }

	// テクスチャ幅取得
	inline int GetWidth() const { return texture2dDesc.Width; }

	// テクスチャ高さ取得
	inline int GetHeight() const { return texture2dDesc.Height; }

	// テクスチャ読み込み関数
	static HRESULT LoadTexture(ID3D11Device* device, const char* filename, ID3D11ShaderResourceView** shaderResourceView, D3D11_TEXTURE2D_DESC* texture2dDesc);

private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	shaderResourceView;
	D3D11_TEXTURE2D_DESC	texture2dDesc;
};
