char *developers = "DEVELOPERS";
struct {
	float t;
	int from, to, relTime;
	int duration;
} period;

int isperiod(int from, int to)
{
	if (from <= demostate.ms && demostate.ms < to) {
		period.duration = to - from;
		period.from = from;
		period.to = to;
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

void ensurealtmainshown()
{
	if (!wins.altMain.shown) {
		DemoSetWindowState(&wins.altMain, NULL, grid.pos, nullpt, SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW);
	}
}

void ensurealtmainhidden()
{
	if (wins.altMain.shown) {
		DemoSetWindowState(&wins.altMain, NULL, nullpt, nullpt, SWP_NOMOVE | SWP_NOSIZE | SWP_HIDEWINDOW);
	}
}

POINT tmpPos;
RECT tmpRect;
int tmp, dccookie;
float t, t2, t3;

void procrastination_is_a_fuck()
{
	int p, backCol, t;
	HDC hDC;

	ensuremainhidden();
	ensurecellshidden();
	ensurealtmainshown();
	srand(period.from);
	p = demostate.ms % 120;
	if (p < 30) {
		t = 255 - (int) (255 * p / 30.0f);
		backCol = RGB(t, t, t);
	} else {
		backCol = RGB(255, 60, 60);
	}
	hDC = wins.altMain.hBackDC;
	dccookie = SaveDC(hDC);
	SetDCBrushColor(hDC, backCol);
	SetDCPenColor(hDC, RGB(0, 0, 0));
	SelectObject(hDC, GetStockObject(DC_BRUSH));
	SelectObject(hDC, GetStockObject(DC_PEN));
	Rectangle(hDC, 0, 0, wins.altMain.clientSize.x - 1, wins.altMain.clientSize.y - 1);
	SelectObject(hDC, hFont);
	SetBkColor(hDC, RGB(0, 0, 0));
	SetBkMode(hDC, TRANSPARENT);
	SetTextColor(hDC, RGB(0, 0, 0));
	tmpRect.top = grid.size.y / 2; // total height of text is supposed to fit in 3 * grid.y (because 3 lines)
	tmpRect.right = wins.altMain.clientSize.x;
	tmpRect.bottom = wins.altMain.clientSize.y;
	tmpRect.left = 0;
	// this surely will cut off on certain screen configurations
	DrawTextA(hDC, "PROCRASTINATION\r\nIS A\r\nFUCK", -1, &tmpRect, DT_CENTER | DT_NOCLIP);
	tmpRect.top -= 4;
	tmpRect.left -= 6;
	SetTextColor(hDC, RGB(255, 255, 255));
	DrawTextA(hDC, "PROCRASTINATION\r\nIS A\r\nFUCK", -1, &tmpRect, DT_CENTER | DT_NOCLIP);
	RestoreDC(hDC, dccookie);
	srand(period.from);
	if (period.relTime > 3000) {
		for (i = 0; i < (period.relTime - 3000) / 150; i++) {
			if (randn(10) < 5) {
				tmpRect.left = randn(wins.altMain.clientSize.x - 20);
				tmpRect.right = randn(20);
				tmpRect.top = randn(wins.altMain.clientSize.y - 20);
				tmpRect.bottom = randn(wins.altMain.clientSize.y);
				tmpPos.x = randn(wins.altMain.clientSize.x);
				tmpPos.y = randn(wins.altMain.clientSize.y);
			} else {
				tmpRect.left = randn(wins.altMain.clientSize.x - 20);
				tmpRect.right = randn(wins.altMain.clientSize.x);
				tmpRect.top = randn(wins.altMain.clientSize.y - 20);
				tmpRect.bottom = randn(20);
				tmpPos.x = randn(wins.altMain.clientSize.x);
				tmpPos.y = randn(wins.altMain.clientSize.y);
			}
			if (tmpRect.left + tmpRect.right > wins.altMain.clientSize.x) {
				tmpRect.right = wins.altMain.clientSize.x - tmpRect.left;
			}
			if (tmpRect.top + tmpRect.bottom > wins.altMain.clientSize.y) {
				tmpRect.bottom = wins.altMain.clientSize.x - tmpRect.top;
			}
			BitBlt(hDC, tmpRect.left, tmpRect.top, tmpRect.right, tmpRect.bottom, hDC, tmpPos.x, tmpPos.y, SRCCOPY);
		}
	}
	RedrawWindow(wins.altMain.hWnd, NULL, NULL, RDW_INTERNALPAINT | RDW_INVALIDATE);
	srand(demostate.ms % 30);
	for (i = 0; i < GRID_BORDERCELLS; i++) {
		if (i < GRID_BORDERCELLS / 2 && p < 30) {
			hDC = wins.border[i].hBackDC;
			dccookie = SaveDC(hDC);
			SetDCBrushColor(hDC, backCol);
			SetDCPenColor(hDC, backCol);
			SelectObject(hDC, GetStockObject(DC_BRUSH));
			SelectObject(hDC, GetStockObject(DC_PEN));
			Rectangle(hDC, 0, 0, wins.border[i].clientSize.x - 1, wins.border[i].clientSize.y - 1);
			RestoreDC(hDC, dccookie);
			tmpPos.x = metrics.rcWork.left + randn(metrics.workingAreaWidth - grid.size.x);
			tmpPos.y = metrics.rcWork.top + randn(metrics.workingAreaHeight - grid.size.y);
			DemoSetWindowState(wins.border + i, NULL, tmpPos, nullpt, SWP_SHOWWINDOW | SWP_NOSIZE);
			RedrawWindow(wins.border[i].hWnd, NULL, NULL, RDW_INTERNALPAINT | RDW_INVALIDATE);
		} else {
			if (wins.border[i].shown) {
				DemoSetWindowState(wins.border + i, NULL, nullpt, nullpt, SWP_HIDEWINDOW | SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
			}
		}
	}
}

void greetings_intro()
{
	static char *gr = "greetings ", *fot = "fly out to";

	char *txt;
	HDC hDC;

	ensuremainshown();
	ensurealtmainhidden();
	ensurecellshidden();

	tmpPos.y = grid.pos.y + (grid.size.y * GRID_CELLS_VERT) / 2 - grid.size.y;
	txt = gr;
	t = period.t;
	if (t > .5f) {
		t -= .5f;
		tmpPos.y += grid.size.y;
		txt = fot;
	}
	t /= .5f;
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
				DrawTextA(hDC, txt + i, 1, &tmpRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
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
}

void demotick()
{
	HDC hDC;

	if (isperiod(0, 2000)) {
		greetings_intro();
	}
	return;
	if (isperiod(0, 10000)) {
		procrastination_is_a_fuck();
	}
	return;
	if (isperiod(0, 6350)) {
		// start
		ensuremainshown();
		ensurealtmainhidden();
		ensurecellshidden();
		ensurebordershidden();
	} else if (isperiod(6350, 8150)) {
		// shake before explosion
		ensuremainshown();
		ensurealtmainhidden();
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
		ensurealtmainhidden();
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
		ensurealtmainhidden();
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
		ensurealtmainhidden();
		ensurecellshidden();
		ensurebordershidden();
	} else if (isperiod(13750, 14750)) {
		ensuremainshown();
		ensurealtmainhidden();
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
		ensurealtmainhidden();
		ensurecellshidden();
		ensurebordershidden();
	} else if (isperiod(15000, 19000)) {
		ensuremainshown();
		ensurealtmainhidden();
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
		ensurealtmainhidden();
		ensurecellshidden();
		ensurebordershidden();
	}
}
