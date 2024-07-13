int seekValue; /*to modify current time by pressing left/right arrow keys to seek through the demo*/
int forceRender; /*to force a gl render instead of waiting for fps delay*/
#define LOADERMAX (sizeof(wins)*2/sizeof(struct win))
int loaderCurrent; /*loader window progress bar*/
int loaderExtra;
int expectLoaderClose; /*so we only quit if loader is closed without us asking it*/
HANDLE hFont, hSmallFont; /*yeah*/
HDC extraDC; /*yeahh*/
HANDLE textsBitmapHandle; /*fuckyeahhh*/
void *textsBitMapBitsPtr;

struct {
	int start, last, now, render;
} tickCount;

void render_loader()
{
	int w, h, dccookie;
	PAINTSTRUCT ps;
	HDC hDC;

	w = wins.loader.clientSize.x;
	h = wins.loader.clientSize.y;
	hDC = wins.loader.hBackDC;
	dccookie = SaveDC(hDC);
	SelectObject(hDC, GetStockObject(BLACK_BRUSH));
	Rectangle(hDC, 0, 0, w, h);
	SelectObject(hDC, GetStockObject(WHITE_BRUSH));
	Rectangle(hDC, 5, 5, w - 5, h - 5);
	SelectObject(hDC, GetStockObject(BLACK_BRUSH));
	Rectangle(hDC, 10, 10, w - 10, h - 10);
	SelectObject(hDC, GetStockObject(WHITE_BRUSH));
	Rectangle(hDC, 15, 15, loaderExtra + (int) ((grid.loaderSize.x - 15) * ((float) loaderCurrent / LOADERMAX)), h - 15);
	RestoreDC(hDC, dccookie);

	BeginPaint(wins.loader.hWnd, &ps);
	BitBlt(ps.hdc, 0, 0, w, h, wins.loader.hBackDC, 0, 0, SRCCOPY);
	EndPaint(wins.loader.hWnd, &ps);
}

LRESULT CALLBACK DemoWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_DESTROY:
		if (hWnd != wins.loader.hWnd || !expectLoaderClose) {
			PostQuitMessage(0);
		}
		return 0;
	case WM_KEYDOWN:
		if (hWnd != wins.loader.hWnd) {
			if (wParam == VK_RIGHT) {
				tickCount.start -= 1000;
				sound_seek_relative_seconds(1);
				forceRender = 1;
				return 0;
			}
			if (wParam == VK_LEFT) {
				tickCount.start += 1000;
				tickCount.now = GetTickCount();
				if (tickCount.start > tickCount.now) tickCount.start = tickCount.now;
				sound_seek_relative_seconds(-1);
				forceRender = 1;
				return 0;
			}
		}
		break;
	case WM_PAINT:
		if (hWnd == wins.loader.hWnd) {
			render_loader();
			return 0;
		}
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK BorderCellWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;

	if (msg == WM_PAINT) {
		for (i = 0; i < GRID_BORDERCELLS; i++) {
			if (hWnd == wins.border[i].hWnd) {
				BeginPaint(hWnd, &ps);
				BitBlt(ps.hdc, 0, 0, wins.border[i].clientSize.x, wins.border[i].clientSize.y, wins.border[i].hBackDC, 0, 0, SRCCOPY);
				EndPaint(hWnd, &ps);
				return 0;
			}
		}
	}
	return CallWindowProc(DemoWndProc, hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK AltMainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;

	if (msg == WM_PAINT) {
		BeginPaint(hWnd, &ps);
		BitBlt(ps.hdc, 0, 0, wins.altMain.clientSize.x, wins.altMain.clientSize.y, wins.altMain.hBackDC, 0, 0, SRCCOPY);
		EndPaint(hWnd, &ps);
		return 0;
	}
	return CallWindowProc(DemoWndProc, hWnd, msg, wParam, lParam);
}

void render_shader_in_cells()
{
	POINT cellClientSize;
	int j, x, y;
	float w, h;

	y = wins.main.clientSize.y;
	h = (float) wins.main.clientSize.y;
	w = (float) wins.main.clientSize.x;
	cellClientSize = wins.cells[0].clientSize;
	for (j = 0; j < GRID_CELLS_VERT; j++) {
		x = 0;
		uniformPar.vmax = y / h;
		y -= cellClientSize.y;
		uniformPar.vmin = y / h;
		y -= metrics.rcBorders.top + metrics.rcBorders.bottom;
		for (i = 0; i < GRID_CELLS_HORZ; i++) {
			uniformPar.umin = x / w;
			x += cellClientSize.x;
			uniformPar.umax = x / w;
			x += metrics.rcBorders.right + metrics.rcBorders.left;
			if (!wins.cells[j * GRID_CELLS_HORZ + i].behind) {
				DemoRenderGl(wins.cells + j * GRID_CELLS_HORZ + i);
			}
		}
	}
}

