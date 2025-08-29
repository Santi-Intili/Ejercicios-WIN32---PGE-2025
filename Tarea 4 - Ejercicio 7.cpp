#include <windows.h>
#include <windowsx.h>   // Necesario para GET_X_LPARAM y GET_Y_LPARAM
#include <string>
#include <deque>
#include <sstream>

// ---------------- Estructura para guardar un evento ----------------
struct Item {
    std::wstring texto;   // Texto descriptivo del evento (ej: "WM_CHAR 'a'")
};

// Cola de eventos (máximo 10)
std::deque<Item> eventos;

// ---------------- Función para añadir eventos ----------------
void AgregarEvento(const std::wstring& texto) {
    // Insertamos el evento más reciente al inicio
    eventos.push_front({ texto });

    // Si hay más de 10, eliminamos el más viejo (al final)
    if (eventos.size() > 10) {
        eventos.pop_back();
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {

        // Evento: el usuario presiona una tecla de texto
    case WM_CHAR: {
        wchar_t ch = (wchar_t)wParam; // carácter presionado
        std::wstringstream ss;
        ss << L"WM_CHAR '" << ch << L"'";
        AgregarEvento(ss.str());              // lo guardamos
        InvalidateRect(hwnd, NULL, TRUE);     // pedimos repintar ventana
        break;
    }

                // Evento: el usuario hace clic izquierdo
    case WM_LBUTTONDOWN: {
        int x = GET_X_LPARAM(lParam); // coordenada X e Y del click
        int y = GET_Y_LPARAM(lParam); 
        std::wstringstream ss;
        ss << L"WM_LBUTTONDOWN (x=" << x << L", y=" << y << L")";
        AgregarEvento(ss.str());
        InvalidateRect(hwnd, NULL, TRUE);
        break;
    }

                       // Evento: la ventana cambia de tamaño
    case WM_SIZE: {
        int ancho = LOWORD(lParam);
        int alto = HIWORD(lParam);
        std::wstringstream ss;
        ss << L"WM_SIZE " << ancho << L"x" << alto;
        AgregarEvento(ss.str());
        InvalidateRect(hwnd, NULL, TRUE);
        break;
    }

                // Evento: repintado de la ventana
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        int y = 20; // posición vertical inicial
        for (const auto& e : eventos) {
            // Dibujamos cada evento, de arriba hacia abajo
            TextOutW(hdc, 20, y, e.texto.c_str(), (int)e.texto.size());
            y += 20; // bajamos 20 píxeles para el siguiente
        }

        EndPaint(hwnd, &ps);
        break;
    }

                 // Evento: cierre de la ventana
    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"ZonaDePruebasWin32";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;                  
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"Ejercicio 7 - Tabla de eventos visible",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 600, 400,
        NULL, NULL, hInstance, NULL
    );

    if (hwnd == NULL) return 0;

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
