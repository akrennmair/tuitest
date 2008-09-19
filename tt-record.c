#include "tuitest.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

void usage(char * argv0) {
	fprintf(stderr, "%s: usage: %s [-f] [-x] <script> <command>\n\n", argv0, argv0);
}

int main(int argc, char * argv[]) {
	const char * script;
	const char * cmd;
	int c;

	while ((c = getopt(argc, argv, "fx")) != -1) {
		switch (c) {
			case 'f':
				tt_set_fastmode(1);
				break;
			case 'x':
				tt_set_xmloutput(1);
				break;
			case '?':
				fprintf(stderr, "Error: unknown option -%c\n", optopt);
				usage(argv[0]);
				return 1;
		}
	}

	if ((argc - optind) < 2) {
		usage(argv[0]);
		return 1;
	}

	
	script = argv[optind];
	cmd = argv[optind+1];

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

	return 0;
}
