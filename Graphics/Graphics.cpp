#include "Misc.h"
#include "Graphics/DefaultSpriteShader.h"
#include "Graphics/LambertShader.h"
#include "Graphics/Graphics.h"

#include "Graphics/UVScrollShader.h"
#include "Graphics/MaskShader.h"
#include "Graphics/PhongShader.h"
#include "Graphics/ToonShader.h"

#include "Graphics/ShadowmapCasterShader.h"

#include "Graphics/PBRShader.h"

#include "Graphics/GaussianBlurShader.h"
#include "Graphics/LuminanceExtractionShader.h"
#include "Graphics/AddbBlendSpriteShader.h"
#include "Graphics/SubtractBlendSpriteShader.h"
#include "Graphics/DepthOfFieldShader.h"

#include "Graphics/LightShaftShader.h"
#include <dxgi1_6.h>

// 高性能GPUを取得する関数
static Microsoft::WRL::ComPtr<IDXGIAdapter4> GetHighPerformanceAdapter()
{
	Microsoft::WRL::ComPtr<IDXGIFactory6> factory;
	HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(factory.GetAddressOf()));
	if (FAILED(hr)) return nullptr;

	Microsoft::WRL::ComPtr<IDXGIAdapter4> adapter;
	factory->EnumAdapterByGpuPreference(
		0,
		DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
		IID_PPV_ARGS(adapter.GetAddressOf())
	);

	return adapter;
}

Graphics* Graphics::instance = nullptr;

