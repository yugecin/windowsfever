int seekValue; /*to modify current time by pressing left/right arrow keys to seek through the demo*/
int forceRender; /*to force a gl render instead of waiting for fps delay*/
#define LOADERMAX (sizeof(wins)*2/sizeof(struct win))
int loaderCurrent; /*loader window progress bar*/

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
	Rectangle(hDC, 15, 15, (int) ((w - 15) * ((float) loaderCurrent / LOADERMAX)), h - 15);
	RestoreDC(hDC, dccookie);

	BeginPaint(wins.loader.hWnd, &ps);
	BitBlt(ps.hdc, 0, 0, w, h, wins.loader.hBackDC, 0, 0, SRCCOPY);
	EndPaint(wins.loader.hWnd, &ps);
}

LRESULT CALLBACK DemoWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_DESTROY:
		if (hWnd != wins.loader.hWnd) {
			PostQuitMessage(0);
		}
		return 0;
	case WM_KEYDOWN:
		if (wParam == VK_RIGHT) {
			seekValue += 1000;
			forceRender = 1;
			return 0;
		}
		if (wParam == VK_LEFT) {
			seekValue -= 1000;
			forceRender = 1;
			return 0;
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
			DemoRenderGl(wins.cells + j * GRID_CELLS_HORZ + i);
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

void demo()
{
	static int cellsShown = 0;

	int startTickCount, lastTickCount, newTickCount, renderTickCount, ms, dorender;
	POINT pos;
	float t;

	renderTickCount = -1000;
	startTickCount = lastTickCount = GetTickCount();
	for (;;) {
		pumpmessages();

		newTickCount = GetTickCount() + seekValue;
		ms = newTickCount - startTickCount;
		lastTickCount = newTickCount;

		dorender = newTickCount - renderTickCount > 10 || forceRender;
		forceRender = 0;

		if (ms > 10000) {
			if (!IsWindowVisible(wins.main.hWnd)) {
				ShowWindow(wins.main.hWnd, SW_SHOW);
				dorender = 1;
				for (i = 0; i < GRID_CELLS_HORZ * GRID_CELLS_VERT; i++) {
					DemoCalcCellLoadingPos(&pos);
					SetWindowPos(wins.cells[i].hWnd, NULL, pos.x, pos.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
				}
				cellsShown = 0;
			}
		} else if (ms > 4000) {
			dorender = 1;
			if (IsWindowVisible(wins.main.hWnd)) {
				cellsShown = 1;
				for (i = 0; i < GRID_CELLS_HORZ * GRID_CELLS_VERT; i++) {
					DemoRestoreWindow(wins.cells + i, SWP_NOSIZE | SWP_NOACTIVATE);
				}
				dorender = 1;
				ShowWindow(wins.main.hWnd, SW_HIDE);
			}
			t = (ms - 4000) / 6000.0f * 1.15f;
			if (t > 1.0f) {
				t = 1.0f;
			}
			t = -1.0f * t * (t - 2.0f);
			for (i = 0; i < GRID_CELLS_HORZ * GRID_CELLS_VERT; i++) {
				explosion_do(i, t);
			}
		}

		if (dorender) {
			renderTickCount = newTickCount;
			//SetWindowPos(hWnd, 0, 10 + (t / 10) % 100, 10, 0, 0, SWP_NOOWNERZORDER | SWP_NOSIZE | SWP_NOZORDER);

			uniformPar.fTime = ms / 1000.0f;
			uniformPar.umin = uniformPar.vmin = 0.0f;
			uniformPar.umax = uniformPar.vmax = 1.0f;
			DemoRenderGl(&wins.main);

			//DemoBitBltClientArea(&wins.cells[0], &wins.main, 0, 0);

			if (cellsShown) {
				render_shader_in_cells();
			}
		}
	}
}

void startdemo()
{
	POINT pos, size;

	grid_init();

	uniformPar.fTime = 0.0f;
	uniformPar.umin = uniformPar.vmin = 0.0f;
	uniformPar.umax = uniformPar.vmax = 1.0f;

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
		err = GetLastError();
		showconsole();
		printf("failed to register demo class: %d\n", err);
		error_exit_loop();
	}

	size.y = grid.size.y * 2;
	size.x = size.y * 3;
	pos.x = metrics.rcWork.left + (metrics.workingAreaWidth - size.x) / 2;
	pos.y = metrics.rcWork.top + (metrics.workingAreaHeight - size.y) / 2;
	DemoWindowSizeDesiredToReal(&pos, &size);
	DemoMakeWin(&wins.loader, pos, size, DEMONAME, MW_VISIBLE | MW_BACKDC);
	loaderCurrent = 2;
	render_loader();

	pos = grid.pos;
	size.x = grid.size.x * GRID_CELLS_HORZ;
	size.y = grid.size.y * GRID_CELLS_VERT;
	DemoWindowSizeDesiredToReal(&pos, &size);
	DemoMakeWin(&wins.main, pos, size, "my-first-shader.glsl", MW_GL);
	loaderCurrent++;

	for (i = 0; i < GRID_CELLS_HORZ * GRID_CELLS_VERT; i++) {
		pos.x = grid.pos.x + grid.size.x * (i % GRID_CELLS_HORZ);
		pos.y = grid.pos.y + grid.size.y * (i / GRID_CELLS_HORZ);
		size = grid.size;
		DemoWindowSizeDesiredToReal(&pos, &size);
		DemoMakeWin(wins.cells + i, pos, size, "m", MW_GL);
		DemoCalcCellLoadingPos(&pos);
		SetWindowPos(wins.cells[i].hWnd, wins.loader.hWnd, pos.x, pos.y, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOACTIVATE);
		loaderCurrent++;
		RedrawWindow(wins.loader.hWnd, NULL, NULL, RDW_INTERNALPAINT | RDW_INVALIDATE);
		pumpmessages();
	}

	for (i = 0; i < GRID_CELLS_HORZ * 2 + GRID_CELLS_VERT * 2 + 4; i++) {
		size = grid.size;
		if (i < GRID_CELLS_HORZ + 2) {
			pos.x = grid.pos.x + grid.size.x * (i - 1);
			pos.y = grid.pos.y - grid.size.y;
		} else if (i >= GRID_CELLS_HORZ + 2 + GRID_CELLS_VERT * 2) {
			pos.x = grid.pos.x + grid.size.x * (i - (GRID_CELLS_HORZ + 2 + GRID_CELLS_VERT * 2) - 1);
			pos.y = grid.pos.y + grid.size.y * GRID_CELLS_VERT;
		} else {
			pos.x = grid.pos.x - grid.size.x + grid.size.x * ((i - GRID_CELLS_HORZ - 2) % 2) * (GRID_CELLS_HORZ + 1);
			pos.y = grid.pos.y + grid.size.y * ((i - GRID_CELLS_HORZ - 2) / 2);
		}
		DemoWindowSizeDesiredToReal(&pos, &size);
		DemoMakeWin(wins.border + i, pos, size, "m", 0);
		DemoCalcCellLoadingPos(&pos);
		SetWindowPos(wins.border[i].hWnd, wins.loader.hWnd, pos.x, pos.y, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOACTIVATE);
		loaderCurrent++;
		RedrawWindow(wins.loader.hWnd, NULL, NULL, RDW_INTERNALPAINT | RDW_INVALIDATE);
		pumpmessages();
	}

	{
		DemoRenderGl(&wins.main);
		loaderCurrent++;
		pumpmessages();
		for (i = 0; i < GRID_CELLS_HORZ * GRID_CELLS_VERT; i++) {
			wglMakeCurrent(wins.cells[i].hDC, hGLRC);
			glProgramUniform1fv(frag, 0, 5, (float*) &uniformPar);
			glRecti(1, 1, -1, -1);
			SwapBuffers(wins.cells[i].hDC);
			loaderCurrent++;
			RedrawWindow(wins.loader.hWnd, NULL, NULL, RDW_INTERNALPAINT | RDW_INVALIDATE);
			pumpmessages();
			Sleep(3);
		}
		for (i = 0; i < GRID_CELLS_HORZ * 2 + GRID_CELLS_VERT * 2 + 4; i++) {
			loaderCurrent++;
			RedrawWindow(wins.loader.hWnd, NULL, NULL, RDW_INTERNALPAINT | RDW_INVALIDATE);
			pumpmessages();
			Sleep(3);
		}
	}

	explosion_init();

	for (i = 0; i < GRID_CELLS_HORZ * 2 + GRID_CELLS_VERT * 2 + 4; i++) {
		DemoRestoreWindow(wins.border + i, SWP_HIDEWINDOW);
	}

	// render main window once so initial render lag doesn't affect demo timing
	ShowWindow(wins.main.hWnd, SW_SHOW);
	DemoRenderGl(&wins.main);

	// and only destroy loader once main window is actually on screen (otherwise we might reveal the cell windows)
	DestroyWindow(wins.loader.hWnd);

	demo();
}
