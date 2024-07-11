
POINT tmpPos;
int tmp;
float t;

void demotick()
{
	if (demostate.ms > 11150) {
		if (!IsWindowVisible(wins.main.hWnd)) {
			DemoSetWindowPos(&wins.main, grid.pos, wins.main.clientSize, SWP_NOZORDER | SWP_SHOWWINDOW | SWP_NOSIZE);
			for (i = 0; i < GRID_CELLS_HORZ * GRID_CELLS_VERT; i++) {
				DemoCalcCellLoadingPos(&tmpPos);
				SetWindowPos(wins.cells[i].hWnd, NULL, tmpPos.x, tmpPos.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
			}
			demostate.doRenderGL = 1;
			demostate.doRenderCellsGL = 0;
		}
	} else if (demostate.ms > 8150) {
		if (IsWindowVisible(wins.main.hWnd)) {
			for (i = 0; i < GRID_CELLS_HORZ * GRID_CELLS_VERT; i++) {
				DemoRestoreWindow(wins.cells + i, SWP_NOSIZE | SWP_NOACTIVATE);
			}
			demostate.doRenderGL = 1;
			demostate.doRenderCellsGL = 1;
			ShowWindow(wins.main.hWnd, SW_HIDE);
		}
		t = eq_out_cubic((demostate.ms - 8250) / 2800.0f);
		if (t < 0.0f) t = 0.0f;
		else if (t > 1.0f) t = 1.0f;
		for (i = 0; i < GRID_CELLS_HORZ * GRID_CELLS_VERT; i++) {
			explosion_do(wins.cells + i, i, t);
		}
	} else {
		if (!IsWindowVisible(wins.main.hWnd)) {
			DemoSetWindowPos(&wins.main, grid.pos, wins.main.clientSize, SWP_NOZORDER | SWP_SHOWWINDOW | SWP_NOSIZE);
			for (i = 0; i < GRID_CELLS_HORZ * GRID_CELLS_VERT; i++) {
				DemoCalcCellLoadingPos(&tmpPos);
				SetWindowPos(wins.cells[i].hWnd, NULL, tmpPos.x, tmpPos.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
			}
			demostate.doRenderGL = 1;
			demostate.doRenderCellsGL = 0;
		}
		if (demostate.ms > 6350 && demostate.ms % 10) {
			tmp = (int) (100 * eq_in_quart((demostate.ms - 6350) / 1800.0f));
			srand(demostate.ms);
			wins.main.framePos.x = grid.pos.x + (randn(tmp) - (tmp) / 2);
			wins.main.framePos.y = grid.pos.y + (randn(tmp) - (tmp) / 2);
			DemoSetWindowPos(&wins.main, wins.main.framePos, wins.main.clientSize, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
		}
	}
}
