#ifndef _KMP_H
#define _KMP_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int KMP(char* s, char* t, int start);

void Get_Next(char* t, int* next);

void Get_row(char* s, FILE* fp);

#endif // !_KMP_H
