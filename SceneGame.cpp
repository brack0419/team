#include "Graphics/Graphics.h"
#include "SceneGame.h"
#include "Camera.h"
#include "EnemyManager.h"
#include "EnemySlime.h"
#include "EffectManager.h"
#include "LightManager.h"
#include "StageManager.h"

//	シャドウマップのサイズ
static	const	UINT	SHADOWMAP_SIZE = 2048;

// 初期化
void SceneGame::Initialize()
{
	// ステージ初期化
	//stageManager = std::make_unique<StageManager>();

	//stage = new Stage();

	StageManager::Instance().Register(new Stage);

	{
		earth[0] = new Earth;
		earth[0]->SetScale({ 0.05f,0.05f,0.05f });
		earth[0]->SetPosition({ 0.0f,3.0f,-10.0f });
		StageManager::Instance().Register(earth[0]);
	}
	//{
	//	earth[1] = std::make_unique<Earth>();
	//	earth[1]->SetScale({ 0.1f,0.1f,0.1f });
	//	earth[1]->SetPosition({ 4.0f,5.0f,-5.0f });
	//	StageManager::Instance().Register(earth[1].get());
	//}
	//{
	//	earth[2] = std::make_unique<Earth>();
	//	earth[2]->SetScale({ 0.05f,0.05f,0.05f });
	//	earth[2]->SetPosition({ 0.0f,3.0f,-10.0f });
	//	StageManager::Instance().Register(earth[2].get());
	//}
	//{
	//	earth[3] = std::make_unique<Earth>();
	//	earth[3]->SetScale({ 0.05f,0.05f,0.05f });
	//	earth[3]->SetPosition({ -4.0f,2.0f,5.0f });
	//	StageManager::Instance().Register(earth[3].get());
	//}
	//{
	//	earth[4] = std::make_unique<Earth>();
	//	earth[4]->SetScale({ 0.05f,0.05f,0.05f });
	//	earth[4]->SetPosition({ 0.0f,3.0f,-10.0f });
	//	StageManager::Instance().Register(earth[4].get());
	//}
	//{
	//	earth[5] = std::make_unique<Earth>();
	//	earth[5]->SetScale({ 0.1f,0.1f,0.1f });
	//	earth[5]->SetPosition({ 0.0f,5.0f,15.0f });
	//	StageManager::Instance().Register(earth[5].get());
	//}

	// プレイヤー
	player = new Player();
	//player->SetPosition({10,10,0});

	// カメラ初期設定
	Graphics& graphics = Graphics::Instance();
	Camera& camera = Camera::Instance();
	camera.SetLookAt(
		DirectX::XMFLOAT3(0, -10, 10),
		DirectX::XMFLOAT3(0, 0, 0),
		DirectX::XMFLOAT3(0, 1, 0)
	);
	camera.SetPerspectiveFov(
		DirectX::XMConvertToRadians(45),
		graphics.GetScreenWidth() / graphics.GetScreenHeight(),
		0.1f,
		1000.0f
	);

	// カメラコントローラー初期化
	cameraController = new CameraController();
	// 平行光源を追加
	//LightManager::Instance().Register(new Light(LightType::Directional));

	{
		mainDirectionalLight = new Light(LightType::Directional);
		mainDirectionalLight->SetDirection({ 1, -1, -1 });
		LightManager::Instance().Register(mainDirectionalLight);
	}

	// 点光源を追加
	{
		Light* light = new Light(LightType::Point);
		light->SetPosition(DirectX::XMFLOAT3(0, 4, 0));
		light->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
		light->SetRange(5.0f);
		LightManager::Instance().Register(light);
	}
	// 点光源を追加
	{
		Light* light = new Light(LightType::Point);
		light->SetPosition(DirectX::XMFLOAT3(5, 4, 0));
		light->SetColor(DirectX::XMFLOAT4(1, 1, 0, 1));
		light->SetRange(5.0f);
		LightManager::Instance().Register(light);
	}
	// スポットライトを追加
	{
		Light* light = new Light(LightType::Spot);
		light->SetPosition(DirectX::XMFLOAT3(-10, 10, 10));
		light->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
		light->SetDirection(DirectX::XMFLOAT3(+1, -1, 0));

		light->SetRange(40.0f);
		LightManager::Instance().Register(light);
	}

	// シャドウマップ用に深度ステンシルの生成
	{
		Graphics& graphics = Graphics::Instance();
		shadowmapDepthStencil = std::make_unique<DepthStencil>(SHADOWMAP_SIZE, SHADOWMAP_SIZE);
	}

	//RenderTarget
	{
		Graphics& graphics = Graphics::Instance();
		offscreen = std::make_unique<RenderTarget>(static_cast<u_int>(graphics.GetScreenWidth()),
			static_cast<u_int>(graphics.GetScreenHeight()), DXGI_FORMAT_R8G8B8A8_UNORM);
	}

	// エネミー初期化
	EnemyManager& enemyManager = EnemyManager::Instance();
	for (int i = 0; i < 0; ++i)
	{
		EnemySlime* slime = new EnemySlime();
		slime->SetPosition(DirectX::XMFLOAT3(i * 2.0f, 0, 5));
		enemyManager.Register(slime);
	}

	//スカイボックス
	skybox = new SkyBox(std::make_shared<Texture>("Data/SkyBox/field20220706.jpg"));

	//パーティクル初期化
	{
		bomb_texture = std::make_unique<Texture>("Data/Effect/Texture/Explosion01_5x5.png");
		bomb_particle = std::make_unique<cParticleSystem>(bomb_texture->GetShaderResourceView().Get(), 5, 5, true);//trueはアニメーションかどうか
	}
	{
		texture = std::make_unique<Texture>("Data/Effect/Texture/particle256x256.png");//4,4は分割
		particle = std::make_unique<cParticleSystem>(texture->GetShaderResourceView().Get(), 4, 4, false, 10000);//10000は粒の量でデフォが1000
	}

	//シーンのスプライト
	screen_sprite = std::make_unique<Sprite>();

	// ポストエフェクトように一時バッファを用意する
	{
		UINT width = static_cast<UINT>(graphics.GetScreenWidth());
		UINT height = static_cast<UINT>(graphics.GetScreenHeight());

		workRenderTargets[0] = std::make_unique<RenderTarget>(width, height,
			DXGI_FORMAT_R8G8B8A8_UNORM);
		workRenderTargets[1] = std::make_unique<RenderTarget>(width, height,
			DXGI_FORMAT_R8G8B8A8_UNORM);
		sceneDepthStencil = std::make_unique<DepthStencil>(width, height);	//	深度バッファを生成
	}

	//	IBL用テクスチャ取得
	{
		iblDiffuseTexture = std::make_unique<Texture>("Data/Skybox/sunset_jhbcentral_4k/diffuse_iem.dds");
		iblSpecularTexture = std::make_unique<Texture>("Data/Skybox/sunset_jhbcentral_4k/specular_pmrem.dds");
		iblGGXLutTexture = std::make_unique<Texture>("Data/Skybox/lut_ggx.dds");
	}
}

