#include "EnemySlime.h"

// コンストラクタ
EnemySlime::EnemySlime()
{
	model = new Model("Data/Model/Slime/Slime.mdl");

	// モデルが大きいのでスケーリング
	scale.x = scale.y = scale.z = 0.01f;

	// 幅、高さ設定
	radius = 0.5f;
	height = 1.0f;
	model->PlayAnimation(Animation::Anim_IdleBattle, true);
}

// デストラクタ
EnemySlime::~EnemySlime()
{
	delete model;
}

void EnemySlime::TrackingMove(float elapsedTime)
{
	// 進行ベクトル取得
	DirectX::XMFLOAT3 moveVec = GetMoveVec();

	// 移動処理
	Move(moveVec.x, moveVec.z, moveSpeed);

	// 旋回処理
	Turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);
}

// 更新処理
void EnemySlime::Update(float elapsedTime)
{
	TrackingMove(elapsedTime);

	// 速力処理更新
	UpdateVelocity(elapsedTime);

	if (model->IsPlayAnimation() != Animation::Anim_WalkFWD)
	{
		model->PlayAnimation(Animation::Anim_WalkFWD, true);
	}

	// 無敵時間更新
	UpdateInvincibleTimer(elapsedTime);

	// オブジェクト行列を更新
	UpdateTransform();

	model->UpdateAnimation(elapsedTime);
	// モデル行列更新
	model->UpdateTransform(transform);
}

// 描画処理
void EnemySlime::Render(const RenderContext& rc, ModelShader* shader)
{
	shader->Draw(rc, model);
}

// 死亡した時に呼ばれる
void EnemySlime::OnDead()
{
	// 自身を破棄
	Destroy();
}