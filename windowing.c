struct {
	RECT rcFull, rcWork, rcBorders;
	int workingAreaHeight, workingAreaWidth;
	POINT reqToRealFramePos, reqToRealFrameSize;
} metrics;
POINT nullpt;

#define GRID_CELLS_HORZ 8
#define GRID_CELLS_VERT 4
#define GRID_CELLS (GRID_CELLS_HORZ * GRID_CELLS_VERT)
#define GRID_BORDERCELLS (GRID_CELLS_HORZ * 2 + GRID_CELLS_VERT * 2 + 4)
struct {
	/*size defines the size of a cell*/
	/*pos is the top-left position where the grid starts, it gives space for GRID_CELLS_VERTxGRID_CELLS_HORZ cells,
	  but there is space for another cell on each side of the grid.*/
	POINT pos, size;
	POINT cellLoadingPos, cellpos[GRID_CELLS], borderpos[GRID_BORDERCELLS];
	POINT loaderPos, loaderSize, mainSize;
} grid;

void grid_init()
{
	grid.size.x = metrics.workingAreaWidth / (GRID_CELLS_HORZ + 3);
	grid.size.y = metrics.workingAreaHeight / (GRID_CELLS_VERT + 3);
	grid.pos.x = metrics.rcWork.left + (metrics.workingAreaWidth - grid.size.x * GRID_CELLS_HORZ) / 2;
	grid.pos.y = metrics.rcWork.top + (metrics.workingAreaHeight - grid.size.y * GRID_CELLS_VERT) / 2;
	grid.mainSize.x = grid.size.x * GRID_CELLS_HORZ;
	grid.mainSize.y = grid.size.y * GRID_CELLS_VERT;

	for (i = 0; i < GRID_CELLS_HORZ * GRID_CELLS_VERT; i++) {
		grid.cellpos[i].x = grid.pos.x + grid.size.x * (i % GRID_CELLS_HORZ);
		grid.cellpos[i].y = grid.pos.y + grid.size.y * (i / GRID_CELLS_HORZ);
	}

	for (i = 0; i < GRID_BORDERCELLS; i++) {
		if (i < GRID_CELLS_HORZ + 2) {
			grid.borderpos[i].x = grid.pos.x + grid.size.x * (i - 1);
			grid.borderpos[i].y = grid.pos.y - grid.size.y;
		} else if (i >= GRID_CELLS_HORZ + 2 + GRID_CELLS_VERT * 2) {
			grid.borderpos[i].x = grid.pos.x + grid.size.x * (i - (GRID_CELLS_HORZ + 2 + GRID_CELLS_VERT * 2) - 1);
			grid.borderpos[i].y = grid.pos.y + grid.size.y * GRID_CELLS_VERT;
		} else {
			grid.borderpos[i].x = grid.pos.x - grid.size.x + grid.size.x * ((i - GRID_CELLS_HORZ - 2) % 2) * (GRID_CELLS_HORZ + 1);
			grid.borderpos[i].y = grid.pos.y + grid.size.y * ((i - GRID_CELLS_HORZ - 2) / 2);
		}
	}

	grid.cellLoadingPos.x = metrics.rcWork.left + (metrics.workingAreaWidth - grid.size.x) / 2;
	grid.cellLoadingPos.y = metrics.rcWork.top + (metrics.workingAreaHeight - grid.size.y) / 2;

	grid.loaderSize.y = grid.size.y * 2;
	grid.loaderSize.x = grid.loaderSize.y * 3;
	grid.loaderPos.x = metrics.rcWork.left + (metrics.workingAreaWidth - grid.loaderSize.x) / 2;
	grid.loaderPos.y = metrics.rcWork.top + (metrics.workingAreaHeight - grid.loaderSize.y) / 2;
}

/*since a hglrc can be used for any hdc that was made on the same device with the same
  pixel format, we only need one hglrc, even if multiple windows want to use opengl*/
HGLRC hGLRC;
/*previous comment also means the shader is compiled once and can be reused*/
GLuint frag;

struct win {
	POINT framePos, clientPos, frameSize, clientSize;
	HDC hDC, hBackDC;
	HWND hWnd;
	int shown;
};

struct {
	struct win loader;
	struct win main;
	struct win cells[GRID_CELLS_HORZ * GRID_CELLS_VERT];
	struct win border[GRID_CELLS_HORZ * 2 + GRID_CELLS_VERT * 2 + 4];
	struct win altMain;
} wins;

void DemoBitBltClientArea(struct win *dst, struct win *src, int x, int y)
{
	BitBlt(
		dst->hDC,
		metrics.rcBorders.left, metrics.rcBorders.top,
		dst->clientSize.x + metrics.rcBorders.left + 1
			/*idk why +1 is necessary, idk why left border is even necessary because
			  cx is documented as "width" and not "end x coord". the behavior seems
			  different from docs and also different from the cy param below, which doesn't
			  need a similar adjustment for top border?!*/,
		dst->clientSize.y,
		src->hDC,
		metrics.rcBorders.left, metrics.rcBorders.top,
		SRCCOPY
	);
}

void DemoWindowSizeDesiredToReal(POINT *size)
{
	size->x -= metrics.reqToRealFrameSize.x;
	size->y -= metrics.reqToRealFrameSize.y;
}