void pumpmessages()
{
	MSG msg;

	if (GetAsyncKeyState(VK_ESCAPE)) {
		PostQuitMessage(0);
	}
	while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
		if (msg.message == WM_QUIT) {
			ExitProcess(0);
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

struct {
	int ms, doRenderGL, doRenderCellsGL;
} demostate;

#include "demotick.c"
void demo()
{

	sound_play(0);
	tickCount.render = -1000;
	tickCount.start = tickCount.last = GetTickCount();
	demostate.doRenderCellsGL = 0;
	for (;;) {
		pumpmessages();

		tickCount.now = GetTickCount();
		demostate.ms = tickCount.now - tickCount.start;
		tickCount.last = tickCount.now;

#ifdef TICKCOUNT_BY_AUDIOPOSITION
		demostate.ms = sound_get_pos_ms();
		if (demostate.ms == SOUND_END) {
			ExitProcess(0);
		}
#endif

		demostate.doRenderGL = demostate.doRenderCellsGL = tickCount.now - tickCount.render > 10;
		demotick();
		if (demostate.doRenderGL || demostate.doRenderCellsGL) {
			tickCount.render = tickCount.now;

			uniformPar.fTime = demostate.ms / 1000.0f;
			uniformPar.umin = uniformPar.vmin = 0.0f;
			uniformPar.umax = uniformPar.vmax = 1.0f;
			DemoRenderGl(&wins.main);

			if (demostate.doRenderCellsGL) {
				uniformPar.brokenState = 0.0f;
				render_shader_in_cells();
			}
		}
	}
}

void startdemo()
{
	BITMAPINFO bmi;

	grid_init();

	uniformPar.fTime = 0.0f;
	uniformPar.umin = uniformPar.vmin = 0.0f;
	uniformPar.umax = uniformPar.vmax = 1.0f;
	uniformPar.renderText = 0.0f;

	wcDemo.cbSize = sizeof(WNDCLASSEX);
	wcDemo.style = 0;
	wcDemo.lpfnWndProc = DemoWndProc;
	wcDemo.cbClsExtra = 0;
	wcDemo.cbWndExtra = 0;
	wcDemo.hInstance = (HINSTANCE)0x400000;
	wcDemo.hIcon = LoadIcon(NULL, IDI_APPLICATION); /*large icon (alt tab)*/
	wcDemo.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcDemo.hbrBackground = (HBRUSH) COLOR_WINDOW;
	wcDemo.lpszMenuName = NULL;
	wcDemo.lpszClassName = "demo";
	wcDemo.hIconSm = LoadIcon(NULL, IDI_APPLICATION); /*small icon (taskbar)*/
	if (!RegisterClassEx(&wcDemo)) {
		MessageBoxA(NULL, "failed to register demo class", DEMONAME, MB_OK);
		ExitProcess(2);
	}

	DemoMakeWin(&wins.loader, grid.loaderPos, grid.loaderSize, DEMONAME, MW_VISIBLE | MW_BACKDC);
	loaderCurrent = 2;
	RedrawWindow(wins.loader.hWnd, NULL, NULL, RDW_INTERNALPAINT | RDW_INVALIDATE);
	pumpmessages();

	for (i = 0; i < GRID_CELLS_HORZ * GRID_CELLS_VERT; i++) {
		DemoMakeWin(wins.cells + i, grid.cellLoadingPos, grid.size, "m", MW_GL);
		DemoSetWindowState(wins.cells + i, wins.loader.hWnd, nullpt, nullpt, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWS_ZORDER);
		loaderCurrent++;
		RedrawWindow(wins.loader.hWnd, NULL, NULL, RDW_INTERNALPAINT | RDW_INVALIDATE);
		pumpmessages();
	}

	for (i = 0; i < GRID_BORDERCELLS; i++) {
		DemoMakeWin(wins.border + i, grid.cellLoadingPos, grid.size, "m", MW_BACKDC);
		SetWindowLong(wins.border[i].hWnd, GWL_WNDPROC, (LONG) (LONG_PTR) &BorderCellWndProc);
		DemoSetWindowState(wins.border + i, wins.loader.hWnd, nullpt, nullpt, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWS_ZORDER);
		//loaderCurrent++;
		RedrawWindow(wins.loader.hWnd, NULL, NULL, RDW_INTERNALPAINT | RDW_INVALIDATE);
		pumpmessages();
	}

	{
		for (i = 0; i < GRID_CELLS_HORZ * GRID_CELLS_VERT; i++) {
			DemoRenderGl(wins.cells + i);
			loaderCurrent++;
			RedrawWindow(wins.loader.hWnd, NULL, NULL, RDW_INTERNALPAINT | RDW_INVALIDATE);
			pumpmessages();
#ifdef RELEASE
			Sleep(3);
#endif
		}
		for (i = 0; i < GRID_CELLS_HORZ * 2 + GRID_CELLS_VERT * 2 + 4; i++) {
			// TODO: initial render or something? this loop is currently useless
			loaderCurrent++;
			RedrawWindow(wins.loader.hWnd, NULL, NULL, RDW_INTERNALPAINT | RDW_INVALIDATE);
			pumpmessages();
#ifdef RELEASE
			Sleep(3);
#endif
		}
	}

	for (i = 0; i < GRID_CELLS_HORZ * 2 + GRID_CELLS_VERT * 2 + 4; i++) {
		DemoSetWindowState(wins.border + i, NULL, nullpt, nullpt, SWP_HIDEWINDOW | SWP_NOSIZE | SWP_NOMOVE);
	}

	hFont = CreateFontA(-(int) (grid.size.y * .8f), 0, 0, 0, FW_ULTRABOLD, 0, 0, 0, ANSI_CHARSET, 0, 0, ANTIALIASED_QUALITY, 0, "Arial");
	hSmallFont = CreateFontA(-(int) (grid.size.y * .6f), 0, 0, 0, FW_ULTRABOLD, 0, 0, 0, ANSI_CHARSET, 0, 0, ANTIALIASED_QUALITY, 0, "Arial");

	sound_init();
	explosion_init();

	DemoMakeWin(&wins.altMain, grid.pos, grid.mainSize, "my-first-shader.glsl", MW_BACKDC);
	SetWindowLong(wins.altMain.hWnd, GWL_WNDPROC, (LONG) (LONG_PTR) &AltMainWndProc);
	loaderCurrent+=2;
	bmi.bmiHeader.biSize = 40;
	bmi.bmiHeader.biWidth = wins.altMain.clientSize.x;
	bmi.bmiHeader.biHeight = wins.altMain.clientSize.y;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = 0;
	bmi.bmiHeader.biSizeImage = 0;
	bmi.bmiHeader.biXPelsPerMeter = 0;
	bmi.bmiHeader.biYPelsPerMeter = 0;
	bmi.bmiHeader.biClrUsed = 0;
	bmi.bmiHeader.biClrImportant = 0;
	*(int*)&bmi.bmiColors = 0;
	textsBitmapHandle = CreateDIBSection(wins.altMain.hBackDC, &bmi, DIB_RGB_COLORS, &textsBitMapBitsPtr, 0, 0);

	loaderCurrent+= 2; // for the main window, but we don't draw loader after that's loaded so do it now

	for (i = 0; i < 200; i++) {
		Sleep(8);
		tmpPos.x = grid.loaderSize.x + i;
		tmpPos.y = grid.loaderSize.y;
		if (loaderCurrent < LOADERMAX && i % 10) {
			loaderCurrent++;
		}
		DemoSetWindowState(&wins.loader, NULL, nullpt, tmpPos, SWP_NOMOVE | SWP_NOACTIVATE);
		RedrawWindow(wins.loader.hWnd, NULL, NULL, RDW_INTERNALPAINT | RDW_INVALIDATE);
		pumpmessages();
	}
	loaderCurrent = LOADERMAX;
	loaderExtra += 200;
	RedrawWindow(wins.loader.hWnd, NULL, NULL, RDW_INTERNALPAINT | RDW_INVALIDATE);
	for (i = 0; i < 30; i++) {
		Sleep(3);
		pumpmessages();
	}

	DemoMakeWin(&wins.main, grid.pos, grid.mainSize, "my-first-shader.glsl", MW_GL | MW_VISIBLE);
	RedrawWindow(wins.loader.hWnd, NULL, NULL, RDW_INTERNALPAINT | RDW_INVALIDATE);
	pumpmessages();
	DemoRenderGl(&wins.main);

	// and only destroy loader once main window is actually on screen (otherwise we might reveal the cell windows)
	expectLoaderClose = 1;
	DestroyWindow(wins.loader.hWnd);

	demo();
}
