#include <stdio.h>
#include <stdlib.h>

#include "stack.h"

#define n 3
#define m 4
#define s 2

int main()
{
	SqStack Lot;
	LinkQueue Passage;
	InitStack(Lot, n);
	InitQueue(Passage, m);
	ElemType e;
	FILE* fp;

	int err;
	err = fopen_s(&fp, "inputData.txt", "r");
	if (err != 0) exit(0);

	printf("Please enter Operator:  Vehicle Plate:  Time: \n");
	while (1)
	{
		//scanf_s("%c", &e.Operator,3);
		//scanf_s("%d", &e.Plate,10);
		//scanf_s("%d", &e.Time,10);
		fscanf_s(fp, "%c", &e.Operator, 3);
		fscanf_s(fp, "%d", &e.Plate, 10);
		fscanf_s(fp, "%d", &e.Time, 10);
		printf("%c %d %d\n", e.Operator, e.Plate, e.Time);
		switch (e.Operator)
		{
			case 'A':Enter(Lot, Passage, e); break;
			case 'P':TraverseL(Lot); break;
			case 'W':TraverseP(Passage); break;
			case 'D':Departure(Lot, Passage, e, s);
			case 'T':Traverse(Lot, Passage); break;
			case 'C':printf("%d\n",GetLocation(Lot, e.Plate)+1); break;
			case 'E':exit(0);
			default: printf("Invalid enter!\n"); break;
		}
		//getchar();
		fgetc(fp);
	}
}
