#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "function.h"

int main() {
	
	printtittle();

	extern int n;
	extern HuffTree T;
	T = NULL;

	char PC;
	char p;

	while (1)
	{
		scanf_s("%c", &PC, 1);
		switch (PC) {
		case 'I': Initialization();break;
		case 'E': Encoding(); break;
		case 'P': Print(); break;
		case 'D': Decoding(); break;
		case 'T': TreePrinting(); break;
		case 'Q': exit(0);
		default:  printf("Invalid input\nplease enter your instruction:\n");
		}
		p=getchar();
	}

	
}