// 終了化
void SceneGame::Finalize()
{
	// エネミー終了化
	EnemyManager::Instance().Clear();

	// ステージ終了化
	StageManager::Instance().Clear();//無いとリリーク起こす

	// カメラコントローラー終了化
	if (cameraController != nullptr)
	{
		delete cameraController;
		cameraController = nullptr;
	}

	// プレイヤー終了化
	if (player != nullptr)
	{
		delete player;
		player = nullptr;
	}

	// ステージ終了化
	if (stage != nullptr)
	{
		delete stage;
		stage = nullptr;
	}

	delete skybox;
}

// 更新処理
void SceneGame::Update(float elapsedTime)
{
	// カメラコントローラー更新処理
	DirectX::XMFLOAT3 target = player->GetPosition();
	target.y += 0.5f;
	cameraController->SetTarget(target);
	cameraController->Update(elapsedTime);

	// ステージ更新処理

	StageManager::Instance().Update(elapsedTime);

	// プレイヤー更新処理
	player->Update(elapsedTime);

	// エネミー更新処理
	EnemyManager::Instance().Update(elapsedTime);

	// エフェクト更新処理
	EffectManager::Instance().Update(elapsedTime);

	// ライト更新処理(仮)
	Light* light = LightManager::Instance().GetLight(3);
	DirectX::XMFLOAT3 p = player->GetPosition();
	p.y += 1.5f;
	light->SetPosition(p);
	DirectX::XMFLOAT3 d = player->GetDirection();
	d.y -= 0.2f;
	light->SetDirection(d);

	if (GetAsyncKeyState('Q') & 1) {
		DirectX::XMFLOAT3 p = player->GetPosition();
		p.y += 2.5f;
		p.x += (rand() % 201 - 100) * 0.01f;
		p.y += (rand() % 201 - 100) * 0.01f;
		p.z += (rand() % 201 - 100) * 0.01f;

		//typeは何分割目の画像を選んでいるか。この場合0番目それが進んでいく
		//120はループできる秒数。1周したら消えるため120もかからないが1秒にするとアニメーションが途中で終わる
		//pは発生する場所
		//vは移動方向。真上に発生する
		//fは下向きに力が働く=重力のような物
		//最後が元画像の三倍サイズ
		bomb_particle->Set(0, 120, true, p, { 0,2.0f,0 }, { 0.0f,-1.3f,0 }, { 3,3 });
	}

	if (GetAsyncKeyState('P') & 1) {
		DirectX::XMFLOAT3 playerPos = player->GetPosition(); // プレイヤーの位置を取得
		const int heightSteps = 10;       // 竜巻を分割する高さの段階
		const int baseParticles = 2;      // 各段階の最低パーティクル数
		const int maxParticles = 20;      // 各段階の最大パーティクル数
		const float baseRadius = 1.0f;    // 渦の基底半径
		const float height = 10.0f;       // 竜巻の高さ
		const float angularSpeed = 2.0f;  // 渦巻きの回転速度
		const float liftSpeed = 2.0f;     // 上昇速度
		const float expansionRate = 0.2f; // 半径の広がり率
		const float duration = 5.0f;      // パーティクルの寿命

		// 高さごとにパーティクルを生成
		for (int step = 0; step < heightSteps; step++) {
			float currentHeight = height * (step / static_cast<float>(heightSteps)); // 現在の高さ
			int particles = baseParticles + (maxParticles - baseParticles) * (step / static_cast<float>(heightSteps)); // 段階的なパーティクル数

			for (int i = 0; i < particles; i++) {
				// ランダムな角度と半径を設定
				float angle = static_cast<float>(rand()) / RAND_MAX * DirectX::XM_2PI; // 0 ～ 2πのランダムな角度
				float r = baseRadius + currentHeight * expansionRate;                  // 高さに応じた半径

				// パーティクルの初期位置
				DirectX::XMFLOAT3 p = playerPos;
				p.x += r * cos(angle); // 渦の円周方向
				p.z += r * sin(angle); // 渦の奥行き方向
				p.y += currentHeight;  // 渦の高さ方向

				// パーティクルの速度を設定 (らせん状の動き)
				DirectX::XMFLOAT3 v;
				v.x = -angularSpeed * sin(angle);      // 回転速度 (X軸)
				v.z = angularSpeed * cos(angle);       // 回転速度 (Z軸)
				v.y = liftSpeed + (currentHeight / height) * 0.5f; // 上昇速度 (Y軸)

				// パーティクルの生成
				particle->Set(
					rand() % 4 + 8,           // ランダムな画像タイプ (0～3)
					duration,             // パーティクルの寿命
					false,                // ループ再生しない
					p,                    // パーティクルの初期位置
					v,                    // パーティクルの速度
					{ 0.0f, 0.0f, 0.0f },  // 重力 (必要に応じて調整)
					{ 0.3f, 0.3f }          // パーティクルのサイズ
				);
			}
		}
	}

	//パーティクルの更新
	bomb_particle->Update(elapsedTime);
	particle->Update(elapsedTime);
}

