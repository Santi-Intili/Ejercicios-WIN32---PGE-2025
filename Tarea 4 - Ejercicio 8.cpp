// PanelMostrador.cpp — Win32/Unicode
// Compilar (MSVC):  cl /EHsc /DUNICODE /D_UNICODE PanelMostrador.cpp user32.lib gdi32.lib

#define UNICODE
#define _UNICODE
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <string>
#include <sstream>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// ---------- Estado (modelo) ----------
static std::wstring g_nombre;     // Se arma con WM_CHAR (BKSP soportado)
static int          g_clicks = 0; // Se incrementa con WM_LBUTTONDOWN
static int          g_w = 0, g_h = 0;  // Tamaño cliente (WM_SIZE)

// Layout de los 3 paneles
enum { IDX_NOMBRE = 0, IDX_CLICS = 1, IDX_TAM = 2, PANELS = 3 };
static RECT   g_rc[PANELS];

// Recursos de presentación
static HFONT  g_fontLabel = nullptr;
static HFONT  g_fontValue = nullptr;
static HPEN   g_penFrame = nullptr;
static HBRUSH g_brNombre = nullptr;
static HBRUSH g_brClicks = nullptr;
static HBRUSH g_brTam = nullptr;

// ---------- Utilidades de presentación ----------
void RecalcLayout(HWND hWnd)
{
    RECT rc; GetClientRect(hWnd, &rc);
    g_w = rc.right - rc.left;
    g_h = rc.bottom - rc.top;

    const int pad = 12;
    const int gap = 10;
    const int wBox = (g_w - pad * 2 - gap * 2) / 3;
    const int hBox = (g_h - pad * 2);

    for (int i = 0; i < PANELS; ++i) {
        int L = pad + i * (wBox + gap);
        g_rc[i] = { L, pad, L + wBox, pad + hBox };
    }
}

void DrawPanel(HDC hdc, const RECT& r, HBRUSH bg, LPCWSTR rotulo, LPCWSTR valor)
{
    // Fondo
    FillRect(hdc, &r, bg);

    // Borde redondeado
    HPEN oldPen = (HPEN)SelectObject(hdc, g_penFrame);
    HBRUSH oldBr = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
    RoundRect(hdc, r.left, r.top, r.right, r.bottom, 12, 12);
    SelectObject(hdc, oldBr);
    SelectObject(hdc, oldPen);

    SetBkMode(hdc, TRANSPARENT);

    // Rótulo (arriba-izq)
    RECT rLab = r; rLab.left += 12; rLab.top += 8;
    HFONT oldF = (HFONT)SelectObject(hdc, g_fontLabel);
    SetTextColor(hdc, RGB(80, 80, 80));
    DrawTextW(hdc, rotulo, -1, &rLab, DT_LEFT | DT_TOP | DT_SINGLELINE | DT_NOPREFIX);

    // Valor (centrado)
    RECT rVal = r; InflateRect(&rVal, -12, -12);
    SelectObject(hdc, g_fontValue);
    SetTextColor(hdc, RGB(25, 25, 25));
    DrawTextW(hdc, valor, -1, &rVal, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);

    SelectObject(hdc, oldF);
}

// Convierte número a wstring
template<typename T>
std::wstring to_w(T v) { std::wstringstream ss; ss << v; return ss.str(); }

// ---------- WinMain ----------
int APIENTRY wWinMain(HINSTANCE hInst, HINSTANCE, LPWSTR, int nCmdShow)
{
    const wchar_t* kClass = L"PanelMostradorClass";

    WNDCLASSW wc{};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = kClass;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    if (!RegisterClassW(&wc)) return 0;

    HWND hWnd = CreateWindowW(
        kClass, L"Integración mini — Panel de mostrador",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 900, 260,
        nullptr, nullptr, hInst, nullptr);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return (int)msg.wParam;
}

// ---------- Window Proc ----------
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
    {
        // Fuentes (tamaño relativo a 96 DPI)
        LOGFONTW lf{}; lf.lfCharSet = DEFAULT_CHARSET; lf.lfQuality = CLEARTYPE_NATURAL_QUALITY;
        wcscpy_s(lf.lfFaceName, L"Segoe UI");

        lf.lfHeight = -16; g_fontLabel = CreateFontIndirectW(&lf);
        lf.lfHeight = -28; g_fontValue = CreateFontIndirectW(&lf);

        g_penFrame = CreatePen(PS_SOLID, 1, RGB(90, 90, 90));
        g_brNombre = CreateSolidBrush(RGB(232, 244, 250)); // celeste suave
        g_brClicks = CreateSolidBrush(RGB(241, 233, 223)); // durazno
        g_brTam = CreateSolidBrush(RGB(226, 242, 226)); // verde claro

        RecalcLayout(hWnd);
        return 0;
    }

    case WM_SIZE:
        RecalcLayout(hWnd);          // actualiza g_w, g_h y rects
        InvalidateRect(hWnd, nullptr, TRUE); // repinta (interfaz clara al redimensionar)
        return 0;

    case WM_CHAR:
    {
        wchar_t ch = (wchar_t)wParam;
        if (ch == 0x08) { // Backspace
            if (!g_nombre.empty()) g_nombre.pop_back();
        }
        else if (ch >= 32) {
            g_nombre.push_back(ch);
        }
        InvalidateRect(hWnd, &g_rc[IDX_NOMBRE], TRUE);
        return 0;
    }

    case WM_LBUTTONDOWN:
        ++g_clicks;
        InvalidateRect(hWnd, &g_rc[IDX_CLICS], TRUE);
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps; HDC hdc = BeginPaint(hWnd, &ps);

        // Valores actuales
        std::wstring vNombre = g_nombre.empty() ? L"—" : g_nombre;
        std::wstring vClicks = to_w(g_clicks);
        std::wstring vTam = to_w(g_w) + L" × " + to_w(g_h);

        DrawPanel(hdc, g_rc[IDX_NOMBRE], g_brNombre, L"Nombre (teclado)", vNombre.c_str());
        DrawPanel(hdc, g_rc[IDX_CLICS], g_brClicks, L"Entradas (clics)", vClicks.c_str());
        DrawPanel(hdc, g_rc[IDX_TAM], g_brTam, L"Tamaño (W×H)", vTam.c_str());

        EndPaint(hWnd, &ps);
        return 0;
    }

    case WM_DESTROY:
        if (g_fontLabel) DeleteObject(g_fontLabel);
        if (g_fontValue) DeleteObject(g_fontValue);
        if (g_penFrame)  DeleteObject(g_penFrame);
        if (g_brNombre)  DeleteObject(g_brNombre);
        if (g_brClicks)  DeleteObject(g_brClicks);
        if (g_brTam)     DeleteObject(g_brTam);
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}
