#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>
#include "rote/rote.h"
#include "tuitest.h"

extern int getout;
extern RoteTerm * rt;
extern WINDOW * term_win;
unsigned int autogen = 1;

FILE * f = NULL;

int tt_open_script(const char * file) {
	char logfilename[1024];
	snprintf(logfilename, sizeof(logfilename), "%s.log", file);
	f = fopen(file, "w+");
	if (!f) {
		return 0;
	}
	fprintf(f, "#!/usr/bin/env ruby\n");
	fprintf(f, "# auto-generated tuitest script\n");
	fprintf(f, "require 'tuitest'\n\n");
	fprintf(f, "Tuitest.init\n");
	fprintf(f, "verifier = Tuitest::Verifier.new(\"%s\")\n\n", logfilename);
	return 1;
}


void tt_close_script() {
	fprintf(f, "\nTuitest.close\n");
	fprintf(f, "verifier.finish\n\n");
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

static void tt_take_snapshot(char screen[TERM_ROWS][TERM_COLS]) {
	unsigned int i, j;
	for (i=0;i<TERM_ROWS;i++) {
		for (j=0;j<TERM_COLS;j++) {
			screen[i][j] = rt->cells[i][j].ch;
		}
	}
}

static void tt_generate_verification(unsigned int row, char oldrow[TERM_COLS], char newrow[TERM_COLS]) {
	unsigned int i, j;
	char * buf;
	for (i=0;i<TERM_COLS;i++) {
		if (oldrow[i] != newrow[i])
			break;
	}
	for (j=TERM_COLS-1;j>i;j--) {
		if (oldrow[j] != newrow[j])
			break;
	}
	buf = malloc(j-i+1);
	memcpy(buf, newrow+i, j-i);
	buf[j-i] = '\0';
	fprintf(f, "verifier.expect(%u, %u, \"%s\")\n", row, i, buf);
}

static void tt_generate_verifications(char oldscreen[TERM_ROWS][TERM_COLS], char newscreen[TERM_ROWS][TERM_COLS]) {
	unsigned int i;
	fprintf(f, "# begin auto-generated verification #%u \n", autogen);
	for (i=0;i<TERM_ROWS;i++) {
		if (memcmp(oldscreen[i],newscreen[i], TERM_COLS)!=0) {
			tt_generate_verification(i, oldscreen[i], newscreen[i]);
		}
	}
	fprintf(f, "# end auto-generated verification #%u \n", autogen);
	autogen++;
}

void tt_record() {
	int ch;
	struct timeval t1, t2;
	char screen[TERM_ROWS][TERM_COLS];

	gettimeofday(&t1, NULL);
	while (!getout) {
		rote_vt_draw(rt, term_win, 1, 1, NULL);
		wrefresh(term_win);

		ch = getch();
		if (ch == KEY_F(5)) {
			tt_take_snapshot(screen);
		} else if (ch == KEY_F(6)) {
			char curscreen[TERM_ROWS][TERM_COLS];

			/* first, record wait */
			gettimeofday(&t2, NULL);
			tt_record_wait((t2.tv_sec*1000 + t2.tv_usec/1000) - (t1.tv_sec*1000 + t1.tv_usec/1000));
			gettimeofday(&t1, NULL);

			/* then, take the snapshot and generate the verifications */
			tt_take_snapshot(curscreen);
			tt_generate_verifications(screen, curscreen);
		} else if (ch != ERR) {
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
