#pragma once

#include <memory>
#include <wrl.h>
#include "Graphics/Shader.h"

class MaskShader : public SpriteShader
{
public:
	MaskShader(ID3D11Device* device);
	~MaskShader() override {}

	//void Begin(ID3D11DeviceContext* dc, const RenderContext& rc) override;
	//void Draw(ID3D11DeviceContext* dc, const Sprite* sprite) override;
	//void End(ID3D11DeviceContext* dc) override;
	void Begin(const RenderContext& rc) override;
	void Draw(const RenderContext& rc, const Sprite* sprite) override;
	void End(const RenderContext& rc) override;

private:

	struct CbScroll
	{
		DirectX::XMFLOAT2	uvScrollValue;	//	UVスクロール値
		DirectX::XMFLOAT2	dummy;			//	ダミー
	};

	// 閾値を送るための定数バッファ用構造体
	struct CbMask
	{
		float				dissolveThreshold;	// ディゾルブ量
		float edgeThreshold; // 縁の閾値
		DirectX::XMFLOAT2	dummy;
		DirectX::XMFLOAT4 edgeColor; // 縁の色
	};

	Microsoft::WRL::ComPtr<ID3D11Buffer>				maskConstantBuffer; 	// 定数バッファ

	Microsoft::WRL::ComPtr<ID3D11Buffer>				scrollConstantBuffer;

	Microsoft::WRL::ComPtr<ID3D11VertexShader>			vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>			pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>			inputLayout;

	Microsoft::WRL::ComPtr<ID3D11BlendState>			blendState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>		rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>		depthStencilState;

	Microsoft::WRL::ComPtr<ID3D11SamplerState>			samplerState;
};
