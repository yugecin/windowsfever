int seekValue; /*to modify current time by pressing left/right arrow keys to seek through the demo*/
int forceRender; /*to force a gl render instead of waiting for fps delay*/

#pragma pack(push,1)
struct {
	struct win broken[GRID_CELLS_HORZ * GRID_CELLS_VERT];
	struct win main;
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

void demo(GLuint fShader)
{
	int startTickCount, lastTickCount, newTickCount, renderTickCount, ms;
	MSG msg;

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

		if (newTickCount - renderTickCount > 10 || forceRender) {
			forceRender = 0;
			//SetWindowPos(hWnd, 0, 10 + (t / 10) % 100, 10, 0, 0, SWP_NOOWNERZORDER | SWP_NOSIZE | SWP_NOZORDER);

			for (i = 0; i < sizeof(wins)/sizeof(struct win); i++) {
				wglMakeCurrent(wins.broken[i].hDC, hGLRC);
				glProgramUniform1iv(fShader, 0, 1, &ms);
				glRecti(1,1,-1,-1);
				SwapBuffers(wins.broken[i].hDC);
			}
			renderTickCount = newTickCount;
		}
	}
done:
	ExitProcess(0);
}

void startdemo()
{
	GLuint vShader, fShader, pipeline;
	char glInfoLogBuf[2000];
	int glInfoLogBufSize;
	POINT pos, size;

	grid_init();

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);

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
	win_make(&wins.main, pos, size, "my-first-shader.glsl");

	for (i = 0; i < sizeof(wins.broken)/sizeof(struct win); i++) {
		pos.x = grid.pos.x + grid.size.x * (i % GRID_CELLS_HORZ);
		pos.y = grid.pos.y + grid.size.y * (i / GRID_CELLS_HORZ);
		size = grid.size;
		DemoWindowSizeDesiredToReal(&pos, &size);
		win_make(wins.broken + i, pos, size, "m");
	}

	for (i = 0; i < sizeof(wins)/sizeof(struct win); i++) {
		wglMakeCurrent(wins.broken[i].hDC, hGLRC);
		vShader = glCreateShaderProgramv(GL_VERTEX_SHADER, 1, &vsh);
		fShader = glCreateShaderProgramv(GL_FRAGMENT_SHADER, 1, &fragSource);
		glGenProgramPipelines(1, &pipeline);
		glBindProgramPipeline(pipeline);
		glUseProgramStages(pipeline, GL_VERTEX_SHADER_BIT, vShader);
		glUseProgramStages(pipeline, GL_FRAGMENT_SHADER_BIT, fShader);

		glInfoLogBufSize = 0;
		glGetProgramInfoLog(fShader, sizeof(glInfoLogBuf), &glInfoLogBufSize, glInfoLogBuf);
		if (glInfoLogBuf[0] && glInfoLogBufSize) {
			showconsole();
			printf("gl info log is not emtpy, assuming error:\n%s", glInfoLogBuf);
			error_exit_loop();
		}
	}

	demo(fShader);
}
