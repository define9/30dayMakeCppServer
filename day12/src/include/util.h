#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>
#include "Socket.h"
#include <errno.h>
#include "Log.h"

void errif(bool, const char*);
int guard(int, const char*);

bool socketIsOpen(int fd);

bool connectStatus(int res);

#endif