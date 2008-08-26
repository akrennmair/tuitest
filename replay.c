#include <ncurses.h>
#include "rote/rote.h"

extern RoteTerm * rt;
extern WINDOW * term_win;

void tt_keypress(int key) {
	rote_vt_draw(rt, term_win, 1, 1, NULL);
	wrefresh(term_win);
	rote_vt_keypress(rt, key);
}

void tt_wait(unsigned int msec) {
	rote_vt_draw(rt, term_win, 1, 1, NULL);
	while (msec > 1000) {
		usleep(1000000);
		msec -= 1000;
		rote_vt_draw(rt, term_win, 1, 1, NULL);
		wrefresh(term_win);
	}
	usleep(msec * 1000);
	rote_vt_draw(rt, term_win, 1, 1, NULL);
	wrefresh(term_win);
}
