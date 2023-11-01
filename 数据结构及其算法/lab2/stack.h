#ifndef _STACK_H
#define _STACK_H

typedef struct {
	char Operator;
	int Plate;
	int Time;
}ElemType;

typedef struct {
	ElemType* info;
	int stacksize;
	int top;
}SqStack;

typedef struct LNode {
	ElemType info;
	struct LNode* next;
}LNode,*LinkList;

typedef LinkList Queueptr;

typedef struct {
	Queueptr front;
	Queueptr rear;
	int length;
	int msize;
}LinkQueue;

void InitStack(SqStack& S, int msize);

void Push(SqStack& S, ElemType e);

void Pop(SqStack& S, ElemType& e);

bool StackFull(SqStack S);

bool StackEmpty(SqStack S);

void InitQueue(LinkQueue& Q, int l);

void Enqueue(LinkQueue& Q, ElemType e);

void Dequeue(LinkQueue& Q, ElemType& e);

void Enter(SqStack& L, LinkQueue& P, ElemType e);

void Traverse(SqStack L, LinkQueue P);

void TraverseL(SqStack L);

void TraverseP(LinkQueue P);

int GetLocation(SqStack L, int k);

void Departure(SqStack& L, LinkQueue& P, ElemType e, int s);

#endif // !_STACK_H

