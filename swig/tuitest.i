%rename(init) tt_init;
%rename(close) tt_close;
%rename(run) tt_run;
%rename(keypress) tt_keypress;
%rename(wait) tt_wait;
%rename(getrow) tt_getrow;

%module tuitest

%{

#include "../tuitest.h"

%}

%feature("autodoc", "0");

void tt_init();
void tt_close();
void tt_run(const char * cmdline);
void tt_keypress(int key);
void tt_wait(unsigned int msec);
char * tt_getrow(unsigned int row);


%init %{
    atexit(tt_close);
%}
