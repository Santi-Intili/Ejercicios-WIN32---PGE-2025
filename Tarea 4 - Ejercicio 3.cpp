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
static HBRUSH g_brPanel1 = nullptr;
static HBRUSH g_brPanel2 = nullptr;
static HBRUSH g_brPanel3 = nullptr;

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

    // Título con EN DASH y tilde usando \uXXXX
    HWND hWnd = CreateWindowExW(
        0, kClass,
        L"Portal de noticias",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 600, 600,
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
        g_brPanel2 = CreateSolidBrush(RGB(235, 248, 255));
        g_brPanel3 = CreateSolidBrush(RGB(237, 242, 237));
        g_penPanel = CreatePen(PS_SOLID, 1, RGB(185, 185, 185));
        return 0;
    }

    case WM_SIZE:
        g_cx = LOWORD(l); g_cy = HIWORD(l); InvalidateRect(hWnd, nullptr, TRUE); return 0;

    case WM_PAINT: {
        PAINTSTRUCT ps; HDC hdc = BeginPaint(hWnd, &ps);
        RECT rc; GetClientRect(hWnd, &rc);
        SetBkMode(hdc, TRANSPARENT);

        // --- TITULAR ---
        HFONT old = (HFONT)SelectObject(hdc, g_fontTitle);
        SetTextColor(hdc, RGB(20, 60, 120));
        RECT hdr = rc;
        hdr.bottom = hdr.top + 60;
        hdr.top += 8;

        // Alineacion segun ancho
        UINT flags = DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX;
        if (g_cx < 700) {
            flags |= DT_CENTER;
        }
        else {
            hdr.left += 16;
            flags |= DT_LEFT;
        }

        DrawTextW(hdc, L"Portal de noticias", -1, &hdr, flags);
        SelectObject(hdc, old);
        SetTextColor(hdc, RGB(0, 0, 0));

        // --- TEXTO DE NOTICIA ---
        HFONT oldP = (HFONT)SelectObject(hdc, g_fontPanel);
        RECT t3 = rc;
        t3.top += 100;
        t3.left += 20;
        t3.right -= 20;

        const wchar_t* noticia =
            L"El Real Madrid ficho a un joven talento Sub 20.\n";

        // Reusar misma logica de alineacion
        UINT flagsNoticia = DT_SINGLELINE | DT_NOPREFIX;
        if (g_cx < 700) {
            flagsNoticia |= DT_CENTER;
        }
        else {
            flagsNoticia |= DT_LEFT;
        }

        DrawTextW(hdc, noticia, -1, &t3, flagsNoticia);

        // --- DIMENSIONES EN ESQUINA ---
        std::wstringstream dbg;
        dbg << g_cx << L"x" << g_cy;
        RECT dbgRect = rc;
        dbgRect.left = dbgRect.right - 200;
        dbgRect.top = dbgRect.bottom - 30;
        DrawTextW(hdc, dbg.str().c_str(), -1, &dbgRect, DT_RIGHT | DT_BOTTOM | DT_SINGLELINE);

        SelectObject(hdc, oldP);
        EndPaint(hWnd, &ps);
        return 0;
    }



    case WM_DESTROY:
        if (g_fontTitle) { DeleteObject(g_fontTitle);  g_fontTitle = nullptr; }
        if (g_fontPanel) { DeleteObject(g_fontPanel);  g_fontPanel = nullptr; }
        if (g_penPanel) { DeleteObject(g_penPanel);   g_penPanel = nullptr; }
        if (g_brPanel1) { DeleteObject(g_brPanel1);   g_brPanel1 = nullptr; }
        if (g_brPanel2) { DeleteObject(g_brPanel2);   g_brPanel2 = nullptr; }
        if (g_brPanel3) { DeleteObject(g_brPanel3);   g_brPanel3 = nullptr; }
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hWnd, m, w, l);
}
