#include "util.h"

#include <stdio.h>
#include <stdlib.h>

void errif(bool condition, const char *errmsg){
    if(condition){
        perror(errmsg);
        exit(EXIT_FAILURE);
    }
}

int guard(int n, const char * err) {
    if (n == -1) {
        perror(err);
        exit(1);
    }
    return n;
}