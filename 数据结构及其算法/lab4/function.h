#pragma once
#ifndef _FUNCTION_H_
#define _FUNCTION_H_

struct CharSet{
	char character;
	int frequency;
	CharSet* next;
};

typedef struct {
	int weight;
	char data;
	int parent, lchild, rchild;
}HTNode;

typedef HTNode* HuffTree;

typedef struct {
	char* elem;
	int stacksize;
	int top;
}SqStack;

struct Hufftable {
	char Character;
	char* Code;
};

void printtittle();

void Initialization();

void Encoding();

void Print();

void Decoding();

void TreePrinting();
#endif // !_FUNCTION_H_

