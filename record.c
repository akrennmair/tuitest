#include <stdio.h>

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
