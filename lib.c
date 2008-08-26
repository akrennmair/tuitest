#include <ncurses.h>
#include <signal.h>
#include "tuitest.h"
#include "rote/rote.h"

int getout = 0;
RoteTerm * rt = NULL;
WINDOW * term_win = NULL;

static void sigchld(int signo) { 
	getout = 1; 
}

void tt_init() {
	int i, j;
	signal(SIGCHLD, sigchld);

	/* initialize ncurses */
	initscr();
	noecho();
	start_color();
	raw();
	nodelay(stdscr, TRUE);
	keypad(stdscr, TRUE);

	/* initialize colors */
	for (i = 0; i < 8; i++) {
		for (j = 0; j < 8; j++) {
			if (i != 7 || j != 0)
				init_pair(j*8+7-i, i, j);
		}
	}
	refresh();

	/* create window in which the AUT will run */
	term_win = newwin(27,82,1,1);
	wattrset(term_win, COLOR_PAIR(7*8+7-0)); /* black over white */
	wborder(term_win, 0, 0, 0, 0, 0, 0, 0, 0);
	mvwprintw(term_win, 0, 27, "[ Application Under Test ]");
	wrefresh(term_win);

	rt = rote_vt_create(25,80);
}

void tt_close() {
	rote_vt_destroy(rt);
	rt = NULL;
	endwin();
}

void tt_run(const char * cmdline) {
	rote_vt_forkpty(rt, cmdline);
}

