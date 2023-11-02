#pragma once
#ifndef _FUNCTION_H_
#define _FUNCTION_H_

typedef struct {
	char keyword[15];
	int frequency;
}HT;

typedef struct {
	char keyword[15];
	int value;
}HVT;

typedef struct {
	int FreqVector[32];
}FV;

void printtittle();

void Create_HashTable();

void ScanFile();

void PrintVector();

void SimilarityDistinguish(int i, int j);

#endif // !_FUNCTION_H_

