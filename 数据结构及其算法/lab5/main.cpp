#include <stdio.h>
#include <stdlib.h>

#include "function.h"

int main()
{
	Printtittle();

	while (1)
	{
		char c;
		char bin;
		scanf_s("%c", &c, 1);
		bin = getchar();

		switch (c) {
		case 'C': Create(); break;
		case 'D': DFStraverse(); break;
		case 'B': BFStraverse(); break;
		case 'M': MST(); break;
		case 'Q': exit(0);
		default: printf("error!\n"); break;
		}
	}
}