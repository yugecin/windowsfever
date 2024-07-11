char *developers = "DEVELOPERS";
struct {
	float t;
	int relTime;
	int duration;
} period;

int isperiod(int from, int to)
{
	if (from <= demostate.ms && demostate.ms < to) {
		period.duration = to - from;
		period.relTime = demostate.ms - from;
		period.t = period.relTime / (float) period.duration;
		return 1;
	}
	return 0;
}

void ensuremainshown()
{
	if (!wins.main.shown) {
		DemoSetWindowState(&wins.main, NULL, grid.pos, wins.main.frameSize, SWP_NOZORDER | SWP_SHOWWINDOW);
		// pumpmessages so that main window is in fact shown before potentially cells are shown (behind)
		// otherwise you might be able to see the cells all together in the middle before the main window shows
		pumpmessages();
		demostate.doRenderGL = 1;
	}
}

void ensuremainhidden()
{
	if (wins.main.shown) {
		DemoSetWindowState(&wins.main, NULL, nullpt, nullpt, SWP_NOMOVE | SWP_NOSIZE | SWP_HIDEWINDOW);
	}
}

void ensurecellsshown()
{
	for (i = 0; i < GRID_CELLS_HORZ * GRID_CELLS_VERT; i++) {
		if (!wins.cells[i].shown) {
			DemoSetWindowState(wins.cells + i, NULL, grid.cellpos[i], nullpt, SWP_NOSIZE | SWP_NOACTIVATE);
			demostate.doRenderCellsGL = 1;
		}
	}
}

void ensurecellshidden()
{
	for (i = 0; i < GRID_CELLS_HORZ * GRID_CELLS_VERT; i++) {
		DemoSetWindowState(wins.cells + i, NULL, grid.cellLoadingPos, nullpt, SWP_NOSIZE | SWP_NOACTIVATE);
	}
}

