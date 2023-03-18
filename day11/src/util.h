#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include "Socket.h"
#include <errno.h>
#include <iostream>

void errif(bool, const char*);
int guard(int, const char*);

bool socketIsOpen(int fd);

bool connectStatus(int res);

#endif