#include "rote/rote.h"

void tt_keypress(int key) {
	rote_vt_draw(rt, term_win, 1, 1, NULL);
	rote_vt_keypress(rt, key);
}

void tt_wait(unsigned int msec) {
	rote_vt_update(rt);
	while (msec > 1000) {
		usleep(1000000);
		msec -= 1000;
		rote_vt_update(rt);
	}
	usleep(msec * 1000);
	rote_vt_update(rt);
}
