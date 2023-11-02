#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "function.h"

int main()
{
	Create_HashTable();
	int i;
	for (i = 0; i < 4; i++)	ScanFile();
	PrintVector();

	SimilarityDistinguish(1, 2);
	SimilarityDistinguish(1, 3);
	SimilarityDistinguish(1, 4);
	SimilarityDistinguish(2, 3);
	SimilarityDistinguish(2, 4);
	SimilarityDistinguish(3, 4);
}