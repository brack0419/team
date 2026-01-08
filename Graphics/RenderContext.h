#pragma once

#include <DirectXMath.h>
#include <D3D11.h>

struct DirectionalLightData
{
	DirectX::XMFLOAT4	direction;	// 向き
	DirectX::XMFLOAT4	color;		// 色
};
// 点光源情報
struct PointLightData
{
	DirectX::XMFLOAT4	position;	// 座標
	DirectX::XMFLOAT4	color;		// 色
	float				range;		// 範囲
	float				influence;	// 光の影響力
	DirectX::XMFLOAT2	dummy;
};

// 点光源の最大数
static	constexpr	int	PointLightMax = 8;

// スポットライト情報
struct SpotLightData
{
	DirectX::XMFLOAT4	position;	// 座標
	DirectX::XMFLOAT4	direction;	// 向き
	DirectX::XMFLOAT4	color;		// 色
	float			range;		// 範囲
	float			innerCorn; 	// インナー角度範囲
	float			outerCorn; 	// アウター角度範囲
	float			influence; // 光の影響力;
};

// スポットライトの最大数
static	constexpr	int	SpotLightMax = 8;

// マスクデータ
struct MaskData
{
	ID3D11ShaderResourceView* maskTexture;
	float					dissolveThreshold;
	float					edgeThreshold; 	// 縁の閾値
	DirectX::XMFLOAT4		edgeColor;		// 縁の色
};

//	UVスクロール情報
struct UVScrollData
{
	DirectX::XMFLOAT2	uvScrollValue;	// UVスクロール値
};

// トゥーンシェーディング用アウトライン
struct OutlineData
{
	DirectX::XMFLOAT4X4		viewProjection;
	DirectX::XMFLOAT4		outlineColor;
	float					outlineSize;
	DirectX::XMFLOAT3		dummy;
};

//	シャドウマップ用情報
struct ShadowMapData
{
	ID3D11ShaderResourceView* shadowMap;				//	シャドウマップテクスチャ
	DirectX::XMFLOAT4X4			lightViewProjection;	//	ライトビュープロジェクション行列
	DirectX::XMFLOAT3			shadowColor;			//	影の色
	float						shadowBias;				//	深度比較用のオフセット値
	int                         PCFKernelSize;          // ソフトシャドーの行列サイズ
	//DirectX::XMFLOAT3           dummy;
};

//	ガウスフィルター計算情報
struct GaussianFilterData {
	int kernelSize = 8;
	float deviation = 10.0f;
	DirectX::XMFLOAT2 textureSize;
};
// ガウスフィルターの最大カーネルサイズ
static const int MaxKernelSize = 16;

// 明るい部分を抽出するための情報
struct LuminanceExtractionData
{
	float	threshold = 0.5f;	//	閾値
	float	intensity = 1.0f;	//	ブルームの強度
	DirectX::XMFLOAT2	dummy;
};

//	被写界深度
struct DepthOfFieldData {
	float focusDistance;	//	被写体距離
	float range;	//	徐々にボケていく範囲
	float nearClip;	//	カメラの映す手前までの距離
	float farClip;	//	カメラの映す奥までの距離
	//	暈したテクスチャ
	ID3D11ShaderResourceView* bokehSceneSRV;
	//	深度テクスチャ
	ID3D11ShaderResourceView* depthSRV;
};

//	IBL用情報
struct IBLData
{
	ID3D11ShaderResourceView* diffuseIrradianceEnvironmentMap;				//	拡散反射IBL用テクスチャ
	ID3D11ShaderResourceView* specularPremappingRadianceEnvironmentMap;				//	拡散反射IBL用テクスチャ
	ID3D11ShaderResourceView* ggxLookUpTableMap;				//	GGXLUTテクスチャ
};

// ライトシャフト情報
// directionalLightData, shadowMapDataの情報も使う
struct LightShaftData
{
	int		iteration = 10;      //  レイマーチングの回数
	float	intensity = 0.3f;    //  明るさの強さ
	float	adjustLength = 500.0f;	//	距離調整
	ID3D11ShaderResourceView* depthSRV;	//	深度テクスチャ
};

// レンダーコンテキスト
struct RenderContext
{
	ID3D11DeviceContext* deviceContext;
	DirectX::XMFLOAT4		viewPosition;
	DirectX::XMFLOAT4X4		view;
	DirectX::XMFLOAT4X4		projection;
	DirectX::XMFLOAT4		lightDirection;

	UVScrollData			uvScrollData;
	MaskData				maskData;

	//	ライト情報
	DirectX::XMFLOAT4		ambientLightColor;
	DirectionalLightData	directionalLightData;
	PointLightData		pointLightData[PointLightMax];	// 点光源情報
	int					pointLightCount = 0;			// 点光源数
	SpotLightData		spotLightData[SpotLightMax];	// スポットライト情報
	int					spotLightCount = 0;			// スポットライト数
	OutlineData				outlinedata;

	//	シャドウマップ情報
	ShadowMapData			shadowMapData;

	// ガウスフィルター情報
	GaussianFilterData		gaussianFilterData;

	//	明るい部分抽出情報
	LuminanceExtractionData	luminanceExtractionData;

	//	被写界深度情報
	DepthOfFieldData		depthOfFieldData;

	//	IBL情報
	IBLData					iblData;

	//	ライトシャフト情報
	LightShaftData			lightShaftData;
};
