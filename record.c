#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include "rote/rote.h"

extern int getout;
extern RoteTerm * rt;
extern WINDOW * term_win;

FILE * f = NULL;

int tt_open_script(const char * file) {
	f = fopen(file, "w+");
	if (!f) {
		return 0;
	}
	fprintf(f, "#!/usr/bin/env ruby\n");
	fprintf(f, "# auto-generated tuitest script\n");
	fprintf(f, "require 'tuitest'\n\n");
	fprintf(f, "Tuitest.init\n\n");
	return 1;
}


void tt_close_script() {
	fprintf(f, "\nTuitest.close\n");
	fprintf(f, "# EOF\n");
	fclose(f);
}

void tt_record_wait(unsigned int msec) {
	fprintf(f, "Tuitest.wait(%u)\n\n", msec);
}

void tt_record_keypress(int key) {
	if (key >= 0x20 && key <= 0x7E) {
		fprintf(f, "Tuitest.keypress(\"%c\"[0])\n", key);
	} else {
		fprintf(f, "Tuitest.keypress(%d)\n", key);
	}
}

void tt_record() {
	int ch;
	struct timeval t1, t2;

	gettimeofday(&t1, NULL);
	while (!getout) {
		rote_vt_draw(rt, term_win, 1, 1, NULL);
		wrefresh(term_win);

		ch = getch();
		if (ch != ERR) {
			gettimeofday(&t2, NULL);
			tt_record_wait((t2.tv_sec*1000 + t2.tv_usec/1000) - (t1.tv_sec*1000 + t1.tv_usec/1000));
			tt_record_keypress(ch);

			rote_vt_keypress(rt, ch);
			gettimeofday(&t1, NULL);
		}
	}
}

void tt_record_run(const char * cmd) {
	fprintf(f, "\nTuitest.run(\"%s\")\n\n", cmd);
}
