#ifndef SIGNAL_HANDLER_H
#define SIGNAL_HANDLER_H

#include <signal.h>

extern volatile sig_atomic_t _interruption_requested;

static void sigint_handler(int unused __attribute__((unused)))
{
    _interruption_requested = 1;
}

#endif