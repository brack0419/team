#pragma once

#include <memory>
#include <wrl.h>
#include "Graphics/Shader.h"

class LightShaftShader : public SpriteShader
{
	struct CBLightShaft
	{
		int iteration; // レイマーチングの回数
		float intensity; //  明るさの強さ
		float adjustLength; // 距離調整
		UINT dummy1; // アニメみたいに暗くする方向のライトシャフト

		DirectX::XMFLOAT4 viewPosition;

		DirectX::XMFLOAT4X4 inverseViewProjection;

		// 平行光源情報処理
		DirectionalLightData directionalLightData;

		//	シャドウマップ情報
		DirectX::XMFLOAT4X4 lightViewProjection; // ライトビュープロジェクション行列

		DirectX::XMFLOAT3 shadowColor; // 影の色
		float shadowBias; // 深度比較用のオフセット値

		int PCFKernelSize; // ソフトシャドーのサイズ
		DirectX::XMFLOAT3 dummy2;
	};

public:
	LightShaftShader(ID3D11Device* device);
	~LightShaftShader() override {}

	void Begin(const RenderContext& rc) override;
	void Draw(const RenderContext& rc, const Sprite* sprite) override;
	void End(const RenderContext& rc) override;

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer>			lsConstantBuffer;

	Microsoft::WRL::ComPtr<ID3D11VertexShader>		vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>		pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>		inputLayout;

	Microsoft::WRL::ComPtr<ID3D11BlendState>		blendState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>	rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>	depthStencilState;

	Microsoft::WRL::ComPtr<ID3D11SamplerState>		samplerState;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>		shadowMapSamplerState;
};
