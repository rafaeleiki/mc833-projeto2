#include "time_utils.h"

long int time_passed(struct timeval *before, struct timeval *after) {
    return (after->tv_sec - before->tv_sec) * 1000000 + after->tv_usec - before->tv_usec;
}
