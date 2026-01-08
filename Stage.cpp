#include "Stage.h"
#include "Graphics/Graphics.h"

// コンストラクタ
Stage::Stage()
{
	model = std::make_shared<Model>("Data/Model/ExampleStage/yuka.mdl");
	model1 = std::make_shared<Model>("Data/Model/ExampleStage/model2.mdl");
}

Stage::Stage(const char* filename)
{
	model = std::make_shared<Model>(filename);
	model1 = std::make_shared<Model>(filename);
}

Stage::~Stage()
{
}

// 更新
void Stage::Update(float elapsedTime)
{
	UpdateTransform();
	model->UpdateTransform(transform);
	model1->UpdateTransform(transform);
}

// レイキャスト（yuka 用）
bool Stage::RayCast(const DirectX::XMFLOAT3& start,
	const DirectX::XMFLOAT3& end,
	HitResult& hit)
{
	return Collision::IntersectRayVsModel(start, end, model.get(), hit);
}

// 描画
void Stage::Render(const RenderContext& rc, ModelShader* shader)
{
	//shader->Draw(rc, model.get());
	shader->Draw(rc, model1.get());
}

// Transform 更新
void Stage::UpdateTransform()
{
	using namespace DirectX;

	XMMATRIX S = XMMatrixScaling(scale.x, scale.y, scale.z);
	XMMATRIX R = XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	XMMATRIX T = XMMatrixTranslation(position.x, position.y, position.z);

	XMMATRIX W = S * R * T;
	XMStoreFloat4x4(&transform, W);
}

///////////////////////////////////////////////////////////////////

// コンストラクタ
Earth::Earth()
{
	// ステージモデルを読み込み
	model = std::make_shared<Model>("Data/Model/earth/earth.mdl");
}

Earth::~Earth()
{
	// ステージモデルを破棄
	//delete model;
}

// 更新処理
void Earth::Update(float elapsedTime)
{
	// 今は特にやることはない
	/*DirectX::XMFLOAT4X4 mat(
		 0.05f,	0,		0,		0 ,
		 0,		0.05f,	0,		0 ,
		 0,		0,		0.05f,	0 ,
		 0,		3.0f,	0,		1
	);
	*/
	Stage::UpdateTransform();
	model->UpdateTransform(transform);
}