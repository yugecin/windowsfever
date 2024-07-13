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
		wins.cells[i].behind = 0;
	}
}

void ensurecellshidden()
{
	for (i = 0; i < GRID_CELLS_HORZ * GRID_CELLS_VERT; i++) {
		DemoSetWindowState(wins.cells + i, NULL, grid.cellLoadingPos, nullpt, SWP_NOSIZE | SWP_NOACTIVATE);
		wins.cells[i].behind = 1;
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
	int p, backCol, t, t2;
	float ct;
	HDC hDC;
	POINT sz;

	ensuremainhidden();
	ensurecellshidden();
	ensurealtmainshown();
	srand(period.from);
	p = demostate.ms % 120;
	ct = p / 120.0f;
	if ((demostate.ms % 240) / 120) {
		t = 255 - (int) (255 * ct);
		backCol = RGB(t, t, t);
	} else {
		t = 255 - (int) (255 * ct);
		t2 = 60 - (int) (60 * ct);
		backCol = RGB(t, t2, t2);
	}
	hDC = wins.altMain.hBackDC;
	dccookie = SaveDC(hDC);
	SetDCBrushColor(hDC, backCol);
	SetDCPenColor(hDC, RGB(0, 0, 0));
	SelectObject(hDC, GetStockObject(DC_BRUSH));
	SelectObject(hDC, GetStockObject(DC_PEN));
	Rectangle(hDC, 0, 0, wins.altMain.clientSize.x - 1, wins.altMain.clientSize.y - 1);
	SelectObject(hDC, hSmallFont);
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
	sz = wins.altMain.clientSize;
	if (period.relTime % 120 < 30) {
		BitBlt(hDC, sz.x * 3 / 4, sz.y * 4 / 6, sz.x / 4, sz.y / 2, hDC, sz.x / 2, sz.y / 2, SRCCOPY);
	}
	if (period.relTime % 120 < 50) {
		BitBlt(hDC, sz.x * 1 / 4, sz.y * 5 / 6, sz.x, sz.y / 4, hDC, sz.x / 3, sz.y * 2/5, SRCCOPY);
	}
	if (period.relTime % 250 > 100) {
		BitBlt(hDC, sz.x * 2/6, sz.y * 3/5, sz.x /2, sz.y, hDC, sz.x*2/3, sz.y*4/7, SRCCOPY);
	}
	if (period.relTime % 180 > 100) {
		BitBlt(hDC, sz.x *5/8, sz.y * 3/5, sz.x, sz.y*3/7, hDC, sz.x*4/7, sz.y*3/5, SRCCOPY);
	}
	if (period.relTime % 150 < 40) {
		BitBlt(hDC, sz.x * 3/5, sz.y/6, sz.x / 2, sz.y / 2, hDC, 0, 0, SRCCOPY);
	}
	RedrawWindow(wins.altMain.hWnd, NULL, NULL, RDW_INTERNALPAINT | RDW_INVALIDATE);
	srand(demostate.ms / 120);
	for (i = 0; i < GRID_BORDERCELLS; i++) {
		if (i < 5) {
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

void creds()
{
	HDC hDC;

	ensuremainshown();
	ensurecellshidden();
	ensurealtmainhidden();
	ensurebordershidden();
	//whiteness = 255 - (period.relTime % 500) * 255 / 500;
	hDC = wins.altMain.hBackDC;
	dccookie = SaveDC(hDC);
	SelectObject(hDC, textsBitmapHandle);
	SetDCBrushColor(hDC, RGB(0, 0, 0));
	SetDCPenColor(hDC, RGB(0, 0, 0));
	SelectObject(hDC, GetStockObject(DC_BRUSH));
	SelectObject(hDC, GetStockObject(DC_PEN));
	Rectangle(hDC, 0, 0, wins.altMain.clientSize.x - 1, wins.altMain.clientSize.y - 1);
	SelectObject(hDC, hSmallFont);
	SetBkColor(hDC, RGB(0, 0, 0));
	SetBkMode(hDC, TRANSPARENT);
	tmpRect.top = grid.size.y / 2; // total height of text is supposed to fit in 3 * grid.y (because 3 lines)
	tmpRect.right = wins.altMain.clientSize.x;
	tmpRect.bottom = wins.altMain.clientSize.y;
	tmpRect.left = 0;
	// TODO WHY DOESN'T THIS OUTLINE WORK
	//SetTextColor(hDC, RGB(130, 136, 255));
	// this surely will cut off on certain screen configurations
	//DrawTextA(hDC, "code: yugecin\r\nmusic: awildbrysen\r\nmotivation: frauke", -1, &tmpRect, DT_CENTER | DT_NOCLIP | DT_NOPREFIX);
	SetTextColor(hDC, RGB(255, 255, 255));
	tmpRect.top -= 4;
	tmpRect.left -= 4;
	DrawTextA(hDC, "code: yugecin\r\nmusic: awildbrysen\r\nmotivation: frauke", -1, &tmpRect, DT_CENTER | DT_NOCLIP | DT_NOPREFIX);
	RestoreDC(hDC, dccookie);
	//RedrawWindow(wins.altMain.hWnd, NULL, NULL, RDW_INTERNALPAINT | RDW_INVALIDATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, wins.altMain.clientSize.x, wins.altMain.clientSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, textsBitMapBitsPtr);
	uniformPar.renderText = 1.0f;
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

void greetings()
{
	// this relies on having plenty of border cells :)
	static char *txt[] = {
		"RBBS",
		"Ninjadev",
		"rohtie",
		"Bongo",
		"Logicoma",
		"Slipstream",
		"lug00ber",
		"vame",
		"and you",
	};
	static char len[] = {
		4,
		8,
		6,
		5,
		8,
		10,
		8,
		4,
		7
	};

	int numgr = 9;
	float timepergreet = 1 / (float) numgr;
	HDC hDC;
	float t;
	POINT from, to;
	int index;
	POINT nextpos;
	POINT extra;

	t = period.t;
	for (index = 0; index < numgr; index++) {
		if (t < timepergreet) {
			break;
		}
		t -= timepergreet;
	}

	t /= timepergreet;
	switch (index) {
	case 0:
	case 7:
		from.x = metrics.rcFull.right + grid.size.x;
		from.y = metrics.rcWork.top + grid.size.y;
		to.x = metrics.rcFull.left - grid.size.x * (len[index] + 1);
		to.y = metrics.rcWork.bottom - grid.size.y * 2;
		nextpos.x = grid.size.x;
		nextpos.y = -grid.size.y / 10;
		to.y = from.y;
		extra.x = 0;
		extra.y = -mcos((int) (360 * t)) * grid.size.y;
		nextpos.y = -mcos((int) (360 * t) - 90) * grid.size.y / 5;
		break;
	case 1:
	case 5:
		from.x = metrics.rcWork.left + metrics.workingAreaWidth / 5;
		from.y = metrics.rcFull.top - grid.size.y * 2;
		to.x = metrics.rcWork.left + metrics.workingAreaWidth * 3 / 5;
		to.y = metrics.rcFull.bottom + grid.size.y * (len[index] + 1);
		nextpos.x = -grid.size.x / 2;
		nextpos.y = -grid.size.y;
		extra.y = 0;
		extra.x = -mcos((int) (360 * t)) * grid.size.y;
		nextpos.x = -mcos((int) (360 * t) - 90) * grid.size.y / 5;
		break;
	case 2:
	case 6:
		from.x = metrics.rcWork.left + metrics.workingAreaWidth * 4 / 5;
		from.y = metrics.rcFull.top - grid.size.y * 2;
		to.x = metrics.rcWork.left + metrics.workingAreaWidth * 3 / 5;
		to.y = metrics.rcFull.bottom + grid.size.y * (len[index] + 1);
		nextpos.x = grid.size.x / 5;
		nextpos.y = -grid.size.y;
		extra.y = 0;
		extra.x = mcos((int) (360 * t)) * grid.size.y;
		nextpos.x = mcos((int) (360 * t) - 90) * grid.size.y / 5;
		break;
	case 3:
		from.x = metrics.rcFull.right + grid.size.x;
		from.y = metrics.rcWork.bottom - grid.size.y;
		to.x = metrics.rcFull.left - grid.size.x * (len[index] + 1);
		to.y = metrics.rcWork.top + metrics.workingAreaHeight * 3 / 5;
		nextpos.x = grid.size.x;
		nextpos.y = grid.size.y / 6;
		extra.y = 0;
		extra.x = -mcos((int) (360 * t)) * grid.size.y;
		nextpos.y = -mcos((int) (360 * t) - 90) * grid.size.y / 5;
		break;
	case 4:
	case 8:
		from.x = metrics.rcWork.left + metrics.workingAreaWidth * 3 / 5;
		from.y = metrics.rcFull.bottom + grid.size.y * 2;
		to.x = metrics.rcWork.left + metrics.workingAreaWidth * 2 / 5;
		to.y = metrics.rcFull.top - grid.size.y * (len[index] + 1);
		nextpos.x = grid.size.x / 6;
		nextpos.y = grid.size.y;
		extra.y = 0;
		extra.x = -mcos((int) (360 * t)) * grid.size.y;
		nextpos.x = -mcos((int) (360 * t) - 90) * grid.size.y / 5;
		break;
	default:
		return;
	}

	ensuremainshown();
	ensurealtmainhidden();
	ensurecellshidden();

	srand(period.from);
	for (i = 0; i < GRID_BORDERCELLS; i++) {
		if (i < len[index]) {
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
			DrawTextA(hDC, txt[index] + i, 1, &tmpRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			RestoreDC(hDC, dccookie);
			tmpPos.x = from.x + (int) ((to.x - from.x) * t) + nextpos.x * i;
			tmpPos.y = from.y + (int) ((to.y - from.y) * t) + nextpos.y * i;
			tmpPos.x += (int) (mcos(i * 33 + (period.relTime - period.relTime % 30) / 2) * 13);
			tmpPos.y += (int) (mcos(i * 13 + (period.relTime - period.relTime % 30) / 2) * 13);
			tmpPos.x += extra.x;
			tmpPos.y += extra.y;
			DemoSetWindowState(wins.border + i, NULL, tmpPos, nullpt, SWP_SHOWWINDOW | SWP_NOSIZE | (wins.border[i].shown ? SWP_NOACTIVATE : 0));
			RedrawWindow(wins.border[i].hWnd, NULL, NULL, RDW_INTERNALPAINT | RDW_INVALIDATE);
		} else {
			if (wins.border[i].shown) {
				DemoSetWindowState(wins.border + i, NULL, nullpt, nullpt, SWP_HIDEWINDOW | SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
			}
		}
	}
	pumpmessages();
}

void preexplosionshake()
{
	// shake before explosion
	ensuremainshown();
	ensurealtmainhidden();
	ensurecellshidden();
	if (demostate.ms % 10) {
		tmp = (int) (100 * eq_in_quart(period.relTime / (float) period.duration));
		srand(demostate.ms);
		tmpPos.x = grid.pos.x + (randn(tmp) - (tmp) / 2);
		tmpPos.y = grid.pos.y + (randn(tmp) - (tmp) / 2);
		DemoSetWindowState(&wins.main, NULL, tmpPos, nullpt, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
	}
	ensurebordershidden();
}

void explosion()
{
	// explosion
	ensuremainhidden();
	ensurealtmainhidden();
	ensurecellsshown();
	t = eq_out_quart((period.relTime - 100) / (float) (period.duration - 200));
	if (t < 0.0f) t = 0.0f;
	else if (t > 1.0f) t = 1.0f;
	for (i = 0; i < GRID_CELLS_HORZ * GRID_CELLS_VERT; i++) {
		explosion_do(wins.cells + i, i, t);
	}
	ensurebordershidden();
}

void flashborder()
{
	HDC hDC;

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
}

void flashrandom()
{
	HDC hDC;

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
}

void borderpattern()
{
	int x, y, tmp, max, len, lowest, highest, val, pd;
	float t, tt;
	HDC hDC;

	pd = period.duration / 4;
	if (period.relTime > pd * 4) {
		ensurebordershidden();
		return;
	}
	len = 5;
	max = GRID_CELLS_HORZ + 1 + GRID_CELLS_VERT + 1 + len;
	tmp = period.relTime % pd;
	t = tmp / (float) pd;
	highest = (int) (t * (max + 2));
	lowest = highest - len;
	for (i = 0; i < GRID_BORDERCELLS; i++) {
		if (i < GRID_CELLS_HORZ + 2) {
			val = i;
		} else if (i < GRID_CELLS_HORZ + 2 + GRID_CELLS_VERT * 2) {
			tmp = i - GRID_CELLS_HORZ - 2;
			val = 1 + tmp / 2;
			val += (tmp % 2) * (GRID_CELLS_HORZ + 1);
		} else {
			val = i - (GRID_CELLS_HORZ + 2 + GRID_CELLS_VERT * 2);
			val += GRID_CELLS_VERT + 1;
		}
		if (lowest <= val && val < highest) {
			tt = (val - lowest) / (float) (highest - lowest);
			tmp = (int) (255 * tt);
			hDC = wins.border[i].hBackDC;
			dccookie = SaveDC(hDC);
			SetDCBrushColor(hDC, RGB(tmp, tmp, tmp));
			SetDCPenColor(hDC, RGB(tmp, tmp, tmp));
			SelectObject(hDC, GetStockObject(DC_BRUSH));
			SelectObject(hDC, GetStockObject(DC_PEN));
			Rectangle(hDC, 0, 0, wins.border[i].clientSize.x - 1, wins.border[i].clientSize.y - 1);
			RestoreDC(hDC, dccookie);
			DemoSetWindowState(wins.border + i, NULL, grid.borderpos[i], nullpt, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOACTIVATE);
			RedrawWindow(wins.border[i].hWnd, NULL, NULL, RDW_INTERNALPAINT | RDW_INVALIDATE);
		} else {
			DemoSetWindowState(wins.border + i, NULL, grid.borderpos[i], nullpt, SWP_HIDEWINDOW | SWP_NOSIZE | SWP_NOACTIVATE);
		}
	}
}

void brokenstate()
{
	uniformPar.brokenState = 1.0f;
	ensuremainshown();
	ensurealtmainhidden();
	ensurecellshidden();
}

void fixingstate()
{
	POINT from, to, pos;
	int x, y, flags;
	float t;

	demostate.doRenderCellsGL = 1;
	uniformPar.brokenState = 2.0f;
	t = period.t * 1.2f - .1f;
	if (t < 0.0f) t = 0.0f;
	else if (t > 1.0f) t = 1.0f;
	t = period.t; // the positions aren't perfect so fuck the before and after state
	ensuremainshown();
	ensurealtmainhidden();
	for (i = 0; i < GRID_CELLS_HORZ * GRID_CELLS_VERT; i++) {
		x = i % GRID_CELLS_HORZ;
		y = i / GRID_CELLS_HORZ;
		flags = 0;
		to = grid.cellpos[i];
		if (x == 2 && y == 2) {
			from = grid.cellpos[4 + GRID_CELLS_HORZ * 1];
		} else if (x == 4 && y == 1) {
			from = grid.cellpos[0 + GRID_CELLS_HORZ * 0];
		} else if (!x && !y) {
			from = grid.cellpos[2 + GRID_CELLS_HORZ * 2];
		} else {
			DemoSetWindowState(wins.cells + i, NULL, grid.cellLoadingPos, nullpt, SWP_NOSIZE | SWP_NOACTIVATE);
			wins.cells[i].behind = 1;
			continue;
		}
		if (!wins.cells[i].behind) {
			flags |= SWP_NOACTIVATE;
		} else {
			wins.cells[i].behind = 0;
		}
		pos.x = from.x + (int) (t * (to.x - from.x));
		pos.y = from.y + (int) (t * (to.y - from.y));
		DemoSetWindowState(wins.cells + i, NULL, pos, nullpt, SWP_NOSIZE | flags);
	}
}

void fixedstate()
{
	if (GetForegroundWindow() != wins.main.hWnd) {
		DemoSetWindowState(&wins.main, NULL, grid.pos, wins.main.frameSize, SWP_NOZORDER | SWP_SHOWWINDOW);
	}
	ensuremainshown();
	ensurealtmainhidden();
	ensurecellshidden();
}

void demotick()
{
	uniformPar.renderText = 0.0f;
	uniformPar.brokenState = 0.0f;
#if 0
	if (isperiod(0, 3000)) {
		creds();
		return;
	}
#endif
#if 0
	if (isperiod(0, 28000)) {
		greetings();
		return;
	}
#endif
#if 0
	if (isperiod(0, 7000)) {
		borderpattern();
	}
	if (isperiod(0, 2000)) {
		brokenstate();
	} else if (isperiod(2000, 5000)) {
		fixingstate();
	} else if (isperiod(5000, 7000)) {
		fixedstate();
	}
	return;
#endif
	if (isperiod(15000, 26500)) {
		borderpattern();
	}
	if (isperiod(0, 10500)) {
		// start
		ensuremainshown();
		ensurealtmainhidden();
		ensurecellshidden();
		ensurebordershidden();
	} else if (isperiod(10500, 12300)) {
		preexplosionshake();
	} else if (isperiod(12300, 15000)) {
		explosion();
	} else if (isperiod(15000, 17000)) {
		brokenstate();
	} else if (isperiod(17000, 22000)) {
		fixingstate();
	} else if (isperiod(22000, 26500)) {
		fixedstate();
	} else if (isperiod(26500, 31000)) {
		greetings_intro();
	} else if (isperiod(31000, 55000)) {
		greetings();
	} else if (isperiod(55000, 57500)) {
		creds();
	} else if (isperiod(57500, 60000)) {
		sound_stop();
		procrastination_is_a_fuck();
	} else {
		ExitProcess(0);
	}
}
