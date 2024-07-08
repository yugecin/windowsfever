int seekValue; /*to modify current time by pressing left/right arrow keys to seek through the demo*/
int forceRender; /*to force a gl render instead of waiting for fps delay*/

#pragma pack(push,1)
struct {
	struct win main;
	struct win cells[GRID_CELLS_HORZ * GRID_CELLS_VERT];
	struct win border[GRID_CELLS_HORZ * 2 + GRID_CELLS_VERT * 2 + 4];
} wins;
#pragma pack(pop)

LRESULT CALLBACK DemoWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
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
	default: return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}

void demo()
{
	int startTickCount, lastTickCount, newTickCount, renderTickCount, ms;
	MSG msg;
	static int b = 0;

	renderTickCount = -1000;
	startTickCount = lastTickCount = GetTickCount();
	while (!GetAsyncKeyState(VK_ESCAPE)) {
		while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				goto done;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		newTickCount = GetTickCount() + seekValue;
		ms = newTickCount - startTickCount;
		lastTickCount = newTickCount;

		if (ms > 10000) {
			//if (!IsWindowVisible(wins.main.hWnd)) {
			//	ShowWindow(wins.main.hWnd, SW_SHOWNA);
			//}
			if (b) {
				b = 0;
				SetWindowPos(wins.main.hWnd, wins.cells[0].hWnd, wins.main.framePos.x - metrics.reqToRealFramePos.x, wins.main.framePos.y - metrics.reqToRealFramePos.y, 0, 0, SWP_NOSIZE);
			}
		} else if (ms > 4000) {
			//if (IsWindowVisible(wins.main.hWnd)) {
			//	ShowWindow(wins.main.hWnd, SW_HIDE);
			//}
			if (!b) {
				SetWindowPos(wins.main.hWnd, NULL, 10000, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
				b = 1;
			}
		}

		if (newTickCount - renderTickCount > 10 || forceRender) {
			forceRender = 0;
			//SetWindowPos(hWnd, 0, 10 + (t / 10) % 100, 10, 0, 0, SWP_NOOWNERZORDER | SWP_NOSIZE | SWP_NOZORDER);

			wglMakeCurrent(wins.main.hDC, hGLRC);
			glProgramUniform1iv(frag, 0, 1, &ms);
			glProgramUniform4f(frag, 1, 0.0f, 1.0f);
			glRecti(1,1,-1,-1);
			SwapBuffers(wins.main.hDC);
			DemoBitBltClientArea(&wins.cells[0], &wins.main, 0, 0);
			wglMakeCurrent(wins.cells[2].hDC, hGLRC);
			glRecti(1,1,-1,-1);
			SwapBuffers(wins.cells[2].hDC);
			renderTickCount = newTickCount;
		}
	}
done:
	ExitProcess(0);
}

void startdemo()
{
	POINT pos, size;

	grid_init();


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

	pos = grid.pos;
	size.x = grid.size.x * GRID_CELLS_HORZ;
	size.y = grid.size.y * GRID_CELLS_VERT;
	DemoWindowSizeDesiredToReal(&pos, &size);
	win_make(&wins.main, pos, size, "my-first-shader.glsl", 1, 1);

	for (i = 0; i < GRID_CELLS_HORZ * GRID_CELLS_VERT; i++) {
		pos.x = grid.pos.x + grid.size.x * (i % GRID_CELLS_HORZ);
		pos.y = grid.pos.y + grid.size.y * (i / GRID_CELLS_HORZ);
		size = grid.size;
		DemoWindowSizeDesiredToReal(&pos, &size);
		win_make(wins.cells + i, pos, size, "m", 1, 1);
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
		win_make(wins.border + i, pos, size, "m", 0, 0);
	}

	demo();
}
