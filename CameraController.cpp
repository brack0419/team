#include <imgui.h>
#include "CameraController.h"
#include "Camera.h"
#include "Input/Input.h"
#include <Windows.h>
#include <DirectXMath.h>
#include <algorithm>

// 更新処理
void CameraController::Update(float elapsedTime)
{
	ImGuiIO& io = ImGui::GetIO();

	// ===============================
	// 右クリックでカメラ操作トグル
	// ===============================
	if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && !io.WantCaptureMouse)
	{
		mouseCameraActive = !mouseCameraActive;

		if (mouseCameraActive)
		{
			ShowCursor(FALSE);  // カメラ操作開始 → 非表示
		}
		else
		{
			ShowCursor(TRUE);   // カメラ操作終了 → 表示
		}
	}

	// カメラ操作していないなら処理しない
	if (!mouseCameraActive)
	{
		return;
	}

	// ===============================
	// マウス移動量でカメラ回転
	// ===============================
	angle.y += io.MouseDelta.x * mouseSensitivity;
	angle.x += io.MouseDelta.y * mouseSensitivity;

	// X軸制限
	angle.x = std::clamp(angle.x, minAngleX, maxAngleX);

	// Y軸ラップ
	if (angle.y < -DirectX::XM_PI) angle.y += DirectX::XM_2PI;
	if (angle.y > DirectX::XM_PI) angle.y -= DirectX::XM_2PI;

	// ===============================
	// カメラ計算
	// ===============================
	DirectX::XMMATRIX transform =
		DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);

	DirectX::XMVECTOR frontVec = transform.r[2];
	DirectX::XMFLOAT3 front;
	DirectX::XMStoreFloat3(&front, frontVec);

	DirectX::XMFLOAT3 eye;
	eye.x = target.x - front.x * range;
	eye.y = target.y - front.y * range;
	eye.z = target.z - front.z * range;

	Camera::Instance().SetLookAt(
		eye,
		target,
		DirectX::XMFLOAT3(0, 1, 0)
	);
	// ===============================
// マウスホイールでズーム（Range変更）
// ===============================
	if (io.MouseWheel != 0.0f && !io.WantCaptureMouse)
	{
		const float zoomSpeed = 1.0f;   // 調整用
		range -= io.MouseWheel * zoomSpeed;

		// 範囲制限
		range = std::clamp(range, 0.5f, 50.0f);
	}
}

// デバッグ用GUI描画
void CameraController::DrawDebugGUI()
{
	if (!ImGui::Begin("Camera"))
	{
		ImGui::End();
		return;
	}

	// =========================
	// カメラ操作 ON / OFF
	// =========================
	ImGui::Checkbox("Mouse Camera Active", &mouseCameraActive);

	ImGui::Separator();

	// =========================
	// マウス感度
	// =========================
	ImGui::SliderFloat(
		"Mouse Sensitivity",
		&mouseSensitivity,
		0.0f,
		0.02f,
		"%.3f"
	);

	ImGui::Separator();

	// =========================
	// 詳細設定
	// =========================
	if (ImGui::CollapsingHeader("CameraController", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// -------- 距離 --------
		ImGui::SliderFloat(
			"Range",
			&range,
			0.5f,
			50.0f
		);

		// -------- 上制限 --------
		float maxAngleDeg = DirectX::XMConvertToDegrees(maxAngleX);

		ImGui::SliderFloat(
			"Max Angle X (deg)",
			&maxAngleDeg,
			0.0f,
			89.0f
		);

		maxAngleX = DirectX::XMConvertToRadians(maxAngleDeg);
	}

	ImGui::End();
}