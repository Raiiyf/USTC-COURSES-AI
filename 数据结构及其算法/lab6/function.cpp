#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "function.h"

HT* HashTable = NULL;
HVT* HashValueTable = NULL;

FV TokenFreqeuncy[5];

float Totaltime = 0;

void printtittle()
{
	printf("1.Create(C)\n2.AverageSearthLength(A)\nSimilarityDistinguish(S)\nPlease input your instruction:\n");
}

void Init_HashTable() 
{
	HashTable = new HT[43];
	int i = 0;
	for (i = 0; i < 43; i++) {
		HashTable[i].keyword[0] = '\0';
		HashTable[i].frequency = 0;
	}
}

void ResetHashTable()
{
	int i = 0;
	for (i = 0; i < 43; i++) HashTable[i].frequency = 0;
}

int Hash(char* key)
{
	int front, rear;
	front = (int)key[0];
	int i = 0;
	for (i = 0; key[i]; i++) {};
	rear = (int)key[i - 1];
	return (100 * front + rear) % 41;
}

int Secondary_Hash(char* key)
{
	int front, rear;
	front = (int)key[1];
	int i = 0;
	for (i = 0; key[i]; i++) {};
	rear = (int)key[i - 2];
	return (1 * front + rear) % 41;
}

void mstrcpy(char* d, char* r)
{
	int s = 0;
	int i = 0;
	for (s = 0; d[s]; s++) {};
	while (r[s + i]) {
		d[i] = r[i];
		i++;
	}
	d[i] = '\0';
}

void PrintHashTable(HT* T)
{
	int i;
	for (i = 0; i < 43; i++) if(T[i].keyword[0]!='\0') printf("%d %s\n", i, T[i].keyword);
}

int Maxcount = 0;

int Insert(char* token)
{
	int count = 1;
	int add = Hash(token);
	if (HashTable[add].keyword[0] != '\0') {
		do {
			add = (add + Secondary_Hash(token) + count) % 43;
			count++;
		} while (HashTable[add].keyword[0] != '\0');
	}
	mstrcpy(HashTable[add].keyword, token);

	if (count > Maxcount) Maxcount = count;

	return count;
}

void Create_HashTable()
{
	FILE* kw;
	int err = 0;
	err = fopen_s(&kw, "keyword.txt", "r");
	if (err != 0) {
		printf("Failed to open keyword.txt!\n");
		exit(0);
	}

	HashValueTable = new HVT[32];
	Init_HashTable();

	int i;
	char EnterEater;
	float Totaltime = 0;
	for (i = 0; !feof(kw); i++) {
		fscanf_s(kw, "%s", HashValueTable[i].keyword, 10);
		HashValueTable[i].value = Hash(HashValueTable[i].keyword);
//		printf("%s %d\n", HashValueTable[i].keyword, HashValueTable[i].value);
		Totaltime += Insert(HashValueTable[i].keyword);
		EnterEater = fgetc(kw);
	}

//	PrintHashTable(HashTable);
	float Avertime = Totaltime / 32;
	printf("HashTable has been created successfully.\nAverage Search Length:%f,    Maximum Search Length:%d\n", Avertime, Maxcount);
}

void HashSearch(HT* T, char* key)
{
	int index = Hash(key);
	int count = 1;

	while (T[index].keyword[0] != '\0') {
		if (strcmp(T[index].keyword, key) == 0) {
			T[index].frequency++;
			break;
		}
		else {
			index = (index + Secondary_Hash(key) + count) % 43;
			count++;
			if (count >= 5) break;
		}
	}
}

void PrintVector()
{
	int i = 0;
	int j = 0;
	for (i = 0; i < 32; i++, j++) {
		while (HashTable[j].keyword[0] == '\0') j++;
		printf("%-10s,%3d    %3d    %3d    %3d\n", HashTable[j].keyword, TokenFreqeuncy[1].FreqVector[i], TokenFreqeuncy[2].FreqVector[i], TokenFreqeuncy[3].FreqVector[i], TokenFreqeuncy[4].FreqVector[i]);
	}
}

void LoadVector(int index)
{
	int i = 0;
	int j = 0;
	for (i = 0; i < 32; i++, j++) {
		while (HashTable[j].keyword[0] == '\0') j++;
		TokenFreqeuncy[index].FreqVector[i] = HashTable[j].frequency;
//		printf("%s, %d\n", HashTable[j].keyword, TokenFreqeuncy[index].FreqVector[i]);
	}
}

int n = 1;

void ScanFile()
{
	FILE* fp;
	printf("Please input the name of file you want to scan:\n");
	char Nof[15];
	scanf_s("%s", Nof, 15);
	int i = 0;
	for (i = 0; Nof[i]; i++);
	Nof[i] = '\0';
	int err;
	err = fopen_s(&fp, Nof, "r");
	if (err != 0) {
		printf("Failed to open target file!\n");
		exit(0);
	}
	
	ResetHashTable();
	char token[255];
	char t;
	while (!feof(fp))
	{
		i = 0;
		t = fgetc(fp);
		while ((t >= 65 && t <= 90) || (t >= 97 && t <= 122) && !feof(fp)) {
			token[i] = t;
			i++;
			t = fgetc(fp);
		}

		token[i] = '\0';
		HashSearch(HashTable, token);
	}
	
	LoadVector(n++);
}

double Cosine(int i, int j)
{
	double S;
	double Xij = 0;
	double Xi = 0;
	double Xj = 0;
	int c;

	for (c = 0; c < 32; c++) {
		Xij += (double)TokenFreqeuncy[i].FreqVector[c] * TokenFreqeuncy[j].FreqVector[c];
		Xi += (double)TokenFreqeuncy[i].FreqVector[c] * TokenFreqeuncy[i].FreqVector[c];
		Xj += (double)TokenFreqeuncy[j].FreqVector[c] * TokenFreqeuncy[j].FreqVector[c];
	}

	S = Xij / (sqrt(Xi) * sqrt(Xj));
	return S;
}

double Distance(int i, int j)
{
	int c = 0;
	double Xi = 0;
	double Xj = 0;
	double D = 0;
	for (c = 0; c < 32; c++) {
		Xi += (double)TokenFreqeuncy[i].FreqVector[c] * TokenFreqeuncy[i].FreqVector[c];
		Xj += (double)TokenFreqeuncy[j].FreqVector[c] * TokenFreqeuncy[j].FreqVector[c];
	}
	Xi = sqrt(Xi);
	Xj = sqrt(Xj);

	for (c = 0; c < 32; c++) 
		D += (TokenFreqeuncy[i].FreqVector[c] / Xi - TokenFreqeuncy[j].FreqVector[c] / Xj) * (TokenFreqeuncy[i].FreqVector[c] / Xi - TokenFreqeuncy[j].FreqVector[c] / Xj);
	D = sqrt(D);
	return D;
}

void SimilarityDistinguish(int i, int j)
{
	double S;
	S = Cosine(i, j);
	if (S < 0.85) {
		printf("test%d.cpp and test%d.cpp is not similar.  Xs=%lf\n\n", i, j, S);
		return;
	}
	else {
		double D = Distance(i, j);
		if (D < 0.25) 	printf("test%d.cpp and test%d.cpp is similar.      Xs=%-8lf   Xd=%lf\n\n", i, j, S,D);
		else printf("test%d.cpp and test%d.cpp is not similar.  Xs=%-8lf   Xd=%lf\n\n", i, j, S, D);
	}
}