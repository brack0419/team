#pragma once

#include "Graphics/Sprite.h"
#include "Graphics/Texture.h"

#include "Scene.h"

// タイトルシーン
class SceneTitle : public Scene
{
public:
	SceneTitle() {}
	~SceneTitle() override {}

	// 初期化
	void Initialize() override;

	// 終了化
	void Finalize() override;

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Render() override;

private:
	//Sprite* sprite = nullptr;
	std::unique_ptr<Sprite> sprite;

	//Texture* texture = nullptr;
	std::unique_ptr<Texture> texture;
	// UVスクロールデータ
	UVScrollData			uvScrollData;

	// マスク画像
	std::unique_ptr<Texture>	maskTexture;
	float					dissolveThreshold;
	float					edgeThreshold; 	// 縁の閾値
	DirectX::XMFLOAT4			edgeColor;		// 縁の色
};
