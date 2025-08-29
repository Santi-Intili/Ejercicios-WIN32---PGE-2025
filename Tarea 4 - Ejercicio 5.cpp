// Win32 + Unicode — Panel de ascensor (último piso solicitado)
// Compilar como Windows Application. Guardar como UTF-8 con BOM.

#define UNICODE
#define _UNICODE
#define WIN32_LEAN_AND_MEAN
#pragma execution_character_set("utf-8")
#include <windowsx.h>   // Necesario para GET_X_LPARAM y GET_Y_LPARAM


#include <windows.h>
#include <string>
#include <sstream>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static int g_piso = 0;   // Piso seleccionado: 0=ninguno, 1,2,3
static int g_cx = 0, g_cy = 0;

static HFONT g_font = nullptr;

int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE, PWSTR, int nCmdShow) {
    const wchar_t* kClass = L"AscensorWin32";

    WNDCLASSEXW wc{ sizeof(wc) };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = kClass;
    RegisterClassExW(&wc);

    HWND hWnd = CreateWindowExW(
        0, kClass,
        L"Panel de ascensor — Ultimo piso solicitado",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 600, 400,
        nullptr, nullptr, hInst, nullptr);
    if (!hWnd) return 0;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT m, WPARAM w, LPARAM l) {
    static RECT btn1, btn2, btn3;

    switch (m) {
    case WM_CREATE: {
        g_font = CreateFontW(
            28, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
            DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
        return 0;
    }

    case WM_SIZE: {
        g_cx = LOWORD(l); g_cy = HIWORD(l);

        int ancho = g_cx / 3;
        int alto = g_cy / 3;
        btn1 = { 0, 40, ancho, 40 + alto };
        btn2 = { ancho, 40, 2 * ancho, 40 + alto };
        btn3 = { 2 * ancho, 40, 3 * ancho, 40 + alto };

        InvalidateRect(hWnd, nullptr, TRUE);
        return 0;
    }

    case WM_LBUTTONDOWN: {
        POINT p = { GET_X_LPARAM(l), GET_Y_LPARAM(l) };
        if (PtInRect(&btn1, p)) g_piso = 1;
        else if (PtInRect(&btn2, p)) g_piso = 2;
        else if (PtInRect(&btn3, p)) g_piso = 3;
        InvalidateRect(hWnd, nullptr, TRUE);
        return 0;
    }

    case WM_PAINT: {
        PAINTSTRUCT ps; HDC hdc = BeginPaint(hWnd, &ps);

        HBRUSH brNorm = CreateSolidBrush(RGB(230, 230, 230));
        HBRUSH brSel = CreateSolidBrush(RGB(180, 220, 255));
        HFONT oldF = (HFONT)SelectObject(hdc, g_font);

        // Dibujar botones
        auto drawBtn = [&](RECT r, int num) {
            FillRect(hdc, &r, (g_piso == num ? brSel : brNorm));
            FrameRect(hdc, &r, (HBRUSH)GetStockObject(BLACK_BRUSH));
            std::wstring txt = std::to_wstring(num);
            DrawTextW(hdc, txt.c_str(), -1, &r,
                DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            };

        drawBtn(btn1, 1);
        drawBtn(btn2, 2);
        drawBtn(btn3, 3);

        // Mostrar último piso
        RECT info; GetClientRect(hWnd, &info);
        info.top = btn1.bottom + 40;
        std::wstringstream s;
        s << L"Ultimo piso: " << (g_piso ? std::to_wstring(g_piso) : L"(ninguno)");
        DrawTextW(hdc, s.str().c_str(), -1, &info,
            DT_CENTER | DT_SINGLELINE);

        SelectObject(hdc, oldF);
        DeleteObject(brNorm);
        DeleteObject(brSel);
        EndPaint(hWnd, &ps);
        return 0;
    }

    case WM_DESTROY:
        if (g_font) { DeleteObject(g_font); g_font = nullptr; }
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hWnd, m, w, l);
}
