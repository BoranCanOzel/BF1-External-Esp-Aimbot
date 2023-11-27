#include <thread>
#include "overlay.h"
#include "Utils\Memory\Memory.h"

Overlay Ov, * v = &Ov;
bool Run = false;
int MenuKey = VK_DELETE;
HWND GameHwnd = NULL;
HWND OverlayHwnd = nullptr;

void ManagerTH();

int main()
{
    // Mmeory Init
    if (!m.Init())
        return 0;

    // Apexのウィンドウサイズ/POINTを取得
    GameHwnd = m.FindTarget();
    GetClientRect(GameHwnd, &v->GameRect);
    ClientToScreen(GameHwnd, &v->GamePoint);
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);

    // Create Overlay
    WNDCLASSEXA wc = { sizeof(WNDCLASSEXA), 0, WndProc, 0, 0, NULL, NULL, NULL, NULL, "PythonP Software On Top", "1989-6-4", NULL };
    RegisterClassExA(&wc);
    OverlayHwnd = CreateWindowExA(NULL, wc.lpszClassName, wc.lpszMenuName, WS_POPUP | WS_VISIBLE, v->GamePoint.x, v->GamePoint.y, v->GameRect.right, v->GameRect.bottom, NULL, NULL, wc.hInstance, NULL);
    SetWindowLong(OverlayHwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);
    SetLayeredWindowAttributes(OverlayHwnd, RGB(0, 0, 0), 255, LWA_ALPHA);
    MARGINS margin = { -1 };
    DwmExtendFrameIntoClientArea(OverlayHwnd, &margin);

    if (!CreateDeviceD3D(OverlayHwnd))
    {
        CleanupDeviceD3D();
        UnregisterClassA(wc.lpszClassName, wc.hInstance);
        exit(0);
    }

    ShowWindow(OverlayHwnd, SW_SHOW);
    UpdateWindow(OverlayHwnd);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.IniFilename = NULL;

    ImGui_ImplWin32_Init(OverlayHwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    // Background color
    ImVec4 clear_color = ImVec4(1.f, 1.f, 1.f, 0.f);

    // Main loop
    Run = true;
    std::thread(ManagerTH).detach();

    while (Run)
    {
        MSG msg;
        while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        HWND ForegroundWindow = GetForegroundWindow();
        if (ForegroundWindow == GameHwnd)
        {
            HWND TempProcessHwnd = GetWindow(ForegroundWindow, GW_HWNDPREV);
            SetWindowPos(OverlayHwnd, TempProcessHwnd, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        }

        // Info
        v->m_Info();

        // Menu
        if (v->ShowMenu)
            v->m_Menu();

        // ESP
       if (g.ESP)
            v->m_ESP();

        ImGui::Render();
        const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        g_pSwapChain->Present(1, 0);
    }

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    DestroyWindow(OverlayHwnd);
    UnregisterClassA(wc.lpszClassName, wc.hInstance);

    return 0;
}

// 主にオーバーレイの管理用スレッド
void ManagerTH()
{
    while (Run)
    {
        // ゲームが実行されているかをチェックする
        GameHwnd = m.FindTarget();
        if (!GameHwnd)
            Run = false;

        // StreamProof - Checked : OBS Studio, Discord
        SetWindowDisplayAffinity(OverlayHwnd, WDA_EXCLUDEFROMCAPTURE);

        // ShowMenu
        static bool menu_key = false;
        if (IsKeyDown(MenuKey) && !menu_key)
        {
            v->ShowMenu = !v->ShowMenu;
            menu_key = true;
        }
        else if (!IsKeyDown(MenuKey) && menu_key)
        {
            menu_key = false;
        }

        // Window style changer
        if (v->ShowMenu)
            SetWindowLong(OverlayHwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TOOLWINDOW);
        else
            SetWindowLong(OverlayHwnd, GWL_EXSTYLE, WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW);

        RECT TmpRect = {};
        POINT TmpPoint = {};
        GetClientRect(GameHwnd, &TmpRect);
        ClientToScreen(GameHwnd, &TmpPoint);

        // ターゲットのウィンドウのサイズに合わせる
        if (TmpRect.left != v->GameRect.left || TmpRect.bottom != v->GameRect.bottom || TmpRect.top != v->GameRect.top || TmpRect.right != v->GameRect.right || TmpPoint.x != v->GamePoint.x || TmpPoint.y != v->GamePoint.y)
        {
            v->GameRect = TmpRect;
            SetWindowPos(OverlayHwnd, nullptr, TmpPoint.x, TmpPoint.y, v->GameRect.right, v->GameRect.bottom, SWP_NOREDRAW);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}