#define SWS_ZORDER SWP_NOZORDER // so the default flag is /not/ changing z-order instead of the opposite
void DemoSetWindowState(struct win *this, HWND hWndInsertAfter, POINT pos, POINT size, int swpFlags)
{
	if (swpFlags & SWS_ZORDER) {
		swpFlags &= ~SWP_NOZORDER;
	} else {
		swpFlags |= SWP_NOZORDER;
	}
	if (swpFlags & SWP_SHOWWINDOW) {
		if (this->shown) {
			swpFlags &= ~SWP_SHOWWINDOW;
		}
		this->shown = 1;
	}
	if (swpFlags & SWP_HIDEWINDOW) {
		if (!this->shown) {
			swpFlags &= ~SWP_HIDEWINDOW;
		}
		this->shown = 0;
	}
	if (!(swpFlags & SWP_NOSIZE)) {
		if (this->frameSize.x == size.x && this->frameSize.y == size.y) {
			swpFlags |= SWP_NOSIZE;
		} else {
			this->frameSize.x = size.x - metrics.reqToRealFrameSize.x;
			this->frameSize.y = size.y - metrics.reqToRealFrameSize.y;
			this->clientSize.x = this->frameSize.x - metrics.rcBorders.left - metrics.rcBorders.right;
			this->clientSize.y = this->frameSize.y - metrics.rcBorders.bottom - metrics.rcBorders.top;
		}
	}
	if (!(swpFlags & SWP_NOMOVE)) {
		if (this->framePos.x == pos.x && this->framePos.y == pos.y) {
			swpFlags |= SWP_NOMOVE;
		} else {
			this->framePos.x = pos.x - metrics.reqToRealFramePos.x;
			this->framePos.y = pos.y - metrics.reqToRealFramePos.y;
			this->clientPos.x = this->framePos.x + metrics.rcBorders.left;
			this->clientPos.y = this->framePos.y + metrics.rcBorders.top;
		}
	}
	SetWindowPos(this->hWnd, hWndInsertAfter, pos.x, pos.y, size.x, size.y, swpFlags);
}

void DemoRenderGl(struct win *this)
{
	if (this->shown) {
		wglMakeCurrent(this->hDC, hGLRC);
		glProgramUniform1fv(frag, 0, 6, (float*) &uniformPar);
		glViewport(0, 0, this->clientSize.x, this->clientSize.y);
		glRecti(1, 1, -1, -1);
		SwapBuffers(this->hDC);
	}
}

#define MW_VISIBLE 1
#define MW_GL 2
#define MW_BACKDC 4
void DemoMakeWin(struct win *this, POINT pos, POINT size, char *title, int flags)
{
	static PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		32,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		32,
		0, 0, 0, 0, 0, 0, 0
	};
	static int pixelFormat = 0;

	char glInfoLogBuf[2000];
	int glInfoLogBufSize;
	GLuint vert, pipeline;

	DemoWindowSizeDesiredToReal(&size);
	this->shown = flags & MW_VISIBLE;
	this->hWnd = CreateWindowEx(
		WS_EX_APPWINDOW, wcDemo.lpszClassName, title,
		(WS_OVERLAPPEDWINDOW | (this->shown ? WS_VISIBLE : 0)) & ~(WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME),
		pos.x, pos.y, size.x, size.y, 0, 0, wcDemo.hInstance, 0
	);
	this->framePos.x = pos.x + metrics.reqToRealFramePos.x;
	this->framePos.y = pos.y + metrics.reqToRealFramePos.y;
	this->clientPos.x = this->framePos.x + metrics.rcBorders.left;
	this->clientPos.y = this->framePos.y + metrics.rcBorders.top;
	this->frameSize.x = size.x + metrics.reqToRealFrameSize.x;
	this->frameSize.y = size.y + metrics.reqToRealFrameSize.y;
	this->clientSize.x = this->frameSize.x - metrics.rcBorders.left - metrics.rcBorders.right;
	this->clientSize.y = this->frameSize.y - metrics.rcBorders.bottom - metrics.rcBorders.top;
	this->hDC = GetWindowDC(this->hWnd);
	if (!pixelFormat) {
		pixelFormat = ChoosePixelFormat(this->hDC, &pfd);
	}
	SetPixelFormat(this->hDC, pixelFormat, &pfd);
	if (flags & MW_BACKDC) {
		this->hBackDC = CreateCompatibleDC(this->hDC);
		SelectObject(this->hBackDC, CreateCompatibleBitmap(this->hDC, this->clientSize.x, this->clientSize.y));
	}
	if ((flags & MW_GL) && !hGLRC) {
		hGLRC = wglCreateContext(this->hDC);
		wglMakeCurrent(this->hDC, hGLRC);
		glstuff_get_procs();
		vert = glCreateShaderProgramv(GL_VERTEX_SHADER, 1, &vertSource);
		frag = glCreateShaderProgramv(GL_FRAGMENT_SHADER, 1, &fragSource);
		glGenProgramPipelines(1, &pipeline);
		glBindProgramPipeline(pipeline);
		glUseProgramStages(pipeline, GL_VERTEX_SHADER_BIT, vert);
		glUseProgramStages(pipeline, GL_FRAGMENT_SHADER_BIT, frag);

		glInfoLogBufSize = 0;
		glGetProgramInfoLog(frag, sizeof(glInfoLogBuf), &glInfoLogBufSize, glInfoLogBuf);
		if (glInfoLogBuf[0] && glInfoLogBufSize) {
			MessageBoxA(NULL, "gl info log is not emtpy, assuming error:", DEMONAME, MB_OK);
			MessageBoxA(NULL, glInfoLogBuf, DEMONAME, MB_OK);
			ExitProcess(1);
		}
	}
}
