#ifndef TUITEST__H
#define TUITEST__H

void tt_init();
void tt_close();

void tt_run(const char * cmdline);
void tt_keypress(int key);
void tt_wait(unsigned int msec);

int tt_open_script(const char * file);
void tt_close_script();
void tt_record_wait(unsigned int msec);
void tt_record_keypress(int key);

void tt_record();


#endif
