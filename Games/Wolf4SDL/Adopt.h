#ifndef ADOPT_H
#define ADOPT_H

#include <stdlib.h>
#include <stdio.h>

int read(FILE* f, void* buf, int len);
int write(FILE* f, void* buf, int len);
long lseek(FILE* f, long pos, int origin);
int exists(char* fileName);

int mkdir(char* dirName);
void unlink(char* file);

#endif