// コンストラクタ
Graphics::Graphics(HWND hWnd)
{
	// インスタンス設定
	_ASSERT_EXPR(instance == nullptr, "already instantiated");
	instance = this;

	// 画面のサイズを取得する。
	RECT rc;
	GetClientRect(hWnd, &rc);
	UINT screenWidth = rc.right - rc.left;
	UINT screenHeight = rc.bottom - rc.top;

	this->screenWidth = static_cast<float>(screenWidth);
	this->screenHeight = static_cast<float>(screenHeight);

	HRESULT hr = S_OK;

	// デバイス＆スワップチェーンの生成
	{
		UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
		D3D_FEATURE_LEVEL featureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_1,
		};

		// スワップチェーンを作成するための設定オプション
		DXGI_SWAP_CHAIN_DESC swapchainDesc;
		{
			swapchainDesc.BufferDesc.Width = screenWidth;
			swapchainDesc.BufferDesc.Height = screenHeight;
			// リフレッシュレートは0/1にしてドライバに任せる（可変にする）
			swapchainDesc.BufferDesc.RefreshRate.Numerator = 0;
			swapchainDesc.BufferDesc.RefreshRate.Denominator = 1;
			swapchainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			swapchainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
			swapchainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

			swapchainDesc.SampleDesc.Count = 1;
			swapchainDesc.SampleDesc.Quality = 0;
			swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

			// FLIP モデルを使い、バックバッファは 2（2以上が推奨）
			swapchainDesc.BufferCount = 2;
			swapchainDesc.OutputWindow = hWnd;
			swapchainDesc.Windowed = TRUE;
			// FLIP_DISCARD / FLIP_SEQUENTIAL のどちらか（FLIP_DISCARD 推奨）
			swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

			// ティアリング（VSyncなしの破綻表示）を許可するフラグ（Win10以降）
			swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
		}

		D3D_FEATURE_LEVEL featureLevel;

		// デバイス＆スワップチェーンの生成
		auto adapter = GetHighPerformanceAdapter();

		hr = D3D11CreateDeviceAndSwapChain(
			adapter.Get(),                 // ★ 高性能GPUを使用
			D3D_DRIVER_TYPE_UNKNOWN,       // ★ UNKNOWN に変更（超重要）
			nullptr,
			createDeviceFlags,
			featureLevels,
			ARRAYSIZE(featureLevels),
			D3D11_SDK_VERSION,
			&swapchainDesc,
			swapchain.GetAddressOf(),
			device.GetAddressOf(),
			&featureLevel,
			immediateContext.GetAddressOf()
		);

		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// レンダーターゲットビューの生成
	{
		// スワップチェーンからバックバッファテクスチャを取得する。
		// ※スワップチェーンに内包されているバックバッファテクスチャは'色'を書き込むテクスチャ。
		Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
		hr = swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		// バックバッファテクスチャへの書き込みの窓口となるレンダーターゲットビューを生成する。
		hr = device->CreateRenderTargetView(backBuffer.Get(), nullptr, renderTargetView.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// 深度ステンシルビューの生成
	{
		// 深度ステンシル情報を書き込むためのテクスチャを作成する。
		D3D11_TEXTURE2D_DESC depthStencilBufferDesc;
		depthStencilBufferDesc.Width = screenWidth;
		depthStencilBufferDesc.Height = screenHeight;
		depthStencilBufferDesc.MipLevels = 1;
		depthStencilBufferDesc.ArraySize = 1;
		depthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	// 1ピクセルあたり、深度情報を24Bit / ステンシル情報を8bitのテクスチャを作成する。
		depthStencilBufferDesc.SampleDesc.Count = 1;
		depthStencilBufferDesc.SampleDesc.Quality = 0;
		depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;		// 深度ステンシル用のテクスチャを作成する。
		depthStencilBufferDesc.CPUAccessFlags = 0;
		depthStencilBufferDesc.MiscFlags = 0;
		hr = device->CreateTexture2D(&depthStencilBufferDesc, nullptr, depthStencilBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		// 深度ステンシルテクスチャへの書き込みに窓口になる深度ステンシルビューを作成する。
		hr = device->CreateDepthStencilView(depthStencilBuffer.Get(), nullptr, depthStencilView.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// ビューポートの設定
	{
		// 画面のどの領域にDirectXで描いた画を表示するかの設定。
		D3D11_VIEWPORT viewport;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = static_cast<float>(screenWidth);
		viewport.Height = static_cast<float>(screenHeight);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		immediateContext->RSSetViewports(1, &viewport);
	}

	// モデルシェーダー
	{
		//shader = std::make_unique<LambertShader>(device.Get());
		modelShaders[static_cast<int>(ModelShaderId::Default)] = std::make_unique<LambertShader>(device.Get());
		modelShaders[static_cast<int>(ModelShaderId::Phong)] = std::make_unique<PhongShader>(device.Get());
		modelShaders[static_cast<int>(ModelShaderId::Toon)] = std::make_unique<ToonShader>(device.Get());

		modelShaders[static_cast<int>(ModelShaderId::ShadowmapCaster)] = std::make_unique<ShadowmapCasterShader>(device.Get());
		modelShaders[static_cast<int>(ModelShaderId::PBR)] =
			std::make_unique<PBRShader>(device.Get());
	}

	// スプライトシェーダー
	{
		spriteShaders[static_cast<int>(SpriteShaderId::Default)] = std::make_unique<DefaultSpriteShader>(device.Get());
		spriteShaders[static_cast<int>(SpriteShaderId::UVScroll)] = std::make_unique<UVScrollShader>(device.Get());
		spriteShaders[static_cast<int>(SpriteShaderId::Mask)] = std::make_unique<MaskShader>(device.Get());

		spriteShaders[static_cast<int>(SpriteShaderId::GaussianBlur)] =
			std::make_unique<GaussianBlurShader>(device.Get());
		spriteShaders[static_cast<int>(SpriteShaderId::LuminanceExtraction)] =
			std::make_unique<LuminanceExtractionShader>(device.Get());
		spriteShaders[static_cast<int>(SpriteShaderId::DefaultAddBlend)] =
			std::make_unique<AddbBlendSpriteShader>(device.Get());
		spriteShaders[static_cast<int>(SpriteShaderId::DefaultSubtractBlend)] =
			std::make_unique<SubtractBlendSpriteShader>(device.Get());
		spriteShaders[static_cast<int>(SpriteShaderId::DepthOfField)] =
			std::make_unique<DepthOfFieldShader>(device.Get());
		spriteShaders[static_cast<int>(SpriteShaderId::LightShaft)] =
			std::make_unique<LightShaftShader>(device.Get());
	}

	// レンダラ
	{
		debugRenderer = std::make_unique<DebugRenderer>(device.Get());
		lineRenderer = std::make_unique<LineRenderer>(device.Get(), 1024);
		imguiRenderer = std::make_unique<ImGuiRenderer>(hWnd, device.Get(), immediateContext.Get());
	}
}

// デストラクタ
Graphics::~Graphics()
{
}