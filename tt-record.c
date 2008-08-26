#include "tuitest.h"
#include <stdio.h>
#include <errno.h>

void usage(char * argv0) {
	fprintf(stderr, "%s: usage: %s <script> <command>\n\n", argv0, argv0);
}

int main(int argc, char * argv[]) {
	const char * script;
	const char * cmd;
	if (argc < 3) {
		usage(argv[0]);
		return 1;
	}

	script = argv[1];
	cmd = argv[2];

	tt_init();

	if (!tt_open_script(script)) {
		fprintf(stderr, "Error: couldn't open `%s': %s\n", script, strerror(errno));
		return 1;
	}

	tt_record_run(cmd);

	tt_run(cmd);

	tt_record();

	tt_close_script();
	tt_close();
}
