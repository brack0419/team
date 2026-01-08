#include "StageManager.h"

// 更新処理
void StageManager::Update(float elapsedTime)
{
	for (Stage* stage : stages)
	{
		stage->Update(elapsedTime);
	}
}

// 描画処理
void StageManager::Render(const RenderContext& rc, ModelShader* shader)
{
	for (Stage* stage : stages)
	{
		stage->Render(rc, shader);
	}
}

// ステージ登録
void StageManager::Register(Stage* stage)
{
	stages.emplace_back(stage);
}

// ステージ全削除
void StageManager::Clear()
{
	for (Stage* stage : stages)
	{
		delete stage;
	}
	stages.clear();
}

// レイキャスト
bool StageManager::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	bool result = false;
	hit.distance = FLT_MAX;

	for (Stage* stage : stages)
	{
		HitResult tmp;
		if (stage->RayCast(start, end, tmp))
		{
			if (hit.distance > tmp.distance)
			{
				hit = tmp;
				result = true;
			}
		}
	}

	return result;
}