//	シャドウマップの描画
void SceneGame::RenderShadowmap()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = nullptr;
	ID3D11DepthStencilView* dsv = shadowmapDepthStencil->GetDepthStencilView().Get();

	// 画面クリア
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	if (!mainDirectionalLight)
		return;

	// レンダーターゲット設定
	dc->OMSetRenderTargets(0, &rtv, dsv);

	// ビューポートの設定
	D3D11_VIEWPORT	vp = {};
	vp.Width = static_cast<float>(shadowmapDepthStencil->GetWidth());
	vp.Height = static_cast<float>(shadowmapDepthStencil->GetHeight());
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	dc->RSSetViewports(1, &vp);

	// 描画処理
	RenderContext rc;
	rc.deviceContext = dc;

	// カメラパラメータ設定
	{
		// 平行光源からカメラ位置を作成し、そこから原点の位置を見るように視線行列を生成
		DirectX::XMVECTOR LightPosition = DirectX::XMLoadFloat3(&mainDirectionalLight->GetDirection());
		LightPosition = DirectX::XMVectorScale(LightPosition, -250.0f);//カメラ250上空
		DirectX::XMMATRIX V = DirectX::XMMatrixLookAtLH(LightPosition,
			DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
			DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

		// シャドウマップに描画したい範囲の射影行列を生成
		DirectX::XMMATRIX P = DirectX::XMMatrixOrthographicLH(shadowDrawRect, shadowDrawRect, 0.1f, 1000.0f);//遠すぎる可能性1000
		DirectX::XMStoreFloat4x4(&rc.view, V);
		DirectX::XMStoreFloat4x4(&rc.projection, P);
		DirectX::XMStoreFloat4x4(&lightViewProjection, V * P);
	}

	// 3Dモデル描画
	{
		ModelShader* shader = graphics.GetShader(ModelShaderId::ShadowmapCaster);
		shader->Begin(rc);

		// ステージ描画
		StageManager::Instance().Render(rc, shader);
		//stage->Render(rc, shader);
		//earth->Render(rc, shader);

		// プレイヤー描画
		player->Render(rc, shader);

		// エネミー描画
		EnemyManager::Instance().Render(rc, shader);

		shader->End(rc);
	}

	ClearConstantBuffers();
	ClearShaderResorceViews();
}

//3D空間の描画
void SceneGame::Render3DScene()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = offscreen->GetRenderTargetView();
	ID3D11DepthStencilView* dsv = sceneDepthStencil->GetDepthStencilView().Get();

	// 画面クリア＆レンダーターゲット設定
	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0～1.0)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);

	// 描画処理
	RenderContext rc;
	rc.lightDirection = { 0.0f, -1.0f, 0.0f, 0.0f };	// ライト方向（下方向）
	// ライトの情報を詰め込む
	LightManager::Instance().PushRenderContext(rc);

	// シャドウマップの設定
	rc.shadowMapData.shadowMap = shadowmapDepthStencil->GetShaderResourceView().Get();
	rc.shadowMapData.lightViewProjection = lightViewProjection;
	rc.shadowMapData.shadowColor = shadowColor;
	rc.shadowMapData.shadowBias = shadowBias;
	rc.shadowMapData.PCFKernelSize = PCFKernelSize;

	// IBLテクスチャ設定
	rc.iblData.diffuseIrradianceEnvironmentMap = iblDiffuseTexture->GetShaderResourceView().Get();
	rc.iblData.specularPremappingRadianceEnvironmentMap = iblSpecularTexture->GetShaderResourceView().Get();
	rc.iblData.ggxLookUpTableMap = iblGGXLutTexture->GetShaderResourceView().Get();

	// ビューポートの設定
	D3D11_VIEWPORT	vp = {};
	//vp.Width = graphics.GetScreenWidth();
	//vp.Height = graphics.GetScreenHeight();
	vp.Width = offscreen->GetTextureWidth();
	vp.Height = offscreen->GetTextureHeight();
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	dc->RSSetViewports(1, &vp);

	// カメラパラメータ設定
	Camera& camera = Camera::Instance();
	rc.viewPosition.x = camera.GetEye().x;
	rc.viewPosition.y = camera.GetEye().y;
	rc.viewPosition.z = camera.GetEye().z;
	rc.viewPosition.w = 1;
	rc.view = camera.GetView();
	rc.projection = camera.GetProjection();
	rc.outlinedata.outlineColor = outlineColor;
	rc.outlinedata.outlineSize = outlinewidth;
	rc.deviceContext = dc;

	// 空描画
	{
		skybox->Render(dc, rc);
	}

	// 3Dモデル描画
	{
		//Shader* shader = graphics.GetShader();
		//ModelShader* shader = graphics.GetShader(ModelShaderId::Default);
		if (ModelShader* shader = graphics.GetShader(ModelShaderId::Phong)) {
			shader->Begin(rc);
			// ステージ描画
			StageManager::Instance().Render(rc, shader);
			shader->End(rc);
		}
		if (ModelShader* shader = graphics.GetShader(ModelShaderId::PBR)) {
			shader->Begin(rc);
			// プレイヤー描画
			player->Render(rc, shader);
			// エネミー描画
			EnemyManager::Instance().Render(rc, shader);
			shader->End(rc);
		}
	}

	// 3Dエフェクト描画
	{
		EffectManager::Instance().Render(rc.view, rc.projection);
		bomb_particle->Render(dc, rc.view, rc.projection, 1);
		particle->Render(dc, rc.view, rc.projection, 1);//1は加算ブレンドで明るくなる。2は減算ブレンドで暗くなる。何もなしはαブレンドでそのままの色
	}

	ClearConstantBuffers();
	ClearShaderResorceViews();
}

