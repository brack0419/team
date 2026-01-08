#pragma once

#include "Graphics/Shader.h"
#include "Character.h"

// エネミー
class Enemy : public Character
{
public:
	Enemy() {}
	~Enemy() override {}

	// 更新処理
	virtual void Update(float elapsedTime) = 0;

	// 描画処理
	//virtual void Render(ID3D11DeviceContext* dc, ModelShader* shader) = 0;
	virtual void Render(const RenderContext& rc, ModelShader* shader) = 0;

	virtual DirectX::XMFLOAT3 GetMoveVec()const;

	// デバッグプリミティブ描画
	virtual void DrawDebugPrimitive();

	// デバッグ用GUI描画
	virtual void DrawDebugGUI();

	// 破棄
	void Destroy();
};
