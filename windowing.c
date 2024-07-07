struct {
	RECT rcFull, rcWork, rcBorders;
	int workingAreaHeight, workingAreaWidth;
	POINT reqToRealFramePos, reqToRealFrameSize;
} metrics;

#define GRID_CELLS_HORZ 8
#define GRID_CELLS_VERT 4
struct {
	/*size defines the size of a cell*/
	/*pos is the top-left position where the grid starts, it gives space for GRID_CELLS_VERTxGRID_CELLS_HORZ cells,
	  but there is space for another cell on each side of the grid.*/
	POINT pos, size;
} grid;

void grid_init()
{
	int w, h;

	w = metrics.workingAreaWidth / (GRID_CELLS_HORZ + 3);
	h = metrics.workingAreaHeight / (GRID_CELLS_VERT + 3);

	if (w * (GRID_CELLS_HORZ + 3) * 9.0f / 16.0f > h * (GRID_CELLS_VERT + 3)) {
		grid.size.x = h * 16 / 9;
		grid.size.y = h;
	} else {
		grid.size.x = w;
		grid.size.y = w * 9 / 16;
	}
	grid.size.x = w;
	grid.size.y = h;
	grid.pos.x = metrics.rcWork.left + (metrics.workingAreaWidth - grid.size.x * GRID_CELLS_HORZ) / 2;
	grid.pos.y = metrics.rcWork.top + (metrics.workingAreaHeight - grid.size.y * GRID_CELLS_VERT) / 2;
}

/*since a hglrc can be used for any hdc that was made on the same device with the same pixel format, we only need one hglrc*/
HGLRC hGLRC;

struct win {
	POINT framePos, clientPos, frameSize, clientSize;
	HWND hWnd;
	HDC hDC;
};

void DemoWindowSizeDesiredToReal(POINT *pos, POINT *size)
{
	size->x -= metrics.reqToRealFrameSize.x;
	size->y -= metrics.reqToRealFrameSize.y;
}

int pixelFormat;

void win_make(struct win *this, POINT pos, POINT size, char *title)
{
	this->hWnd = CreateWindowEx(
		WS_EX_APPWINDOW, wcDemo.lpszClassName, title,
		(WS_OVERLAPPEDWINDOW | WS_VISIBLE) & ~(WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME),
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
	if (!hGLRC) {
		hGLRC = wglCreateContext(this->hDC);
		wglMakeCurrent(this->hDC, hGLRC);
		glstuff_get_procs();
	}
}
