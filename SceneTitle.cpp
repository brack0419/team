#include "Graphics/Graphics.h"
#include "SceneTitle.h"
#include "SceneGame.h"
#include "SceneLoading.h"
#include "SceneManager.h"
#include "Input/Input.h"

// 初期化
void SceneTitle::Initialize()
{
	// テクスチャー初期化
	//texture = new Texture("Data/Sprite/Title.png");
	texture = std::make_unique<Texture>("Data/Sprite/Title.png");
	// マスクテクスチャの読み込み
	maskTexture = std::make_unique<Texture>("Data/Texture/dissolve_animation.png");

	// スプライト初期化
	//sprite = new Sprite();
	sprite = std::make_unique<Sprite>();
	// テクスチャーの設定
	sprite->SetShaderResourceView(texture->GetShaderResourceView(), texture->GetWidth(), texture->GetHeight());
	// 閾値の初期化
	dissolveThreshold = 1.0f;
	edgeThreshold = 0.2f; 	// 縁の閾値
	edgeColor = { 1, 0, 0, 1 };		// 縁の色
}

// 終了化
void SceneTitle::Finalize()
{
	//// スプライト終了化
	//if (sprite != nullptr)
	//{
	//	delete sprite;
	//	sprite = nullptr;
	//}
	//// テクスチャー終了化
	//if (texture != nullptr)
	//{
	//	delete texture;
	//	texture = nullptr;
	//}
}

// 更新処理
void SceneTitle::Update(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	// なにかボタンを押したらローディングシーンへ切り替え
	const GamePadButton anyButton =
		GamePad::BTN_A
		| GamePad::BTN_B
		| GamePad::BTN_X
		| GamePad::BTN_Y
		;
	if (gamePad.GetButtonDown() & anyButton)
	{
		SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame));
	}

	sprite->Update(0.0f, 0.0f,
		Graphics::Instance().GetScreenWidth(), Graphics::Instance().GetScreenHeight(),
		0.0f, 0.0f,
		static_cast<float>(texture->GetWidth()), static_cast<float>(texture->GetHeight()),
		0.0f,
		1.0f, 1.0f, 1.0f, 1.0f);
}

// 描画処理
void SceneTitle::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

	// 画面クリア＆レンダーターゲット設定
	FLOAT color[] = { 0.5f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0～1.0)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);

	// 2Dスプライト描画
	{
		float screenWidth = static_cast<float>(graphics.GetScreenWidth());
		float screenHeight = static_cast<float>(graphics.GetScreenHeight());
		float textureWidth = static_cast<float>(sprite->GetTextureWidth());
		float textureHeight = static_cast<float>(sprite->GetTextureHeight());
		// タイトルスプライト描画
		//sprite->Render(dc,
		//	0, 0, screenWidth, screenHeight,
		//	0, 0, textureWidth, textureHeight,
		//	0,
		//	1, 1, 1, 1);
		//sprite->Update(
		//	0, 0, screenWidth, screenHeight,
		//	0, 0, textureWidth, textureHeight,
		//	0,
		//	1, 1, 1, 1);
	}

	// 2Dスプライト描画
	{
		RenderContext rc{};
		rc.deviceContext = dc;

		SpriteShader* shader = graphics.GetShader(SpriteShaderId::Mask);
		shader->Begin(rc);
		rc.uvScrollData.uvScrollValue = uvScrollData.uvScrollValue;
		rc.maskData.maskTexture = maskTexture->GetShaderResourceView().Get();
		rc.maskData.dissolveThreshold = dissolveThreshold;
		rc.maskData.edgeThreshold = edgeThreshold;
		rc.maskData.edgeColor = edgeColor;

		sprite->Render(rc, shader);

		shader->End(rc);
	}

	// 2DデバッグGUI描画
	{
		ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

		if (ImGui::Begin("Debug Menu", nullptr, ImGuiWindowFlags_None))
		{
			if (ImGui::CollapsingHeader("UVScroll", ImGuiTreeNodeFlags_DefaultOpen))
			{
				float min_value = -1.0f;  // スライダーの最小値
				float max_value = 1.0f;  // スライダーの最大値

				// ImGuiの中でスライダーを描画する
				ImGui::SliderFloat("UVScroll_X", &uvScrollData.uvScrollValue.x, min_value, max_value);
				ImGui::SliderFloat("UVScroll_Y", &uvScrollData.uvScrollValue.y, min_value, max_value);
			}

			if (ImGui::CollapsingHeader("Mask", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::SliderFloat("Dissolve Threshold", &dissolveThreshold, 0.0f, 1.0f);
				ImGui::SliderFloat("Edge Threshold", &edgeThreshold, 0.0f, 1.0f);
				ImGui::ColorEdit4("Edge Color", &edgeColor.x);
			}
		}
		ImGui::End();
	}
}