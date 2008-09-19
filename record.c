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
int fastmode = 0;
int xmloutput = 0;

FILE * f = NULL;

static void tt_record_wait_until_idle() {
	fprintf(f, "\nTuitest.wait_until_idle\n");
}

void tt_set_fastmode(int fast) {
	fastmode = fast;
}

void tt_set_xmloutput(int xml) {
	xmloutput = xml;
}

static void escape_quotes(char * target, const char * src) {
	for (;*src;src++,target++) {
		if (*src == '"')
			*target++ = '\\';
		*target = *src;
	}
	*target = '\0';
}

int tt_open_script(const char * file) {
	char logfilename[1024];
	char xmlfilename[1024];
	char escaped_logfile[2048];
	char escaped_xmlfile[2048];
	snprintf(logfilename, sizeof(logfilename), "%s.log", file);
	snprintf(xmlfilename, sizeof(xmlfilename), "RESULT-%s.xml", file);
	escape_quotes(escaped_logfile, logfilename);
	escape_quotes(escaped_xmlfile, xmlfilename);
	f = fopen(file, "w+");
	if (!f) {
		return 0;
	}
	fprintf(f, "#!/usr/bin/env ruby\n");
	fprintf(f, "# auto-generated tuitest script\n");
	fprintf(f, "require 'tuitest'\n\n");
	fprintf(f, "Tuitest.init\n");
	if (xmloutput) {
		fprintf(f, "verifier = Tuitest::Verifier.new(\"%s\", \"%s\")\n\n", escaped_logfile, escaped_xmlfile);
	} else {
		fprintf(f, "verifier = Tuitest::Verifier.new(\"%s\")\n\n", escaped_logfile);
	}
	return 1;
}


void tt_close_script() {
	if (fastmode)
		tt_record_wait_until_idle();
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
		fprintf(f, "Tuitest.keypress(\"%s%c\"[0])\n", key == '"' ? "\\" : "", key);
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

static char * tt_getlinediff(char oldrow[TERM_COLS], char newrow[TERM_COLS], unsigned int * column) {
	unsigned int i, j;
	char * buf;
	char * escbuf;
	for (i=0;i<TERM_COLS;i++) {
		if (oldrow[i] != newrow[i])
			break;
	}
	for (j=TERM_COLS-1;j>i;j--) {
		if (oldrow[j] != newrow[j])
			break;
	}
	buf = malloc(j-i+2);
	memcpy(buf, newrow+i, j-i+1);
	buf[j-i+1] = '\0';
	escbuf = malloc(strlen(buf)*2 + 1);
	escape_quotes(escbuf, buf);
	free(buf);
	*column = i;
	return escbuf;
}

static void tt_generate_wait_until_expected_text(unsigned int row, char oldrow[TERM_COLS], char newrow[TERM_COLS]) {
	unsigned int col = 0;
	char * buf = tt_getlinediff(oldrow, newrow, &col);
	fprintf(f, "if not Tuitest.wait_until_expected_text(%u, %u, \"%s\", 0) then # TODO: set timeout to required limit (milliseconds)\n", row, col, buf);
	fprintf(f, "\tverifier.log_fail(\"Waiting for `%s' on position (%u, %u) timed out.\", :warn)\n", buf, row, col);
	fprintf(f, "end\n");
	free(buf);
}

static void tt_generate_verification(unsigned int row, char oldrow[TERM_COLS], char newrow[TERM_COLS]) {
	unsigned int col = 0;
	char * buf = tt_getlinediff(oldrow, newrow, &col);
	fprintf(f, "verifier.expect(%u, %u, \"%s\")\n", row, col, buf);
	free(buf);
}

static void tt_generate_verifications(char oldscreen[TERM_ROWS][TERM_COLS], char newscreen[TERM_ROWS][TERM_COLS], int first_verification_is_wait) {
	unsigned int i;
	if (fastmode && !first_verification_is_wait) {
		tt_record_wait_until_idle();
	}
	fprintf(f, "\n# begin auto-generated verification #%u \n", autogen);
	for (i=0;i<TERM_ROWS;i++) {
		if (memcmp(oldscreen[i],newscreen[i], TERM_COLS)!=0) {
			if (first_verification_is_wait) {
				first_verification_is_wait = 0;
				tt_generate_wait_until_expected_text(i, oldscreen[i], newscreen[i]);
			}
			tt_generate_verification(i, oldscreen[i], newscreen[i]);
		}
	}
	fprintf(f, "# end auto-generated verification #%u \n\n", autogen);
	autogen++;
}

void tt_record() {
	int ch;
	struct timeval t1, t2;
	char screen[TERM_ROWS][TERM_COLS];

	tt_take_snapshot(screen);
	gettimeofday(&t1, NULL);

	while (!getout) {
		rote_vt_draw(rt, term_win, 1, 1, NULL);
		wrefresh(term_win);

		ch = getch();
		if (ch == KEY_F(5)) {
			tt_take_snapshot(screen);
		} else if (ch == KEY_F(6) || ch == KEY_F(7)) {
			char curscreen[TERM_ROWS][TERM_COLS];

			/* first, record wait */
			gettimeofday(&t2, NULL);
			if (!fastmode && ch != KEY_F(7))
				tt_record_wait((t2.tv_sec*1000 + t2.tv_usec/1000) - (t1.tv_sec*1000 + t1.tv_usec/1000));
			gettimeofday(&t1, NULL);

			/* then, take the snapshot and generate the verifications */
			tt_take_snapshot(curscreen);
			tt_generate_verifications(screen, curscreen, ch == KEY_F(7));

			/* after that, take a new snapshot, so that we can subsequently press F6 */
			tt_take_snapshot(screen);
		} else if (ch != ERR) {
			gettimeofday(&t2, NULL);
			if (!fastmode)
				tt_record_wait((t2.tv_sec*1000 + t2.tv_usec/1000) - (t1.tv_sec*1000 + t1.tv_usec/1000));
			tt_record_keypress(ch);

			rote_vt_keypress(rt, ch);
			gettimeofday(&t1, NULL);
		}
	}
}

void tt_record_run(const char * cmd) {
	char * esccmd = malloc(strlen(cmd)*2 + 1);
	escape_quotes(esccmd, cmd);
	fprintf(f, "\nTuitest.run(\"%s\")\n\n", esccmd);
	if (fastmode)
		tt_record_wait_until_idle();
	free(esccmd);
}
