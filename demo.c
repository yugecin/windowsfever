PIXELFORMATDESCRIPTOR pfd = {
	sizeof(PIXELFORMATDESCRIPTOR),
	1,
	PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
	32,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	32,
	0, 0, 0, 0, 0, 0, 0
};

WNDCLASSEX wcDemo = {0};
int seekValue; /*to modify current time by pressing left/right arrow keys to seek through the demo*/
int forceRender; /*to force a gl render instead of waiting for fps delay*/

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

void demo(HDC hDC, GLuint fShader)
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
			glProgramUniform1iv(fShader, 0, 1, &ms);
			glRecti(1,1,-1,-1);
			SwapBuffers(hDC);
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
	HWND hWnd;
	HDC hDC;

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

	hWnd = CreateWindowEx(
		WS_EX_APPWINDOW, wcDemo.lpszClassName, "my-first-shader.glsl",
		(WS_OVERLAPPEDWINDOW | WS_VISIBLE) & ~(WS_MINIMIZEBOX | WS_MAXIMIZEBOX),
		CW_USEDEFAULT, CW_USEDEFAULT, 700, 500, 0, 0, wcDemo.hInstance, 0
	);

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	hDC = GetDC(hWnd);
	SetPixelFormat(hDC, ChoosePixelFormat(hDC, &pfd) , &pfd);
	wglMakeCurrent(hDC, wglCreateContext(hDC));

	glstuff_get_procs();

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

	demo(hDC, fShader);
}
