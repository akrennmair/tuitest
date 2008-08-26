%rename(init) tt_init;
%rename(close) tt_close;
%rename(run) tt_run;
%rename(keypress) tt_keypress;
%rename(wait) tt_wait;

%module tuitest

%{

#include "../tuitest.h"

%}

%include "../tuitest.h"

%init %{
    atexit(tt_close);
%}
