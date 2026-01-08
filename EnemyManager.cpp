#include <imgui.h>
#include "EnemyManager.h"
#include "Collision.h"
#include "EnemySlime.h"

// 更新処理
void EnemyManager::Update(float elapsedTime)
{
	for (Enemy* enemy : enemies)
	{
		enemy->Update(elapsedTime);
	}

	// 破棄処理
	// ※enemiesの範囲for文中でerase()すると不具合が発生してしまうため、
	// 　更新処理が終わった後に破棄リストに積まれたオブジェクトを削除する。
	for (Enemy* enemy : removes)
	{
		// std::vectorから要素を削除する場合はイテレーターで削除しなければならない
		std::vector<Enemy*>::iterator it = std::find(enemies.begin(), enemies.end(), enemy);
		if (it != enemies.end())
		{
			enemies.erase(it);
		}

		// 削除
		delete enemy;
	}
	// 破棄リストをクリア
	removes.clear();

	// 敵同士の衝突処理
	CollisionEnemyVsEnemies();
}

// 描画処理
void EnemyManager::Render(const RenderContext& rc, ModelShader* shader)
{
	for (Enemy* enemy : enemies)
	{
		enemy->Render(rc, shader);
	}
}

// デバッグプリミティブ描画
void EnemyManager::DrawDebugPrimitive()
{
	if (!visibleDebugPrimitive) return;

	for (Enemy* enemy : enemies)
	{
		enemy->DrawDebugPrimitive();
	}
}

// エネミー登録
void EnemyManager::Register(Enemy* enemy)
{
	enemies.emplace_back(enemy);
}

// エネミー全削除
void EnemyManager::Clear()
{
	for (Enemy* enemy : enemies)
	{
		delete enemy;
	}
	enemies.clear();
}

// エネミー削除
void EnemyManager::Remove(Enemy* enemy)
{
	// 破棄リストに追加
	removes.insert(enemy);
}

// エネミー同士の衝突処理
void EnemyManager::CollisionEnemyVsEnemies()
{
	size_t enemyCount = enemies.size();
	for (int i = 0; i < enemyCount; ++i)
	{
		Enemy* enemyA = enemies.at(i);
		for (int j = i + 1; j < enemyCount; ++j)
		{
			Enemy* enemyB = enemies.at(j);

			DirectX::XMFLOAT3 outPosition;
			if (Collision::IntersectCylinderVsCylinder(
				enemyA->GetPosition(),
				enemyA->GetRadius(),
				enemyA->GetHeight(),
				enemyB->GetPosition(),
				enemyB->GetRadius(),
				enemyB->GetHeight(),
				outPosition))
			{
				enemyB->SetPosition(outPosition);
			}
		}
	}
}

// デバッグ用GUI描画
void EnemyManager::DrawDebugGUI()
{
	if (ImGui::CollapsingHeader("EnemyManager", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::Button("Add"))
		{
			DirectX::XMFLOAT3 position =
			{
				((static_cast<float>(rand() * 2) / RAND_MAX) - 1.0f) * 5.0f,
				0.0f,
				((static_cast<float>(rand() * 2) / RAND_MAX) - 1.0f) * 5.0f,
			};
			Enemy* enemy = new EnemySlime();
			enemy->SetPosition(position);
			Register(enemy);
		}
		ImGui::SameLine();

		if (ImGui::Button("Clear"))
		{
			Clear();
		}

		ImGui::Checkbox("VisibleDebugPrimitive", &visibleDebugPrimitive);

		int index = 0;
		for (Enemy* enemy : enemies)
		{
			char name[32];
			::sprintf_s(name, sizeof(name), "%d", index);

			ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_DefaultOpen;

			if (ImGui::TreeNodeEx(enemy, node_flags, name))
			{
				ImGui::PushID(index);
				enemy->DrawDebugGUI();
				ImGui::PopID();
				ImGui::TreePop();
			}
			index++;
		}
	}
}