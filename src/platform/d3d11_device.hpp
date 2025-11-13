#pragma once

#include <d3d11.h>
#include <windows.h>

class D3D11Device {
public:
    ~D3D11Device();

    bool initialize(HWND hwnd);
    void shutdown();

    void begin_frame();
    void end_frame();
    void present();

    ID3D11Device* device() const { return m_device; }
    ID3D11DeviceContext* context() const { return m_context; }

private:
    bool create_render_target();
    void cleanup_render_target();

    IDXGISwapChain* m_swap_chain = nullptr;
    ID3D11Device* m_device = nullptr;
    ID3D11DeviceContext* m_context = nullptr;
    ID3D11RenderTargetView* m_main_render_target = nullptr;
};