void SceneGame::RenderPostprocess()
{
#if 01
	Camera& camera = Camera::Instance();
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

	// 画面クリア＆レンダーターゲット設定
	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0～1.0)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);

	//// 描画処理
	RenderContext rc;

	// ビューポートの設定
	D3D11_VIEWPORT	vp = {};
	vp.Width = graphics.GetScreenWidth();
	vp.Height = graphics.GetScreenHeight();
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	dc->RSSetViewports(1, &vp);

	rc.deviceContext = dc;
#endif

	// ブルーム処理
	{
		static const int LuminanceExtractionIndex = 0;
		static const int GaussianFilterIndex = 1;

		// 描画コンテキスト設定
		rc.luminanceExtractionData = luminanceExtractionData;
		rc.gaussianFilterData.kernelSize = bloomBlurSize;
		rc.gaussianFilterData.textureSize.x = workRenderTargets[0]->GetTextureWidth();
		rc.gaussianFilterData.textureSize.y = workRenderTargets[0]->GetTextureHeight();

		// ①明るい部分を抜き出す処理
		{
			//	描画先の変更
			ID3D11RenderTargetView* rtvs;
			rtvs = workRenderTargets[LuminanceExtractionIndex]->GetRenderTargetView();
			FLOAT clear_color[4] = { 0,0,0,0 };
			rc.deviceContext->ClearRenderTargetView(rtvs, clear_color);
			rc.deviceContext->OMSetRenderTargets(1, &rtvs, nullptr);

			//	ビューポートの設定を変える
			D3D11_VIEWPORT	vp = {};
			vp.Width = workRenderTargets[LuminanceExtractionIndex]->GetTextureWidth();
			vp.Height = workRenderTargets[LuminanceExtractionIndex]->GetTextureHeight();
			vp.MinDepth = 0.0f;
			vp.MaxDepth = 1.0f;
			dc->RSSetViewports(1, &vp);

			//	高輝度部分を抽出するシェーダーを用いる
			SpriteShader* shader = graphics.GetShader(SpriteShaderId::LuminanceExtraction);
			shader->Begin(rc);
			screen_sprite->SetShaderResourceView(offscreen->GetShaderResourceView(),
				offscreen->GetTextureWidth(), offscreen->GetTextureHeight());
			screen_sprite->Update(0, 0, vp.Width, vp.Height,
				0, 0, offscreen->GetTextureWidth(), offscreen->GetTextureHeight(),
				0,
				1, 1, 1, 1);
			shader->Draw(rc, screen_sprite.get());
			shader->End(rc);
		}
		//	②抽出した明るい部分をぼかす
		{
			//	描画先の変更
			ID3D11RenderTargetView* rtvs;
			rtvs = workRenderTargets[GaussianFilterIndex]->GetRenderTargetView();
			FLOAT clear_color[4] = { 0,0,0,0 };
			rc.deviceContext->ClearRenderTargetView(rtvs, clear_color);
			rc.deviceContext->OMSetRenderTargets(1, &rtvs, nullptr);

			//	ビューポートの設定を変える
			D3D11_VIEWPORT	vp = {};
			vp.Width = workRenderTargets[GaussianFilterIndex]->GetTextureWidth();
			vp.Height = workRenderTargets[GaussianFilterIndex]->GetTextureHeight();
			vp.MinDepth = 0.0f;
			vp.MaxDepth = 1.0f;
			dc->RSSetViewports(1, &vp);

			//	ガウスブラーをかける
			SpriteShader* shader = graphics.GetShader(SpriteShaderId::GaussianBlur);
			shader->Begin(rc);
			screen_sprite->SetShaderResourceView(
				workRenderTargets[LuminanceExtractionIndex]->GetShaderResourceView(),
				workRenderTargets[LuminanceExtractionIndex]->GetTextureWidth(),
				workRenderTargets[LuminanceExtractionIndex]->GetTextureHeight());
			screen_sprite->Update(0, 0, vp.Width, vp.Height,
				0, 0,
				workRenderTargets[LuminanceExtractionIndex]->GetTextureWidth(),
				workRenderTargets[LuminanceExtractionIndex]->GetTextureHeight(),
				0,
				1, 1, 1, 1);
			shader->Draw(rc, screen_sprite.get());
			shader->End(rc);
		}
		//	③シーンバッファに対して描き込む
		{
			//	描画先の変更
			ID3D11RenderTargetView* rtvs;
			rtvs = offscreen->GetRenderTargetView();
			rc.deviceContext->OMSetRenderTargets(1, &rtvs, nullptr);

			//	ビューポートの設定を変える
			D3D11_VIEWPORT	vp = {};
			vp.Width = offscreen->GetTextureWidth();
			vp.Height = offscreen->GetTextureHeight();
			vp.MinDepth = 0.0f;
			vp.MaxDepth = 1.0f;
			dc->RSSetViewports(1, &vp);

			//	ガウスブラーをかける
			SpriteShader* shader = graphics.GetShader(SpriteShaderId::DefaultAddBlend);
			shader->Begin(rc);
			screen_sprite->SetShaderResourceView(
				workRenderTargets[GaussianFilterIndex]->GetShaderResourceView(),
				workRenderTargets[GaussianFilterIndex]->GetTextureWidth(),
				workRenderTargets[GaussianFilterIndex]->GetTextureHeight());
			screen_sprite->Update(0, 0, vp.Width, vp.Height,
				0, 0,
				workRenderTargets[GaussianFilterIndex]->GetTextureWidth(),
				workRenderTargets[GaussianFilterIndex]->GetTextureHeight(),
				0,
				1, 1, 1, 1);
			shader->Draw(rc, screen_sprite.get());
			shader->End(rc);
		}
	}

	//	ライトシャフト処理
	if (mainDirectionalLight && lightShaftIteration > 0)
	{
		// 描画コンテキスト設定
		{
			rc.viewPosition.x = camera.GetEye().x;
			rc.viewPosition.y = camera.GetEye().y;
			rc.viewPosition.z = camera.GetEye().z;
			rc.viewPosition.w = 1;
			rc.view = camera.GetView();
			rc.projection = camera.GetProjection();

			rc.lightShaftData.iteration = lightShaftIteration;
			rc.lightShaftData.intensity = lightShaftIntensity;
			rc.lightShaftData.adjustLength = lightShaftAdjustLength;
			rc.lightShaftData.depthSRV = sceneDepthStencil->GetShaderResourceView().Get();

			rc.directionalLightData.direction.x = mainDirectionalLight->GetDirection().x;
			rc.directionalLightData.direction.y = mainDirectionalLight->GetDirection().y;
			rc.directionalLightData.direction.z = mainDirectionalLight->GetDirection().z;
			rc.directionalLightData.direction.w = 0.0f;
			rc.directionalLightData.color.x = mainDirectionalLight->GetColor().x;
			rc.directionalLightData.color.y = mainDirectionalLight->GetColor().y;
			rc.directionalLightData.color.z = mainDirectionalLight->GetColor().z;
			rc.directionalLightData.color.w = mainDirectionalLight->GetColor().w;

			rc.shadowMapData.lightViewProjection = lightViewProjection;
			rc.shadowMapData.shadowColor = shadowColor;
			rc.shadowMapData.shadowBias = shadowBias;
			rc.shadowMapData.shadowMap = shadowmapDepthStencil->GetShaderResourceView().Get();
			rc.shadowMapData.PCFKernelSize = PCFKernelSize;
		}
		static constexpr int LightShaftSceneIndex = 0;
		static constexpr int GaussianFilterIndex = 1;
		// ①明るい部分を抜き出す処理
		{
			ID3D11RenderTargetView* rtvs;
			rtvs = workRenderTargets[LightShaftSceneIndex]->GetRenderTargetView();
			FLOAT clear_color[4] = { 0,0,0,0 };
			rc.deviceContext->ClearRenderTargetView(rtvs, clear_color);
			rc.deviceContext->OMSetRenderTargets(1, &rtvs, nullptr);

			D3D11_VIEWPORT	vp = {};
			vp.Width = workRenderTargets[LightShaftSceneIndex]->GetTextureWidth();
			vp.Height = workRenderTargets[LightShaftSceneIndex]->GetTextureHeight();
			vp.MinDepth = 0.0f;
			vp.MaxDepth = 1.0f;
			dc->RSSetViewports(1, &vp);

			SpriteShader* shader = graphics.GetShader(SpriteShaderId::LightShaft);
			shader->Begin(rc);
			screen_sprite->Update(0, 0, vp.Width, vp.Height,
				0, 0, offscreen->GetTextureWidth(), offscreen->GetTextureHeight(),
				0,
				1, 1, 1, 1);
			shader->Draw(rc, screen_sprite.get());
			shader->End(rc);
		}
		//	②抽出した明るい部分をぼかす
		{
			//	描画先の変更
			ID3D11RenderTargetView* rtvs;
			rtvs = workRenderTargets[GaussianFilterIndex]->GetRenderTargetView();
			FLOAT clear_color[4] = { 0,0,0,0 };
			rc.deviceContext->ClearRenderTargetView(rtvs, clear_color);
			rc.deviceContext->OMSetRenderTargets(1, &rtvs, nullptr);

			//	ビューポートの設定を変える
			D3D11_VIEWPORT	vp = {};
			vp.Width = workRenderTargets[GaussianFilterIndex]->GetTextureWidth();
			vp.Height = workRenderTargets[GaussianFilterIndex]->GetTextureHeight();
			vp.MinDepth = 0.0f;
			vp.MaxDepth = 1.0f;
			dc->RSSetViewports(1, &vp);

			//	ガウスブラーをかける
			SpriteShader* shader = graphics.GetShader(SpriteShaderId::GaussianBlur);
			shader->Begin(rc);
			screen_sprite->SetShaderResourceView(
				workRenderTargets[LightShaftSceneIndex]->GetShaderResourceView(),
				workRenderTargets[LightShaftSceneIndex]->GetTextureWidth(),
				workRenderTargets[LightShaftSceneIndex]->GetTextureHeight());
			screen_sprite->Update(0, 0, vp.Width, vp.Height,
				0, 0,
				workRenderTargets[LightShaftSceneIndex]->GetTextureWidth(),
				workRenderTargets[LightShaftSceneIndex]->GetTextureHeight(),
				0,
				1, 1, 1, 1);
			shader->Draw(rc, screen_sprite.get());
			shader->End(rc);
		}
		//	③シーンバッファに対して描き込む
		{
			//	描画先の変更
			ID3D11RenderTargetView* rtvs;
			rtvs = offscreen->GetRenderTargetView();
			rc.deviceContext->OMSetRenderTargets(1, &rtvs, nullptr);

			//	ビューポートの設定を変える
			D3D11_VIEWPORT	vp = {};
			vp.Width = offscreen->GetTextureWidth();
			vp.Height = offscreen->GetTextureHeight();
			vp.MinDepth = 0.0f;
			vp.MaxDepth = 1.0f;
			dc->RSSetViewports(1, &vp);

			//	ガウスブラーをかける
			SpriteShader* shader = graphics.GetShader(SpriteShaderId::DefaultAddBlend);
			shader->Begin(rc);
			screen_sprite->SetShaderResourceView(
				workRenderTargets[GaussianFilterIndex]->GetShaderResourceView(),
				workRenderTargets[GaussianFilterIndex]->GetTextureWidth(),
				workRenderTargets[GaussianFilterIndex]->GetTextureHeight());
			screen_sprite->Update(0, 0, vp.Width, vp.Height,
				0, 0,
				workRenderTargets[GaussianFilterIndex]->GetTextureWidth(),
				workRenderTargets[GaussianFilterIndex]->GetTextureHeight(),
				0,
				1, 1, 1, 1);
			shader->Draw(rc, screen_sprite.get());
			shader->End(rc);
		}
	}

	//	被写界深度
	{
		static constexpr int BokehSceneIndex = 0;
		static constexpr int MixSceneIndex = 1;
		//	①ガウスブラーを使ってシーンをぼかす
		{
			//	描画先を変更
			ID3D11RenderTargetView* rtv = workRenderTargets[BokehSceneIndex]->GetRenderTargetView();
			FLOAT color[] = { 0,0,0,0 };
			rc.deviceContext->ClearRenderTargetView(rtv, color);
			rc.deviceContext->OMSetRenderTargets(1, &rtv, nullptr);

			D3D11_VIEWPORT viewport{};
			viewport.Width =
				static_cast<float>(workRenderTargets[BokehSceneIndex]->GetTextureWidth());
			viewport.Height =
				static_cast<float>(workRenderTargets[BokehSceneIndex]->GetTextureHeight());
			viewport.MinDepth = 0.0f;
			viewport.MaxDepth = 1.0f;
			rc.deviceContext->RSSetViewports(1, &viewport);
			//	ガウスブラーをかける
			SpriteShader* shader = graphics.GetShader(SpriteShaderId::GaussianBlur);
			//	ボケ量を指定
			rc.gaussianFilterData.kernelSize = depthOfFieldBlurSize;
			rc.gaussianFilterData.textureSize.x =
				static_cast<float>(workRenderTargets[BokehSceneIndex]->GetTextureWidth());
			rc.gaussianFilterData.textureSize.y =
				static_cast<float>(workRenderTargets[BokehSceneIndex]->GetTextureHeight());
			shader->Begin(rc);
			{
				//	スプライトの描画設定変更
				screen_sprite->SetShaderResourceView(offscreen->GetShaderResourceView(),
					offscreen->GetTextureWidth(), offscreen->GetTextureHeight());
				screen_sprite->Update(0, 0, viewport.Width, viewport.Height,
					0, 0, offscreen->GetTextureWidth(), offscreen->GetTextureHeight());
				//	スプライト描画
				shader->Draw(rc, screen_sprite.get());
			}
			shader->End(rc);
		}
		//	②ぼかしたシーンと深度バッファと元のシーンの３つを使って合成
		{
			ID3D11RenderTargetView* rtv = workRenderTargets[MixSceneIndex]->GetRenderTargetView();
			FLOAT color[] = { 0,0,0,0 };
			rc.deviceContext->ClearRenderTargetView(rtv, color);
			rc.deviceContext->OMSetRenderTargets(1, &rtv, nullptr);
			D3D11_VIEWPORT viewport{};
			viewport.Width = static_cast<float>(workRenderTargets[MixSceneIndex]->GetTextureWidth());
			viewport.Height = static_cast<float>(workRenderTargets[MixSceneIndex]->GetTextureHeight());
			viewport.MinDepth = 0.0f;
			viewport.MaxDepth = 1.0f;
			rc.deviceContext->RSSetViewports(1, &viewport);

			//	被写界深度をかける
			SpriteShader* shader = graphics.GetShader(SpriteShaderId::DepthOfField);
			//	被写界深度情報の設定
			rc.depthOfFieldData.bokehSceneSRV =
				workRenderTargets[BokehSceneIndex]->GetShaderResourceView();
			rc.depthOfFieldData.depthSRV = sceneDepthStencil->GetShaderResourceView().Get();
			rc.depthOfFieldData.focusDistance = focusDistance;
			rc.depthOfFieldData.range = dofRange;
			rc.depthOfFieldData.nearClip = 0.1f;
			rc.depthOfFieldData.farClip = 1000.0f;
			shader->Begin(rc);
			{
				//	スプライトの描画設定変更
				screen_sprite->SetShaderResourceView(offscreen->GetShaderResourceView(),
					offscreen->GetTextureWidth(), offscreen->GetTextureHeight());
				screen_sprite->Update(0, 0, viewport.Width, viewport.Height,
					0, 0, offscreen->GetTextureWidth(), offscreen->GetTextureHeight());
				//	スプライト描画
				shader->Draw(rc, screen_sprite.get());
			}
			shader->End(rc);
		}

		ClearShaderResorceViews();

		//	③シーンバッファに対して描き込む
		{
			ID3D11RenderTargetView* rtvs = offscreen->GetRenderTargetView();
			rc.deviceContext->OMSetRenderTargets(1, &rtvs, nullptr);
			D3D11_VIEWPORT	vp = {};
			vp.Width = offscreen->GetTextureWidth();
			vp.Height = offscreen->GetTextureHeight();
			vp.MinDepth = 0.0f;
			vp.MaxDepth = 1.0f;
			dc->RSSetViewports(1, &vp);
			SpriteShader* shader = graphics.GetShader(SpriteShaderId::Default);
			shader->Begin(rc);
			screen_sprite->SetShaderResourceView(
				workRenderTargets[MixSceneIndex]->GetShaderResourceView(),
				workRenderTargets[MixSceneIndex]->GetTextureWidth(),
				workRenderTargets[MixSceneIndex]->GetTextureHeight());
			screen_sprite->Update(0, 0, vp.Width, vp.Height,
				0, 0, workRenderTargets[MixSceneIndex]->GetTextureWidth(),
				workRenderTargets[MixSceneIndex]->GetTextureHeight());
			shader->Draw(rc, screen_sprite.get());
			shader->End(rc);
		}
	}

	//	バックバッファに描画先を変更
	{
		// 画面クリア＆レンダーターゲット設定
		FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0～1.0)
		dc->ClearRenderTargetView(rtv, color);
		dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		dc->OMSetRenderTargets(1, &rtv, dsv);

		// ビューポートの設定
		D3D11_VIEWPORT	vp = {};
		vp.Width = graphics.GetScreenWidth();
		vp.Height = graphics.GetScreenHeight();
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		dc->RSSetViewports(1, &vp);
	}

	// 2Dスプライト描画
	{
		SpriteShader* shader = graphics.GetShader(SpriteShaderId::Default);
		if (sceneBlurSize > 1) {
			//	ガウスフィルターを使って画面を暈す
			shader = graphics.GetShader(SpriteShaderId::GaussianBlur);
			rc.gaussianFilterData.kernelSize = sceneBlurSize;
			rc.gaussianFilterData.textureSize.x = offscreen->GetTextureWidth();
			rc.gaussianFilterData.textureSize.y = offscreen->GetTextureHeight();
		}

		screen_sprite->SetShaderResourceView(
			offscreen->GetShaderResourceView(),
			offscreen->GetTextureWidth(),
			offscreen->GetTextureHeight());
		screen_sprite->Update(0, 0, graphics.GetScreenWidth(), graphics.GetScreenHeight(), 0, 0, offscreen->GetTextureWidth(), offscreen->GetTextureHeight());
		shader->Begin(rc);
		screen_sprite->Render(rc, shader);
		shader->End(rc);
	}

	ClearConstantBuffers();
	ClearShaderResorceViews();
}

