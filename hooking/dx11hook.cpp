/**
 * @file dx11hook.cpp
 * @author ElCapor (infosec@yu-tech.cloud)
 * @brief Direct X 11 Hooking Library Implementation
 * @version 0.1
 * @date 2026-02-15
 * 
 * CC-BY-SA 4.0 Copyright (c) 2026 @ElCapor
 * 
 */
#include <hooking/dx11hook.h>
#include <comdef.h>
#include <detours.h>
#include <hooking/hookmanager.h>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

// define your own macro for encryption, in case you want one
#ifndef TXT
#define TXT(text) text
#endif

// The swap chain for dx11
IDXGISwapChain* dx11::CreateSwapChain()
{
    WNDCLASSEX wc { 0 };
	wc.cbSize = sizeof(wc);
	wc.lpfnWndProc = DefWindowProc;
	wc.lpszClassName = TXT(TEXT("dummy class"));
	RegisterClassExA(&wc);
	HWND hwnd = CreateWindow(wc.lpszClassName, TXT(TEXT("")), WS_DISABLED, 0, 0, 0, 0, NULL, NULL, NULL, nullptr);

	DXGI_SWAP_CHAIN_DESC desc{ 0 };
	desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	desc.SampleDesc.Count = 1;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.BufferCount = 1;
	desc.OutputWindow = hwnd;
	desc.Windowed = TRUE;
	desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_11_1
	};

	ID3D11Device* dummyDevice = nullptr;
	IDXGISwapChain* dummySwapChain = nullptr;
	HRESULT result = D3D11CreateDeviceAndSwapChain(
		NULL, 
		D3D_DRIVER_TYPE_HARDWARE,
		NULL, 
		0, 
		featureLevels, 
		1,
		D3D11_SDK_VERSION,
		&desc, 
		&dummySwapChain,
		&dummyDevice, 
		NULL, 
		NULL);

	DestroyWindow(desc.OutputWindow);
	UnregisterClass(wc.lpszClassName, GetModuleHandle(nullptr));
	if (FAILED(result))
	{
		_com_error err(result);
		std::cout << TXT("ERROR : ") << err.ErrorMessage() << std::endl;
		return nullptr;
	}

	return dummySwapChain;
}

void dx11::HookSwapChain(IDXGISwapChain* dummySwapChain, Present_t hookedFunction)
{
	void** vtable = *(void***)dummySwapChain;
	static Present_t oPresent = (Present_t)(vtable[8]);
	HookManager::Install(oPresent, hookedFunction);

	dummySwapChain->Release();
}

bool init = false;

IDXGISwapChain* dx11::GetSwapChain(IDXGISwapChain* swp = nullptr)
{
	static IDXGISwapChain* swap = nullptr;
	if (swp != nullptr)
	{
		swap = swp;
		return swap;
	}
	return swap;
}

ID3D11Device* dx11::GetDevice()
{
	static ID3D11Device* device = nullptr;
	if (device == nullptr)
	{
		IDXGISwapChain* swp = GetSwapChain();
		if (!swp)
			std::cout << TXT("Failed to get swap chain") << std::endl;

		if (SUCCEEDED(swp->GetDevice(__uuidof(ID3D11Device), (void**)&device)))
		{
			return device;
		}
		std::cout << TXT("Failed to get device ") << std::endl;
	}
	return device;
}

ID3D11DeviceContext* dx11::GetContext()
{
	static ID3D11DeviceContext* context = nullptr;
	if (context == nullptr)
	{
		ID3D11Device* device = GetDevice();
		if (device != nullptr)
		{
			device->GetImmediateContext(&context);
		}
	}
	if (context == nullptr)
	{
		std::cout << TXT("Failed to get context") << std::endl;
	}
	return context;
}

ID3D11RenderTargetView* dx11::GetRenderTarget()
{
	static ID3D11RenderTargetView* mainRenderTargetView = nullptr;
	if (mainRenderTargetView == nullptr)
	{
		ID3D11Device* dev = GetDevice();
		if (dev != nullptr)
		{
			ID3D11Texture2D* backBuffer = nullptr;
			IDXGISwapChain* swp = GetSwapChain();
			swp->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
			dev->CreateRenderTargetView(backBuffer, 0, &mainRenderTargetView);
			backBuffer->Release();
			return mainRenderTargetView;
		}
		std::cout << TXT("Failed to get render target") << std::endl;

	}

	return mainRenderTargetView;
}