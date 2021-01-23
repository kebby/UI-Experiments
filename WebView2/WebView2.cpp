// XAMLIslands.cpp : Defines the entry point for the application.
//

#include <SDKDDKVer.h>

#include <windows.h>
#include <stdlib.h>
#include <string.h>

#include <wrl.h>
#include <wil/com.h>
#include "WebView2.h"

#include "resource.h"

using namespace Microsoft::WRL;

// Global Variables:
HINSTANCE hInst;
WCHAR szTitle[256];                  // The title bar text
WCHAR szWindowClass[256];            // the main window class name


// Pointer to WebViewController
static wil::com_ptr<ICoreWebView2Controller> webviewController;

// Pointer to WebView window
static wil::com_ptr<ICoreWebView2> webviewWindow;


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        /*
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    */
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: Add any drawing code that uses hdc here...
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_SIZE:
    {
        if (webviewController != nullptr) {
            RECT bounds;
            GetClientRect(hWnd, &bounds);
            webviewController->put_Bounds(bounds);
        };    
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


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    hInst = hInstance;

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, 256);
    LoadStringW(hInstance, IDC_WEBVIEW2, szWindowClass, 256);

    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WEBVIEW2));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    RegisterClassExW(&wcex);

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }


    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // Step 3 - Create a single WebView within the parent window
    // Locate the browser and set up the environment for WebView
    CreateCoreWebView2EnvironmentWithOptions(nullptr, nullptr, nullptr,
        Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
            [hWnd](HRESULT result, ICoreWebView2Environment* env) -> HRESULT {

        // Create a CoreWebView2Controller and get the associated CoreWebView2 whose parent is the main window hWnd
        env->CreateCoreWebView2Controller(hWnd, Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
            [hWnd](HRESULT result, ICoreWebView2Controller* controller) -> HRESULT {
            if (controller != nullptr) {
                webviewController = controller;
                webviewController->get_CoreWebView2(&webviewWindow);
            }

            // Add a few settings for the webview
            // The demo step is redundant since the values are the default settings
            ICoreWebView2Settings* Settings;
            webviewWindow->get_Settings(&Settings);
            Settings->put_IsScriptEnabled(TRUE);
            Settings->put_AreDefaultScriptDialogsEnabled(TRUE);
            Settings->put_IsWebMessageEnabled(TRUE);

            // Resize WebView to fit the bounds of the parent window
            RECT bounds;
            GetClientRect(hWnd, &bounds);
            webviewController->put_Bounds(bounds);

            // Schedule an async task to navigate to Bing
            webviewWindow->Navigate(L"https://www.bing.com/");

            // Step 4 - Navigation events

            // Step 5 - Scripting

            // Step 6 - Communication between host and web content

            return S_OK;
        }).Get());
        return S_OK;
    }).Get());


    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WEBVIEW2));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