void SceneGame::ClearRenderTargetSlots()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	dc->OMSetRenderTargets(0, nullptr, nullptr);
}

void SceneGame::ClearConstantBuffers(int start_slot, int num)
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11Buffer* clear_constant_bufferes[D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT]{};
	dc->VSSetConstantBuffers(start_slot, num, clear_constant_bufferes);
	dc->HSSetConstantBuffers(start_slot, num, clear_constant_bufferes);
	dc->DSSetConstantBuffers(start_slot, num, clear_constant_bufferes);
	dc->GSSetConstantBuffers(start_slot, num, clear_constant_bufferes);
	dc->PSSetConstantBuffers(start_slot, num, clear_constant_bufferes);
	dc->CSSetConstantBuffers(start_slot, num, clear_constant_bufferes);
}

void SceneGame::ClearShaderResorceViews(int start_slot, int num)
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11ShaderResourceView* clear_shader_resource_view[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT]{};
	dc->VSSetShaderResources(start_slot, num, clear_shader_resource_view);
	dc->HSSetShaderResources(start_slot, num, clear_shader_resource_view);
	dc->DSSetShaderResources(start_slot, num, clear_shader_resource_view);
	dc->GSSetShaderResources(start_slot, num, clear_shader_resource_view);
	dc->PSSetShaderResources(start_slot, num, clear_shader_resource_view);
	dc->CSSetShaderResources(start_slot, num, clear_shader_resource_view);
}

