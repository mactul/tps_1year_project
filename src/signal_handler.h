#ifndef SIGNAL_HANDLER_H
#define SIGNAL_HANDLER_H

#include <signal.h>

extern volatile sig_atomic_t _interruption_requested;

static inline void sigint_handler(int __attribute__((unused)) sig_id)
{
    _interruption_requested = 1;
}

#endif