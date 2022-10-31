// Pinball.cpp : Definisce il punto di ingresso dell'applicazione.
//

#define _CRT_RAND_S

#include "stdlib.h"
#include "pch.h"
#include "framework.h"
#include "Pinball.h"

#define MAX_LOADSTRING 100

// Variabili globali:
HINSTANCE hInst;                                // istanza corrente
WCHAR szTitle[MAX_LOADSTRING];                  // Testo della barra del titolo
WCHAR szWindowClass[MAX_LOADSTRING];            // nome della classe della finestra principale
ULARGE_INTEGER oraPrec, ultDis;
Palla pallaUgo;

// Dichiarazioni con prototipo di funzioni incluse in questo modulo di codice:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Inserire qui il codice.

    // Inizializzare le stringhe globali
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_PINBALL, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Eseguire l'inizializzazione dall'applicazione:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PINBALL));

    MSG msg;

    // Ciclo di messaggi principale:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNZIONE: MyRegisterClass()
//
//  SCOPO: Registra la classe di finestre.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PINBALL));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_PINBALL);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNZIONE: InitInstance(HINSTANCE, int)
//
//   SCOPO: Salva l'handle di istanza e crea la finestra principale
//
//   COMMENTI:
//
//        In questa funzione l'handle di istanza viene salvato in una variabile globale e
//        viene creata e visualizzata la finestra principale del programma.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Archivia l'handle di istanza nella variabile globale

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

ULARGE_INTEGER leggiOra()
{
    ULARGE_INTEGER valTempo;
    FILETIME oraAttuale;

    // legge l'ora attuale
    GetSystemTimeAsFileTime(&oraAttuale);
    // carica l'ora in un ULARGE_INTEGER per operazioni future
    valTempo.LowPart = oraAttuale.dwLowDateTime;
    valTempo.HighPart = oraAttuale.dwHighDateTime;

    return valTempo;
}

ULARGE_INTEGER DisegnaPallina(HWND hWnd, HDC hdc)
{
    ULARGE_INTEGER valTempo;
    LONG deltaX, deltaY;
    CHAR strTempo[40];
    SIZE_T a;
    WCHAR strTempo1[40];
    PCWSTR testo;
    RECT rectFinestra;
    unsigned int number;
    
    GetClientRect(hWnd, &rectFinestra);
    // legge l'ora attuale
    valTempo = leggiOra();
    // crea un numero casuale
    number = rand();
    number = number % 200;
    // prepara la stringa da scrivere
    // _ui64toa_s(valTempo.QuadPart, strTempo, 30, 10);
    _itoa_s(pallaUgo.velY, strTempo, 10);
    mbstowcs_s(&a, strTempo1, 30, strTempo, 30);
    testo = strTempo1;
    // scrive la stringa
    DrawText(hdc, testo, -1, &rectFinestra, DT_SINGLELINE);
    // disegna la pallina
    if (ultDis.QuadPart > 0) {
        // verifica se è passato abbastanza tempo da richiedere un refresh
        deltaX = pallaUgo.velX * LONG (valTempo.QuadPart - ultDis.QuadPart) / 10000000;
        deltaY = pallaUgo.velY * LONG (valTempo.QuadPart - ultDis.QuadPart) / 10000000;
        if ((deltaX != 0) || (deltaY != 0)) {
            pallaUgo.x += deltaX;
            pallaUgo.y += deltaY;
            // controlla se deve rimbalzare
            deltaX = rectFinestra.right - (pallaUgo.x + pallaUgo.diam);
            if (deltaX < 0) {
                pallaUgo.x += deltaX;
                pallaUgo.velX = -pallaUgo.velX+number;
            }
            deltaY = rectFinestra.bottom - (pallaUgo.y + pallaUgo.diam);
            if (deltaY < 0) {
                pallaUgo.y += deltaY;
                pallaUgo.velY = -pallaUgo.velY+number;
            }
            if (pallaUgo.x < 0) {
                pallaUgo.x = -pallaUgo.x;
                pallaUgo.velX = -pallaUgo.velX+number;
            }
            if (pallaUgo.y < 0) {
                pallaUgo.y = -pallaUgo.y;
                pallaUgo.velY = -pallaUgo.velY+number;
            }
            ultDis = valTempo;
        }
    }
    else {
        ultDis = valTempo;
    }
    Rectangle(hdc, pallaUgo.x, pallaUgo.y, pallaUgo.x + pallaUgo.diam, pallaUgo.y + pallaUgo.diam);
    return valTempo;
}
//
//  FUNZIONE: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  SCOPO: Elabora i messaggi per la finestra principale.
//
//  WM_COMMAND  - elabora il menu dell'applicazione
//  WM_PAINT    - Disegna la finestra principale
//  WM_DESTROY  - inserisce un messaggio di uscita e restituisce un risultato
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    switch (message)
    {
    case WM_NCCREATE:
        {
        oraPrec = leggiOra();
        ultDis.QuadPart = 0;
        pallaUgo.diam = 20;
        pallaUgo.x = 0;
        pallaUgo.y = 0;
        pallaUgo.velX = 500;
        pallaUgo.velY = 500;
        }
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Analizzare le selezioni di menu:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Aggiungere qui il codice di disegno che usa HDC...
            oraPrec = DisegnaPallina(hWnd, hdc);
            EndPaint(hWnd, &ps);
            // manda messaggio WM_USER per controllare il tempo passato
            PostMessageA(hWnd, WM_USER, NULL, NULL);
        }
        break;
    case WM_USER:
        {
        // controlla se è passato un secondo
        if ((leggiOra().QuadPart - oraPrec.QuadPart) > 100000)
        {
            RedrawWindow(hWnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE);
        }
        else //se non è passato un secondo chiede un nuovo controllo
        {
            PostMessageA(hWnd, WM_USER, NULL, NULL);
        }

        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Gestore di messaggi per la finestra Informazioni su.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
