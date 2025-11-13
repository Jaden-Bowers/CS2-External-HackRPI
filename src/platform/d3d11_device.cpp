#include "d3d11_device.hpp"

#pragma comment(lib, "d3d11.lib")

D3D11Device::~D3D11Device() {
    shutdown();
}

bool D3D11Device::initialize(HWND hwnd) {
    DXGI_SWAP_CHAIN_DESC sd{};
    sd.BufferCount = 2;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hwnd;
    sd.SampleDesc.Count = 1;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT flags = 0;
    const D3D_FEATURE_LEVEL feature_levels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };
    D3D_FEATURE_LEVEL created_level;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr,
                                               D3D_DRIVER_TYPE_HARDWARE,
                                               nullptr,
                                               flags,
                                               feature_levels,
                                               ARRAYSIZE(feature_levels),
                                               D3D11_SDK_VERSION,
                                               &sd,
                                               &m_swap_chain,
                                               &m_device,
                                               &created_level,
                                               &m_context);
    if (FAILED(hr))
        return false;

    return create_render_target();
}

void D3D11Device::shutdown() {
    cleanup_render_target();
    if (m_swap_chain) { m_swap_chain->Release(); m_swap_chain = nullptr; }
    if (m_context) { m_context->Release(); m_context = nullptr; }
    if (m_device) { m_device->Release(); m_device = nullptr; }
}

void D3D11Device::begin_frame() {
    const float clear_color[4] = { 0.f, 0.f, 0.f, 0.f };
    m_context->OMSetRenderTargets(1, &m_main_render_target, nullptr);
    m_context->ClearRenderTargetView(m_main_render_target, clear_color);
}

void D3D11Device::end_frame() {
    // nothing for now
}

void D3D11Device::present() {
    if (m_swap_chain)
        m_swap_chain->Present(1, 0);
}

bool D3D11Device::create_render_target() {
    ID3D11Texture2D* back_buffer = nullptr;
    if (FAILED(m_swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer))))
        return false;

    const HRESULT hr = m_device->CreateRenderTargetView(back_buffer, nullptr, &m_main_render_target);
    back_buffer->Release();
    return SUCCEEDED(hr);
}

void D3D11Device::cleanup_render_target() {
    if (m_main_render_target) {
        m_main_render_target->Release();
        m_main_render_target = nullptr;
    }
}