void SceneGame::ClearUnorderedAccessViews(int start_slot, int num)
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11UnorderedAccessView* clear_unordered_access_view[8]{};
	dc->CSSetUnorderedAccessViews(start_slot, num, clear_unordered_access_view, nullptr);
}

void SceneGame::ClearSampler(int start_slot, int num)
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11SamplerState* clear_sampler[D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT]{};
	dc->VSSetSamplers(start_slot, num, clear_sampler);
	dc->HSSetSamplers(start_slot, num, clear_sampler);
	dc->DSSetSamplers(start_slot, num, clear_sampler);
	dc->GSSetSamplers(start_slot, num, clear_sampler);
	dc->PSSetSamplers(start_slot, num, clear_sampler);
	dc->CSSetSamplers(start_slot, num, clear_sampler);
}

void SceneGame::Render()
{
	// シャドウマップの描画
	RenderShadowmap();

	// 3D空間の描画を別のバッファに対して行う
	Render3DScene();

	// ポストプロセスを適応してバックバッファに描画
	RenderPostprocess();
#ifdef _DEBUG
	// 2DデバッグGUI描画
	//----------------------------------
	// Lighting
	//----------------------------------
	ImGui::SetNextWindowSize(ImVec2(350, 300), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Lighting"))
	{
		LightManager::Instance().DrawDebugGUI();
	}
	ImGui::End();

	//----------------------------------
	// Toon
	//----------------------------------
	ImGui::SetNextWindowSize(ImVec2(350, 200), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Toon"))
	{
		ImGui::SliderFloat("OutLine", &outlinewidth, 0, 1.0f);
		ImGui::ColorEdit3("OutLineColor", &outlineColor.x);
	}
	ImGui::End();

	//----------------------------------
	// Player
	//----------------------------------
	ImGui::SetNextWindowSize(ImVec2(350, 300), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Player"))
	{
		player->DrawDebugGUI();
	}
	ImGui::End();

	//----------------------------------
// Camera
//----------------------------------
	ImGui::SetNextWindowSize(ImVec2(350, 300), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Camera"))
	{
		Camera::Instance().DrawDebugGUI();
		cameraController->DrawDebugGUI();
	}
	ImGui::End();

	//----------------------------------
	// Enemy
	//----------------------------------
	ImGui::SetNextWindowSize(ImVec2(350, 300), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Enemy"))
	{
		EnemyManager::Instance().DrawDebugGUI();
	}
	ImGui::End();

	//----------------------------------
	// ShadowMap
	//----------------------------------
	ImGui::SetNextWindowSize(ImVec2(400, 500), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("ShadowMap"))
	{
		ImGui::SliderFloat("DrawRect", &shadowDrawRect, 1.0f, 512.0f);
		ImGui::ColorEdit3("Color", &shadowColor.x);
		ImGui::SliderFloat("Bias", &shadowBias, 0.0f, 0.01f, "% 0.5f");
		ImGui::SliderInt("PCFKernelSize", &PCFKernelSize, 1, 15);

		ImGui::Text("Depth Texture");
		ImGui::Image(
			shadowmapDepthStencil->GetShaderResourceView().Get(),
			{ 256,256 }, { 0,0 }, { 1,1 },
			{ 1,1,1,1 }
		);
	}
	ImGui::End();

	//----------------------------------
	// SceneMap (PostEffect)
	//----------------------------------
	ImGui::SetNextWindowSize(ImVec2(450, 600), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("SceneMap"))
	{
		ImGui::SliderInt("sceneBlurSize", &sceneBlurSize, 0, MaxKernelSize - 1);

		ImGui::SliderFloat("bloom intensity", &luminanceExtractionData.intensity, 0.0f, 10.0f);
		ImGui::SliderFloat("bloom threshold", &luminanceExtractionData.threshold, 0.0f, 1.0f);
		ImGui::SliderInt("bloomBlurSize", &bloomBlurSize, 0, MaxKernelSize - 1);

		ImGui::SliderInt("depthOfFieldBlurSize", &depthOfFieldBlurSize, 0, MaxKernelSize - 1);

		ImGui::SliderInt("lightShaftIteration", &lightShaftIteration, 0, 32);
		ImGui::SliderFloat("lightShaftIntensity", &lightShaftIntensity, 0.0f, 1.0f);
		ImGui::SliderFloat("lightShaftAdjustLength", &lightShaftAdjustLength, 0.0f, 10.0f);

		ImGui::Text("Work Texture 0");
		ImGui::Image(workRenderTargets[0]->GetShaderResourceView(), { 1280 / 5,720 / 4 });

		ImGui::Text("Work Texture 1");
		ImGui::Image(workRenderTargets[1]->GetShaderResourceView(), { 1280 / 5,720 / 4 });

		ImGui::Text("Final Offscreen");
		ImGui::Image(offscreen->GetShaderResourceView(), { 1280 / 5,720 / 4 });
	}
	ImGui::End();

#endif
}