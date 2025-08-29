// Win32 + Unicode (a prueba de encoding) — paneles diferenciados en español
// Compilar como Windows Application. Recomendado: /utf-8 y guardar como UTF-8 (con BOM).

#define UNICODE
#define _UNICODE
#define WIN32_LEAN_AND_MEAN
#pragma execution_character_set("utf-8") // no hace daño; los escapes evitan mojibake

#include <windows.h>
#include <string>
#include <cwctype>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static std::wstring g_typed;

static HFONT  g_fontTitle = nullptr;
static HFONT  g_fontPanel = nullptr;
static HPEN   g_penPanel = nullptr;
static HBRUSH g_brPanel1 = nullptr;

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

    HWND hWnd = CreateWindowExW(
        0, kClass,
        L"PGE",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 960, 600,
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
        g_brPanel1 = CreateSolidBrush(RGB(255, 245, 230));
        g_penPanel = CreatePen(PS_SOLID, 1, RGB(185, 185, 185));
        return 0;
    }

    case WM_CHAR: {
        wchar_t ch = (wchar_t)w;
        if (ch == L'\b') { if (!g_typed.empty()) g_typed.pop_back(); }
        else if (iswprint(ch)) g_typed.push_back(ch);
        InvalidateRect(hWnd, nullptr, TRUE);
        return 0;
    }

    case WM_LBUTTONDOWN:
        InvalidateRect(hWnd, nullptr, TRUE);
        return 0;

    case WM_SIZE:
        InvalidateRect(hWnd, nullptr, TRUE);
        return 0;

    case WM_PAINT: {
        PAINTSTRUCT ps; HDC hdc = BeginPaint(hWnd, &ps);
        RECT rc; GetClientRect(hWnd, &rc);
        SetBkMode(hdc, TRANSPARENT);

        HFONT old = (HFONT)SelectObject(hdc, g_fontTitle);
        SetTextColor(hdc, RGB(20, 60, 120));
        RECT hdr = rc; hdr.bottom = hdr.top + 60; hdr.left += 16; hdr.top += 8;
        DrawTextW(hdc, L"PGE",
            -1, &hdr, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
        SelectObject(hdc, old);
        SetTextColor(hdc, RGB(0, 0, 0));

        const int M = 16;
        RECT area = rc; area.top += 72; area.left += M; area.right -= M; area.bottom -= M;

        RECT p1 = area;
        p1.right = area.left + (area.right - area.left) / 2 - 8;
        p1.bottom = area.top + 190;
        DrawPanelFrame(hdc, p1, g_brPanel1, L"Tarea 1");

        HFONT oldP = (HFONT)SelectObject(hdc, g_fontPanel);
        RECT t1 = p1; t1.left += 12; t1.top += 36; t1.right -= 12;

        const wchar_t* demoEsp = L"Nombre del huesped:";
        std::wstring linea = L"Nombre: " + (g_typed.empty() ? L"(vac\u00EDo)" : g_typed);
        DrawTextW(hdc, demoEsp, -1, &t1, DT_LEFT | DT_WORDBREAK | DT_NOPREFIX);
        t1.top += 52;
        DrawTextW(hdc, linea.c_str(), -1, &t1, DT_LEFT | DT_WORDBREAK | DT_NOPREFIX);
        SelectObject(hdc, oldP);

        EndPaint(hWnd, &ps);
        return 0;
    }

    case WM_DESTROY:
        if (g_fontTitle) { DeleteObject(g_fontTitle);  g_fontTitle = nullptr; }
        if (g_fontPanel) { DeleteObject(g_fontPanel);  g_fontPanel = nullptr; }
        if (g_penPanel) { DeleteObject(g_penPanel);   g_penPanel = nullptr; }
        if (g_brPanel1) { DeleteObject(g_brPanel1);   g_brPanel1 = nullptr; }
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hWnd, m, w, l);
}
