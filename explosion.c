struct explosion {
	struct {
		int cellId, x, y;
	} from, to;
	int desktopX, desktopY;
};
struct explosion explosiontracker[GRID_CELLS_HORZ * GRID_CELLS_VERT];

void explosion_init()
{
	struct explosion *et;
	int j;

	srand(80284);
	for (i = 0; i < GRID_CELLS; i++) {
		explosiontracker[i].to.cellId = -1;
	}
	for (i = 0; i < GRID_CELLS; i++) {
		et = explosiontracker + i;
		et->from.cellId = i;
		et->to.cellId = rand() / (RAND_MAX / GRID_CELLS + 1);
		et->desktopX = rand() / (RAND_MAX / 7 + 1) - 3;
		if (!et->desktopX) et->desktopX++;
		et->desktopY = rand() / (RAND_MAX / 7 + 1) - 3;
		if (!et->desktopY) et->desktopY++;
		for (j = 0; j < i; j++) {
			// what a horrible idea
			if (explosiontracker[j].to.cellId == et->to.cellId) {
				j = -1;
				et->to.cellId = (et->to.cellId + 1) % GRID_CELLS;
			}
		}
	}
	for (i = 0; i < GRID_CELLS; i++) {
		et = explosiontracker + i;
		et->from.x = grid.pos.x + grid.size.x * (et->from.cellId % GRID_CELLS_HORZ);
		et->from.y = grid.pos.y + grid.size.y * (et->from.cellId / GRID_CELLS_HORZ);
		et->to.x = grid.pos.x + grid.size.x * (et->to.cellId % GRID_CELLS_HORZ);
		et->to.y = grid.pos.y + grid.size.y * (et->to.cellId / GRID_CELLS_HORZ);
	}
}

void explosion_do(int index, float progress)
{
	struct explosion *et = explosiontracker + i;
	struct win *win = wins.cells + i;
	POINT pos, size;

	size.x = size.y = 0;
	pos.x = et->from.x + (int) ((et->to.x - et->from.x) * progress);
	pos.y = et->from.y + (int) ((et->to.y - et->from.y) * progress);
	DemoSetWindowPos(win, pos, size, SWP_NOSIZE | SWP_NOACTIVATE);
}
