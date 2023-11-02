#include "KMP.h"

int main()
{
	char s[256];
	FILE* fp;

	int err;
	err = fopen_s(&fp, "../test.txt", "r");
	if (err != 0) {
		printf("Fail to open file test.txt\n");
		exit(0);
	}

	int i;
	char t[100];
	printf("Please enter the pattern string:");
	scanf("%[^\n]s", t + 1);
	t[0] = 'x';

	//for (i = 0; s[i - 1] != '\n'; i++) {
	//	s[i] = getchar();
	//}
	//s[i - 1] = '\0';

	int start=0;
	int frequency=0,flag=0,row=0;

	while (!feof(fp)) {
		Get_row(s, fp);
		row++;
		do {
			start=KMP(s, t + 1, start)+1;
			if (start != 0) {
				frequency++;
				flag = 1;
			}
		} while (start != 0);
		if (flag == 1) {
			printf("%d  %s\n", row,s);
			flag = 0;
		}
	}

	printf("Total: %d", frequency);
	printf("\n");
}