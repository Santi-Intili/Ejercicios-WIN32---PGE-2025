// Win32 + Unicode (a prueba de encoding) — paneles diferenciados en español
// Compilar como Windows Application. Recomendado: /utf-8 y guardar como UTF-8 (con BOM).

#define UNICODE
#define _UNICODE
#define WIN32_LEAN_AND_MEAN
#pragma execution_character_set("utf-8")

#include <windows.h>
#include <string>
#include <sstream>
#include <cwctype>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static std::wstring g_typed;
static int          g_clicks = 0;
static int          g_cx = 0, g_cy = 0;

static HFONT  g_fontTitle = nullptr;
static HFONT  g_fontPanel = nullptr;
static HPEN   g_penPanel = nullptr;
static HBRUSH g_brPanel2 = nullptr;

void DrawPanelFrame(HDC hdc, const RECT& r, HBRUSH bg, LPCWSTR titulo) {
    FillRect(hdc, &r, bg);
    HPEN   oldPen = (HPEN)SelectObject(hdc, g_penPanel);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
    Rectangle(hdc, r.left, r.top, r.right, r.bottom);
    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);

    HFONT old = (HFONT)SelectObject(hdc, g_fontPanel);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(25, 25, 25));
    RECT rt = r; rt.bottom = rt.top + 26; rt.left += 10;
    DrawTextW(hdc, titulo, -1, &rt, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
    SelectObject(hdc, old);
}

int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE, PWSTR, int nCmdShow) {
    const wchar_t* kClass = L"PGE_Unidad1_UI_Espanol";

    WNDCLASSEXW wc{ sizeof(wc) };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = kClass;
    RegisterClassExW(&wc);

    // 🔹 Ventana emergente normal (con barra de título, redimensionable)
    HWND hWnd = CreateWindowExW(
        0, kClass,
        L"Taquilla de cine",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE, // ventana normal
        CW_USEDEFAULT, CW_USEDEFAULT,
        800, 600,                          // tamaño inicial
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
    switch (m) {
    case WM_CREATE: {
        g_fontTitle = CreateFontW(
            30, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
            OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
            DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
        g_fontPanel = CreateFontW(
            18, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
            DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
        g_brPanel2 = CreateSolidBrush(RGB(235, 248, 255));
        g_penPanel = CreatePen(PS_SOLID, 1, RGB(185, 185, 185));
        return 0;
    }

    case WM_LBUTTONDOWN:
        ++g_clicks; InvalidateRect(hWnd, nullptr, TRUE); return 0;

    case WM_RBUTTONDOWN:
        if (g_clicks > 0) { --g_clicks; InvalidateRect(hWnd, nullptr, TRUE); return 0; }

    case WM_KEYDOWN:
        if (w == VK_ESCAPE) { // salir con ESC
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            return 0;
        }
        break;

    case WM_SIZE:
        g_cx = LOWORD(l); g_cy = HIWORD(l); InvalidateRect(hWnd, nullptr, TRUE); return 0;

    case WM_PAINT: {
        PAINTSTRUCT ps; HDC hdc = BeginPaint(hWnd, &ps);
        RECT rc; GetClientRect(hWnd, &rc);
        SetBkMode(hdc, TRANSPARENT);

        // Cabecera
        HFONT old = (HFONT)SelectObject(hdc, g_fontTitle);
        SetTextColor(hdc, RGB(20, 60, 120));
        RECT hdr = rc; hdr.bottom = hdr.top + 60; hdr.left += 16; hdr.top += 8;
        DrawTextW(hdc, L"Taquilla de cine",
            -1, &hdr, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
        SelectObject(hdc, old);
        SetTextColor(hdc, RGB(0, 0, 0));

        // Layout (área disponible bajo la cabecera)
        const int M = 16;
        RECT area = rc;
        area.top += 72;
        area.left += M;
        area.right -= M;
        area.bottom -= M;

        HFONT oldP = (HFONT)SelectObject(hdc, g_fontPanel);

        // 🔹 Panel 2 ocupa todo el área
        RECT p2 = area;
        DrawPanelFrame(hdc, p2, g_brPanel2, L"Instrucciones de venta");

        // --- Texto dentro del panel ---
        RECT textArea = p2;
        textArea.left += 12;
        textArea.top += 36;
        textArea.right -= 12;

        DrawTextW(hdc,
            L"Cada vez que haga un click izquierdo, se incrementara en 1 la cantidad de entradas vendidas.",
            -1, &textArea, DT_LEFT | DT_WORDBREAK | DT_NOPREFIX);

        textArea.top += 60;

        DrawTextW(hdc,
            L"Cada vez que haga un click derecho, se decrementara en 1 la cantidad de entradas vendidas.",
            -1, &textArea, DT_LEFT | DT_WORDBREAK | DT_NOPREFIX);

        textArea.top += 60;

        std::wstringstream s2;
        s2 << L"Entradas: " << g_clicks;
        DrawTextW(hdc, s2.str().c_str(), -1, &textArea,
            DT_LEFT | DT_SINGLELINE | DT_NOPREFIX);

        SelectObject(hdc, oldP);
        EndPaint(hWnd, &ps);
        return 0;
    }

    case WM_DESTROY:
        if (g_fontTitle) { DeleteObject(g_fontTitle);  g_fontTitle = nullptr; }
        if (g_fontPanel) { DeleteObject(g_fontPanel);  g_fontPanel = nullptr; }
        if (g_penPanel) { DeleteObject(g_penPanel);   g_penPanel = nullptr; }
        if (g_brPanel2) { DeleteObject(g_brPanel2);   g_brPanel2 = nullptr; }
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hWnd, m, w, l);
}
