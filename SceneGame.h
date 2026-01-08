#pragma once

#include "Stage.h"
#include "Player.h"
#include "CameraController.h"
#include "Scene.h"
#include "Graphics/RenderTarget.h"
#include "Graphics/DepthStencil.h"
#include "Light.h"
#include "Graphics/SkyBox.h"
//#include "PostprocessingRenderer.h"
//#include"Effect.h"
#include"ParticleSystem.h"
#include "Graphics/Texture.h"

// ゲームシーン
class SceneGame : public Scene
{
public:
	SceneGame() {}
	~SceneGame() override {}

	// 初期化
	void Initialize() override;

	// 終了化
	void Finalize() override;

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Render() override;

private:

	// 3D空間の描画
	void Render3DScene();

	// シャドウマップの描画
	void RenderShadowmap();

	// ポストプロセス描画処理
	void RenderPostprocess();

	//	テキトーにクリア関数
	void ClearRenderTargetSlots();
	void ClearConstantBuffers(int start_slot = 0, int num = D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT);
	void ClearShaderResorceViews(int start_slot = 0, int num = D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT);
	void ClearUnorderedAccessViews(int start_slot = 0, int num = 8);
	void ClearSampler(int start_slot = 0, int num = D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT);

private:
	Stage* stage = nullptr;

	//std::unique_ptr<StageManager> stageManager;
	/*std::unique_ptr<Stage> earth;
	std::unique_ptr<Earth> earth2;*/

	Stage* earth[8];

	Player* player = nullptr;
	CameraController* cameraController = nullptr;
	//// 平行光源データ
	//std::unique_ptr<Light>	directional_light;
	//DirectX::XMFLOAT4		ambientLightColor;

	float   outlinewidth = 0.0f;
	DirectX::XMFLOAT4  outlineColor{ 0,0,0,1 };
	SkyBox* skybox = nullptr;//初期値null

	//	シャドウマップ用情報
	Light* mainDirectionalLight = nullptr;					//	シャドウマップを生成する平行光源
	std::unique_ptr<DepthStencil> shadowmapDepthStencil;	//	シャドウマップ用深度ステンシルバッファ
	float	shadowDrawRect = 150.0f;	//	シャドウマップに描画する範囲
	DirectX::XMFLOAT4X4	lightViewProjection;				//	ライトビュープロジェクション行列
	DirectX::XMFLOAT3	shadowColor = { 0.2f, 0.2f, 0.2f };	//	影の色
	float				shadowBias = 0.00015f;				//	深度比較用のオフセット値
	int                 PCFKernelSize = 15;                  // ソフトシャドーの行列サイズ
	int                 sceneBlurSize = 0;					//	シーンぼかし量

	std::unique_ptr<cParticleSystem>bomb_particle;
	std::unique_ptr<Texture>bomb_texture;

	std::unique_ptr<cParticleSystem>particle;
	std::unique_ptr<Texture>texture;
	//	ポストプロセス
	//std::unique_ptr<PostprocessingRenderer>	postprocessingRenderer;
	std::unique_ptr<RenderTarget>offscreen;
	std::unique_ptr<Sprite>screen_sprite;

	std::unique_ptr<RenderTarget>	workRenderTargets[2];	//	作業用の描画バッファ
	LuminanceExtractionData			luminanceExtractionData;//	明るい所の抽出情報
	int								bloomBlurSize = 10;		//	ブルーム用のぼかし量

	int								depthOfFieldBlurSize = 10;	//	被写界深度用のぼかし量
	std::unique_ptr<DepthStencil>	sceneDepthStencil;	//	シーン深度バッファ
	float							focusDistance = 15.0f;	//	被写体距離
	float							dofRange = 20.0f;	//	ボケていく範囲

	//	IBLテクスチャ
	std::unique_ptr<Texture>		iblDiffuseTexture;
	std::unique_ptr<Texture>		iblSpecularTexture;
	std::unique_ptr<Texture>		iblGGXLutTexture;

	//	ライトシャフト
	int								lightShaftIteration = 24; // レイマーチングの回数
	float							lightShaftIntensity = 0.5f; //  明るさの強さ
	float							lightShaftAdjustLength = 7.5f;	// 距離調整
};
