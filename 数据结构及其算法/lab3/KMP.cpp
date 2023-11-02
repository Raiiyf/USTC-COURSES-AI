#include "KMP.h"

int KMP(char* s, char* t, int start)
{
	void Get_Next(char* t, int* next);
	int i = start, j = 0;
	int next[10];

	Get_Next(t, next);

	while(s[i]!='\0' && t[j]!='\0') {
		if (j == -1 || s[i] == t[j]) {
			i++;
			j++;
		}
		else j = next[j];
	}
	if (t[j] == '\0') return i - j;
	else return -1;
}

void Get_Next(char* t, int* next)
{
	int i = 0, j = -1;
	next[0] = -1;
	while (t[i]) {
		if (j == -1 || t[i] == t[j]) {
			i++;
			j++;
			next[i] = j;
		}
		else j = next[j];
	}
}

void Get_row(char* s, FILE* fp)
{
	int i = 1;
	s[0] = fgetc(fp);
	while ( !feof(fp) && s[i-1] != '\n')
	{
		s[i] = fgetc(fp);
		i++;
	}

	s[i] = '\0';
}