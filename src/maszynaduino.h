#ifndef MASZYNADUINO_H
#define MASZYNADUINO_H

#ifndef ARRAY_LENGTH
#define ARRAY_LENGTH(x) (sizeof (x) / sizeof (x[0]))
#endif

#include "mux.h"
#include "debugmonitor.h"
#include "console.h"
#include "switches.h"
#include "indicators.h"
#include "comm.h"
#include "version.h"

extern void mux_setup_interrupts();

#endif
