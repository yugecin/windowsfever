#define _CRT_SECURE_NO_WARNINGS

#define DEMONAME "windows"

#define _INC_STDLIB // don't want it

//#define WIN32_LEAN_AND_MEAN
//#define WIN32_EXTRA_LEAN
#include "windows.h"
#include "dwmapi.h"
#include <GL/gl.h>
#include "glext.h"
#include "glstuff.c"
#include "frag.glsl.c"
#include "util.c"

WNDCLASSEX wcDemo = {0};
HFONT hfDefault;
int i;
#define REQUESTED_SIZE 210
#define REQUESTED_POSITION 200

#define IDC_BTN_START 3
#define IDC_BTN_FULLSCREEN 4

void error_exit_loop()
{
	MSG msg;

	while (!GetAsyncKeyState(VK_ESCAPE)) {
		while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				goto done;
			}
		}
		Sleep(2);
	}
done:
	ExitProcess(1);
}

#include "windowing.c"

void updatemetrics(HWND hWnd)
{
	RECT rcFrame, rcClient;
	MONITORINFO mi;
	POINT p;

	mi.cbSize = sizeof(mi);
	GetMonitorInfo(MonitorFromWindow(hWnd, MONITOR_DEFAULTTOPRIMARY), &mi);
	metrics.rcFull = mi.rcMonitor;
	metrics.rcWork = mi.rcWork;
	metrics.workingAreaHeight = metrics.rcWork.bottom - metrics.rcWork.top;
	metrics.workingAreaWidth = metrics.rcWork.right - metrics.rcWork.left;

	DwmGetWindowAttribute(hWnd, DWMWA_EXTENDED_FRAME_BOUNDS, &rcFrame, sizeof(rcFrame));
	metrics.reqToRealFrameSize.x = rcFrame.right - rcFrame.left - REQUESTED_SIZE;
	metrics.reqToRealFrameSize.y = rcFrame.bottom - rcFrame.top - REQUESTED_SIZE;
	metrics.reqToRealFramePos.x = rcFrame.left - REQUESTED_POSITION;
	metrics.reqToRealFramePos.y = rcFrame.top - REQUESTED_POSITION;
	GetClientRect(hWnd, &rcClient);
	p.x = 0; p.y = 0;
	ClientToScreen(hWnd, &p);
	metrics.rcBorders.left = p.x - rcFrame.left;
	metrics.rcBorders.right = rcFrame.right - rcFrame.left - rcClient.right - metrics.rcBorders.left;
	metrics.rcBorders.top = p.y - rcFrame.top;
	metrics.rcBorders.bottom = rcFrame.bottom - rcFrame.top - rcClient.bottom - metrics.rcBorders.top;
}

#include "explosion.c"
#include "demo.c"

LRESULT CALLBACK StartupWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static int didStart = 0;
	HWND control;

	switch (msg) {
	case WM_CREATE:
		control = CreateWindowExA(0, "BUTTON", "Full screen (unavailable)", BS_CHECKBOX | WS_CHILD | WS_VISIBLE | WS_DISABLED, 30, 30, 150, 25, hWnd, (HMENU) IDC_BTN_FULLSCREEN, 0, 0);
		SendMessage(control, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM(FALSE, 0));
		control = CreateWindowExA(0, "BUTTON", "Start", WS_CHILD | WS_VISIBLE, 60, 90, 80, 25, hWnd, (HMENU) IDC_BTN_START, 0, 0);
		SendMessage(control, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM(FALSE, 0));
		return 0;
	case WM_DESTROY:
		if (!didStart) {
			PostQuitMessage(0);
			return 0;
		}
	case WM_COMMAND:
		if (LOWORD(wParam) == IDC_BTN_START) {
			didStart = 1;
			updatemetrics(hWnd);
			DestroyWindow(hWnd);
			startdemo();
		}
		return 0;
	default: return DefWindowProc(hWnd, msg, wParam, lParam);
	}
}

//gcc+ld? int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
//gcc+link?  int WINAPI _WinMainCRTStartup(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
//gcc+crinkler void mainCRTStartup(void)
//gcc+crinkler subsystem:windows
void WinMainCRTStartup(void)
{
	MSG msg;
	HANDLE hWnd;
	WNDCLASSEX wc = {0};
	NONCLIENTMETRICS nonClientMetrics;

	nonClientMetrics.cbSize = sizeof(nonClientMetrics);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(nonClientMetrics), &nonClientMetrics, 0);
	hfDefault = CreateFontIndirect(&(nonClientMetrics.lfMessageFont));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = StartupWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = (HINSTANCE)0x400000;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION); /*large icon (alt tab)*/
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH) COLOR_WINDOW;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "startup";
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION); /*small icon (taskbar)*/

	if (!RegisterClassEx(&wc)) {
		MessageBoxA(NULL, "failed to register startup class", DEMONAME, MB_OK);
		ExitProcess(1);
	}

	hWnd = CreateWindowEx(
		WS_EX_APPWINDOW, wc.lpszClassName, DEMONAME,
		(WS_OVERLAPPEDWINDOW | WS_VISIBLE) & ~(WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME),
		REQUESTED_POSITION, REQUESTED_POSITION, REQUESTED_SIZE, REQUESTED_SIZE, 0, 0, wc.hInstance, 0
	);

	while (GetMessage(&msg, 0, 0, 0)) {
		if (msg.message == WM_QUIT) {
			ExitProcess(0);
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}
