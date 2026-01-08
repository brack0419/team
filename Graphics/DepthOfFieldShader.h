#pragma once

#include <memory>
#include <wrl.h>
#include "Graphics/Shader.h"

class DepthOfFieldShader : public SpriteShader
{
	struct CBDepthOfField
	{
		float focusDistance; //	フォーカス距離
		float dofRange; //	焦点範囲
		float near_clip; // ニアクリップ面
		float far_clip; // ファークリップ面
	};

public:
	DepthOfFieldShader(ID3D11Device* device);
	~DepthOfFieldShader() override {}

	void Begin(const RenderContext& rc) override;
	void Draw(const RenderContext& rc, const Sprite* sprite) override;
	void End(const RenderContext& rc) override;

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer>				dofConstantBuffer;

	Microsoft::WRL::ComPtr<ID3D11VertexShader>			vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>			pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>			inputLayout;

	Microsoft::WRL::ComPtr<ID3D11BlendState>			blendState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>		rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>		depthStencilState;

	Microsoft::WRL::ComPtr<ID3D11SamplerState>			samplerState;
};
