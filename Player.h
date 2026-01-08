#pragma once

#include "Graphics/Shader.h"
#include "Graphics/Model.h"
#include "Character.h"
#include "ProjectileManager.h"
#include "Effect.h"

// プレイヤー
class Player : public Character
{
public:
	Player();
	~Player() override;

	// インスタンス取得
	static Player& Instance();

	// 更新処理
	void Update(float elapsedTime);

	// 描画処理
	void Render(const RenderContext& rc, ModelShader* shader);

	// デバッグ用GUI描画
	void DrawDebugGUI();

	// デバッグプリミティブ描画
	void DrawDebugPrimitive();

	// 前方取得関数
	DirectX::XMFLOAT3 GetDirection() { return direction; }

protected:
	// 着地した時に呼ばれる
	void OnLanding() override;

private:
	// スティック入力値から移動ベクトルを取得
	DirectX::XMFLOAT3 GetMoveVec() const;

	// 移動入力処理
	void InputMove(float elapsedTime);

	// ジャンプ入力処理
	void InputJump();

	// 弾丸入力処理
	void InputProjectile();

	// プレイヤーとエネミーとの衝突処理
	void CollisionPlayerVsEnemies();

	// 弾丸と敵の衝突処理
	void CollisionProjectilesVsEnemies();
private:
	// アニメーション
	enum Animation
	{
		Anim_Attack,
		Anim_Death,
		Anim_Falling,
		Anim_GetHit1,
		Anim_GetHit2,
		Anim_Idle,
		Anim_Jump,
		Anim_Jump_Flip,
		Anim_Landing,
		Anim_Revive,
		Anim_Running,
		Anim_Walking
	};

private:
	Model* model = nullptr;
	float				moveSpeed = 5.0f;
	float				turnSpeed = DirectX::XMConvertToRadians(720);
	float				jumpSpeed = 20.0f;
	int					jumpCount = 0;
	int					jumpLimit = 2;
	ProjectileManager	projectileManager;
	Effect* hitEffect = nullptr;
	bool				visibleDebugPrimitive = true;
	DirectX::XMFLOAT3	direction = { 0,0,1 }; // 前方
	bool drawModel = false;
};
