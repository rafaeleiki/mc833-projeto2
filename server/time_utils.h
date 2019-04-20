#ifndef TIME_UTILS_H
#define TIME_UTILS_H

#include <sys/time.h>

long int time_passed(struct timeval *before, struct timeval *after);

#endif