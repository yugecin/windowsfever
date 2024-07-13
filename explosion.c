struct explosion {
	struct {
		int cellId, x, y;
	} from, to;
	int desktopX, desktopY;
};
struct explosion explosiontracker[GRID_CELLS_HORZ * GRID_CELLS_VERT];

RECT explBounds;
POINT explSize;

void explosion_init()
{
	struct explosion *et;

	srand(80284);
	for (i = 0; i < GRID_CELLS; i++) {
		explosiontracker[i].to.cellId = -1;
	}
	for (i = 0; i < GRID_CELLS; i++) {
		et = explosiontracker + i;
		et->from.cellId = et->to.cellId = i;
		et->desktopX = randn(6) - 3; /*-3 to 2*/
		et->desktopY = randn(6) - 3;
		if (et->desktopX < 0 && et->desktopX > -2) et->desktopX = -2;
		if (et->desktopX >= 0 && et->desktopX < 2) et->desktopX = 2;
		if (et->desktopY >= 0) et->desktopY++;
	}

#if GRID_CELLS_VERT < 3 || GRID_CELLS_HORZ < 5
#error "well this won't work"
#endif
#define CELLID(X,Y) (GRID_CELLS_HORZ * Y + X)
	explosiontracker[CELLID(2,2)].to.cellId = CELLID(4,1);
	explosiontracker[CELLID(4,1)].to.cellId = CELLID(0,0);
	explosiontracker[CELLID(0,0)].to.cellId = CELLID(2,2);

	explBounds.top = metrics.rcWork.top;
	explBounds.right = metrics.rcWork.right - grid.size.x;
	explBounds.bottom = metrics.rcWork.bottom - grid.size.y;
	explBounds.left = metrics.rcWork.left;
	explSize.x = explBounds.right - explBounds.left;
	explSize.y = explBounds.bottom - explBounds.top;
	for (i = 0; i < GRID_CELLS; i++) {
		et = explosiontracker + i;
		et->from.x = grid.pos.x + grid.size.x * (et->from.cellId % GRID_CELLS_HORZ);
		et->from.y = grid.pos.y + grid.size.y * (et->from.cellId / GRID_CELLS_HORZ);
		et->to.x = grid.pos.x + grid.size.x * (et->to.cellId % GRID_CELLS_HORZ);
		et->to.y = grid.pos.y + grid.size.y * (et->to.cellId / GRID_CELLS_HORZ);
		if (et->desktopX % 2) et->to.x = explBounds.right - (et->to.x - explBounds.left);
		if (et->desktopY % 2) et->to.y = explBounds.bottom - (et->to.y - explBounds.top);
		et->to.x += explSize.x * et->desktopX;
		et->to.y += explSize.y * et->desktopY;
	}
}

void explosion_do(struct win *win, int index, float progress)
{
	struct explosion *et = explosiontracker + i;
	POINT pos, isUneven;

	pos.x = et->from.x + (int) ((et->to.x - et->from.x) * progress);
	pos.y = et->from.y + (int) ((et->to.y - et->from.y) * progress);
	// check if desktop is uneven because then we need to mirror
	isUneven.x = 0;
	isUneven.y = 0;
	// move back to working area
	while (pos.x > explBounds.right) { pos.x -= explSize.x; isUneven.x ^= 1; }
	while (pos.x < explBounds.left) { pos.x += explSize.x; isUneven.x ^= 1; }
	while (pos.y > explBounds.bottom) { pos.y -= explSize.y; isUneven.y ^= 1; }
	while (pos.y < explBounds.top) { pos.y += explSize.y; isUneven.y ^= 1; }
	// mirror if uneven
	if (isUneven.x) pos.x = explBounds.right - (pos.x - explBounds.left);
	if (isUneven.y) pos.y = explBounds.bottom - (pos.y - explBounds.top);

	DemoSetWindowState(win, NULL, pos, nullpt, SWP_NOSIZE | SWP_NOACTIVATE);
}
