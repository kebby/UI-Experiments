// XAMLIslands.cpp : Defines the entry point for the application.
//

#include <SDKDDKVer.h>

#include <windows.h>
#include <stdlib.h>
#include <string.h>

#include "resource.h"

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.system.h>
#include <winrt/windows.ui.xaml.hosting.h>
#include <windows.ui.xaml.hosting.desktopwindowxamlsource.h>
#include <winrt/windows.ui.xaml.controls.h>
#include <winrt/Windows.ui.xaml.media.h>

using namespace winrt;
using namespace Windows::UI;
using namespace Windows::UI::Composition;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Hosting;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::Foundation::Numerics;

// Global Variables:
HINSTANCE hInst;
WCHAR szTitle[256];                  // The title bar text
WCHAR szWindowClass[256];            // the main window class name
HWND hWndXamlIsland;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
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
		RECT client;
		GetClientRect(hWnd, &client);
		SetWindowPos(hWndXamlIsland, 0, client.left, client.top, client.right - client.left, client.bottom - client.top, SWP_SHOWWINDOW);
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
	LoadStringW(hInstance, IDC_XAMLISLANDS, szWindowClass, 256);

	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_XAMLISLANDS));
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

	// Begin XAML Island section.

 // The call to winrt::init_apartment initializes COM; by default, in a multithreaded apartment.
	winrt::init_apartment(apartment_type::multi_threaded);

	// Initialize the XAML framework's core window for the current thread.
	auto winxamlmanager = WindowsXamlManager::InitializeForCurrentThread();

	// This DesktopWindowXamlSource is the object that enables a non-UWP desktop application 
	// to host WinRT XAML controls in any UI element that is associated with a window handle (HWND).
	DesktopWindowXamlSource desktopSource;

	// Get handle to the core window.
	auto interop = desktopSource.as<IDesktopWindowXamlSourceNative>();

	// Parent the DesktopWindowXamlSource object to the current window.
	check_hresult(interop->AttachToWindow(hWnd));

	// This HWND will be the window handler for the XAML Island: A child window that contains XAML.  
	hWndXamlIsland = nullptr;

	// Get the new child window's HWND. 
	interop->get_WindowHandle(&hWndXamlIsland);

	// Update the XAML Island window size because initially it is 0,0.
	SetWindowPos(hWndXamlIsland, 0, 200, 100, 800, 200, SWP_SHOWWINDOW);

	// Create the XAML content.
	StackPanel panel;
	panel.Background(SolidColorBrush{ Colors::LightGray() });

	TextBlock tb;
	tb.Text(L"Hello World from Xaml Islands!");
	tb.VerticalAlignment(VerticalAlignment::Center);
	tb.HorizontalAlignment(HorizontalAlignment::Center);
	tb.FontSize(48);

	panel.Children().Append(tb);
	panel.UpdateLayout();


	desktopSource.Content(panel);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_XAMLISLANDS));

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

