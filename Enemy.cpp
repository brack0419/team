#include "Enemy.h"
#include "EnemyManager.h"
#include "Graphics/Graphics.h"
#include "Player.h"

DirectX::XMFLOAT3 Enemy::GetMoveVec()const
{
	DirectX::XMFLOAT3 target{ 5,0,-10 }; // (仮)
	target = Player::Instance().GetPosition();

	DirectX::XMFLOAT3 vec;
	vec.x = target.x - GetPosition().x;
	vec.y = 0;
	vec.z = target.z - GetPosition().z;
	// 正規化
	float len = sqrtf(vec.x * vec.x + vec.z * vec.z);
	if (len > 0) { vec.x /= len; vec.z /= len; }

	return vec;
}

// デバッグプリミティブ描画
void Enemy::DrawDebugPrimitive()
{
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

	// 衝突判定用のデバッグ円柱を描画
	debugRenderer->DrawCylinder(position, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));
}

// デバッグ用GUI描画
void Enemy::DrawDebugGUI()
{
	// 位置
	ImGui::InputFloat3("Position", &position.x);
	// 半径
	ImGui::InputFloat("Radius", &radius);
	// 高さ
	ImGui::InputFloat("Height", &height);
	// 健康
	ImGui::InputInt("Health", &health);
}

// 破棄
void Enemy::Destroy()
{
	EnemyManager::Instance().Remove(this);
}