void ensurebordershidden()
{
	for (i = 0; i < GRID_BORDERCELLS; i++) {
		if (wins.border[i].shown) {
			DemoSetWindowState(wins.border + i, NULL, nullpt, nullpt, SWP_HIDEWINDOW | SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
		}
	}
}

POINT tmpPos;
RECT tmpRect;
int tmp, dccookie;
float t, t2, t3;

void demotick()
{
	HDC hDC;

	if (isperiod(0, 6350)) {
		// start
		ensuremainshown();
		ensurecellshidden();
		ensurebordershidden();
	} else if (isperiod(6350, 8150)) {
		// shake before explosion
		ensuremainshown();
		ensurecellshidden();
		if (demostate.ms % 10) {
			tmp = (int) (100 * eq_in_quart((demostate.ms - 6350) / 1800.0f));
			srand(demostate.ms);
			tmpPos.x = grid.pos.x + (randn(tmp) - (tmp) / 2);
			tmpPos.y = grid.pos.y + (randn(tmp) - (tmp) / 2);
			DemoSetWindowState(&wins.main, NULL, tmpPos, nullpt, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
		}
		ensurebordershidden();
	} else if (isperiod(8150, 11050)) {
		// explosion
		ensuremainhidden();
		ensurecellsshown();
		t = eq_out_cubic((period.relTime - 100) / (float) (period.duration - 200));
		if (t < 0.0f) t = 0.0f;
		else if (t > 1.0f) t = 1.0f;
		for (i = 0; i < GRID_CELLS_HORZ * GRID_CELLS_VERT; i++) {
			explosion_do(wins.cells + i, i, t);
		}
		ensurebordershidden();
	} else if (isperiod(11050, 11750)) {
		ensuremainshown();
		ensurecellshidden();
		for (i = 0; i < GRID_BORDERCELLS; i++) {
			if (!wins.border[i].shown) {
				DemoSetWindowState(wins.border + i, NULL, grid.borderpos[i], grid.size, SWP_SHOWWINDOW | SWP_NOACTIVATE);
			}
			tmp = 255 - (int) (255 * eq_in_quad(period.t));
			hDC = wins.border[i].hBackDC;
			dccookie = SaveDC(hDC);
			SetDCBrushColor(hDC, RGB(tmp, tmp, tmp));
			SetDCPenColor(hDC, RGB(tmp, tmp, tmp));
			SelectObject(hDC, GetStockObject(DC_BRUSH));
			SelectObject(hDC, GetStockObject(DC_PEN));
			Rectangle(hDC, 0, 0, wins.border[i].clientSize.x - 1, wins.border[i].clientSize.y - 1);
			RestoreDC(hDC, dccookie);
			RedrawWindow(wins.border[i].hWnd, NULL, NULL, RDW_INTERNALPAINT | RDW_INVALIDATE);
		}
	} else if (isperiod(11750, 13750)) {
		ensuremainshown();
		ensurecellshidden();
		ensurebordershidden();
	} else if (isperiod(13750, 14750)) {
		ensuremainshown();
		ensurecellshidden();
		for (i = 0; i < GRID_BORDERCELLS; i++) {
			srand(grid.borderpos[i].x);
			tmp = randn(period.duration - 200);
			t = (tmp - period.relTime) / 200.0f;
			if (t < 0.0 || t > 1.0) {
				if (wins.border[i].shown) {
					DemoSetWindowState(wins.border + i, NULL, nullpt, nullpt, SWP_HIDEWINDOW | SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
				}
			} else {
				t = 1.0f - t;
				tmp = 255 - (int) (255 * t);
				hDC = wins.border[i].hBackDC;
				dccookie = SaveDC(hDC);
				SetDCBrushColor(hDC, RGB(tmp, tmp, tmp));
				SetDCPenColor(hDC, RGB(tmp, tmp, tmp));
				SelectObject(hDC, GetStockObject(DC_BRUSH));
				SelectObject(hDC, GetStockObject(DC_PEN));
				Rectangle(hDC, 0, 0, wins.border[i].clientSize.x - 1, wins.border[i].clientSize.y - 1);
				RestoreDC(hDC, dccookie);
				tmpPos.x = metrics.rcWork.left + randn(metrics.workingAreaWidth - grid.size.x);
				tmpPos.y = metrics.rcWork.top + randn(metrics.workingAreaHeight - grid.size.y);
				DemoSetWindowState(wins.border + i, NULL, tmpPos, nullpt, SWP_SHOWWINDOW | SWP_NOSIZE);
				RedrawWindow(wins.border[i].hWnd, NULL, NULL, RDW_INTERNALPAINT | RDW_INVALIDATE);
			}
		}
		pumpmessages();
	} else if (isperiod(14750, 15000)) {
		ensuremainshown();
		ensurecellshidden();
		ensurebordershidden();
	} else if (isperiod(15000, 19000)) {
		ensuremainshown();
		ensurecellshidden();

		tmpPos.y = grid.pos.y + (grid.size.y * GRID_CELLS_VERT) / 2 - grid.size.y * 2 + grid.size.y / 2;
		t = period.t;
		if (t > .33f) {	t -= .33f; tmpPos.y += grid.size.y; }
		if (t > .33f) {	t -= .33f; tmpPos.y += grid.size.y; }
		t /= .33f;
		for (i = 0; i < GRID_BORDERCELLS; i++) {
			if (i < 10 && t < 1.0f) {
				tmpPos.x = grid.pos.x - grid.size.x + grid.size.x * i;
				t2 = i / 10.0f * .8f;
				t3 = i / 10.0f * .8f + .2f;
				if (t2 <= t && t < t3) {
					hDC = wins.border[i].hBackDC;
					dccookie = SaveDC(hDC);
					SetDCBrushColor(hDC, RGB(0, 0, 0));
					SetDCPenColor(hDC, RGB(0, 0, 0));
					SelectObject(hDC, GetStockObject(DC_BRUSH));
					SelectObject(hDC, GetStockObject(DC_PEN));
					Rectangle(hDC, 0, 0, wins.border[i].clientSize.x - 1, wins.border[i].clientSize.y - 1);
					SelectObject(hDC, hFont);
					SetBkColor(hDC, RGB(0, 0, 0));
					SetBkMode(hDC, TRANSPARENT);
					SetTextColor(hDC, RGB(255, 255, 255));
					tmpRect.top = 0;
					tmpRect.right = wins.border[i].clientSize.x;
					tmpRect.bottom = wins.border[i].clientSize.y;
					tmpRect.left = 0;
					DrawTextA(hDC, developers + i, 1, &tmpRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
					RestoreDC(hDC, dccookie);
					DemoSetWindowState(wins.border + i, NULL, tmpPos, nullpt, SWP_SHOWWINDOW | SWP_NOSIZE | (wins.border[i].shown ? SWP_NOACTIVATE : 0));
					RedrawWindow(wins.border[i].hWnd, NULL, NULL, RDW_INTERNALPAINT | RDW_INVALIDATE);
				} else {
					if (wins.border[i].shown) {
						DemoSetWindowState(wins.border + i, NULL, nullpt, nullpt, SWP_HIDEWINDOW | SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
					}
				}
			} else {
				if (wins.border[i].shown) {
					DemoSetWindowState(wins.border + i, NULL, nullpt, nullpt, SWP_HIDEWINDOW | SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
				}
			}
		}
		pumpmessages();
	} else if (isperiod(19000, 60000)) {
		// end
		ensuremainshown();
		ensurecellshidden();
		ensurebordershidden();
	}
}
