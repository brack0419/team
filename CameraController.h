#pragma once

#include <DirectXMath.h>

// カメラコントローラー
class CameraController
{
public:
	CameraController() {}
	~CameraController() {}

	// 更新処理
	void Update(float elapsedTime);

	// ターゲット位置設定
	void SetTarget(const DirectX::XMFLOAT3& target) { this->target = target; }

	// デバッグ用GUI描画
	void DrawDebugGUI();

private:
	DirectX::XMFLOAT3		target = { 0, 0, 0 };
	DirectX::XMFLOAT3		angle = { 0, 0, 0 };
	float					rollSpeed = DirectX::XMConvertToRadians(90);
	float					range = 10.0f;
	float					maxAngleX = DirectX::XMConvertToRadians(89);
	float					minAngleX = DirectX::XMConvertToRadians(0);
	bool mouseCameraActive = false;   // カメラ操作中か
	float mouseSensitivity = 0.003f;    // マウス感度
};
