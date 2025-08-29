#include <windows.h>
#include <string>
#include <cwctype>   // para iswdigit

// Texto que guarda el importe
std::wstring importe = L"";

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CHAR: {
        wchar_t ch = (wchar_t)wParam;

        if (iswdigit(ch)) {
            // Si es un número, lo añadimos
            importe.push_back(ch);
            InvalidateRect(hwnd, NULL, TRUE);
        }
        else if (ch == L',' || ch == L'.') {
            // Permitir coma o punto, pero solo uno
            if (importe.find(L',') == std::wstring::npos &&
                importe.find(L'.') == std::wstring::npos) {
                importe.push_back(L','); // lo normalizamos a coma
                InvalidateRect(hwnd, NULL, TRUE);
            }
        }
        else if (ch == 8) { // Backspace
            if (!importe.empty()) {
                importe.pop_back();
                InvalidateRect(hwnd, NULL, TRUE);
            }
        }
        // Si no es válido, lo ignoramos
        return 0;
    }
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        std::wstring texto = L"Importe: " + importe;
        TextOutW(hdc, 20, 20, texto.c_str(), texto.size());

        EndPaint(hwnd, &ps);
        break;
    }//PRUEBA CAMBIO
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"CajaRegistradoraWin32";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"Caja Registradora - Solo Números",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 200,
        NULL,
        NULL,
        hInstance,
        NULL
    );//AGREGA EL CAMBIO
    //AGREGO CAMBIO
    //PRIMER CAMBIO
    if (hwnd == NULL) return 0;

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
