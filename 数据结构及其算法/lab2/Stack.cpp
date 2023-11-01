#include <stdio.h>
#include <stdlib.h>

#include "stack.h"

void InitStack(SqStack &S, int msize)
{
	S.info = new ElemType[msize];
	S.stacksize = msize;
	S.top = -1;
}

void Push(SqStack &S, ElemType e)
{
	if (StackFull(S)) printf("The Stack has been full!\n");
	else S.info[++S.top] = e;
}

void Pop(SqStack &S, ElemType &e)
{
	if (StackEmpty(S)) printf("The Stack has been empty!\n");
	else e = S.info[S.top--];
}

bool StackFull(SqStack S)
{
	if (S.top == S.stacksize - 1) return true;
	else return false;
}

bool StackEmpty(SqStack S)
{
	if (S.top == - 1) return true;
	else return false;
}

void InitQueue(LinkQueue& Q,int l)
{
	Q.front = Q.rear = new LNode;
	Q.front->next = NULL;
	Q.length = 0;
	Q.msize = l;
}

void Enqueue(LinkQueue& Q, ElemType e)
{
	if (Q.length < Q.msize) {
		LinkList p = new LNode;
		p->next = NULL; p->info = e;
		Q.rear->next = p;
		Q.rear = p;
		Q.length++;
	}
	else printf("Enqueue False!\n");

}

void Dequeue(LinkQueue& Q, ElemType& e)
{
	if (Q.front == Q.rear) printf("Dequeue False!\n");
	else {
		LinkList p;
		p = Q.front->next;
		Q.front->next = p->next;
		e = p->info;
		if (Q.rear == p)Q.rear = Q.front;
		delete p;
		Q.length--;
	}
}

void Enter(SqStack& L,LinkQueue& P, ElemType e)
{
	if (StackFull(L))
		if (P.length == P.msize) printf("All the space is full!\n");
		else {
			Enqueue(P, e);
			printf("Vehicle %d parks at No. %d room in passage.\n",P.rear->info.Plate, P.length);
		}
	else {
		Push(L, e);
		printf("Vehicle %d parks at No. %d room in lot.\n", L.info[L.top].Plate, L.top + 1);
	}
}

void Traverse(SqStack L, LinkQueue P)
{
	if (StackEmpty(L)) printf("There is no car in the parking lot!\n");
	else {
		int i;
		for (i = 0; i <= L.top; i++) printf("Vehicle % d parks at No. % d room in lot. Arrival time: %d\n", L.info[i].Plate, i + 1, L.info[i].Time);
		if (StackFull(L) && P.rear != P.front)
		{
			LinkList p = P.front->next;
			int j = 1;
			while (p)
			{
				printf("Vehicle %d parks at No. %d room in passage. Arrival time: %d\n", p->info.Plate, j++, p->info.Time);
				p = p->next;
			}
		}
	}
}

void TraverseL(SqStack L)
{
	if (StackEmpty(L)) printf("There is no car in the parking lot!\n");
	else {
		int i;
		for (i = 0; i <= L.top; i++) printf("Vehicle % d parks at No. % d room in lot.\nArrival time: %d\n", L.info[i].Plate, i + 1,L.info[i].Time);
	}
}

void TraverseP(LinkQueue P)
{
	if (P.rear != P.front)
	{
		LinkList p = P.front->next;
		int j = 1;
		while (p)
		{
			printf("Vehicle %d parks at No. %d room in passage.\nArrival time: %d\n", p->info.Plate, j++,p->info.Time);
			p = p->next;
		}
	}
	else printf("There is no car in the passage!\n");
}

int GetLocation(SqStack L, int k)
{
	if (!StackEmpty(L))
	{
		int i = 0;
		while (i <= L.top)
		{
			if (k == L.info[i].Plate) return i;
			i++;
		}
	}
	printf("Can not find car %d!\n", k);
	return -1;
}

void Departure(SqStack& L, LinkQueue& P,ElemType e,int s)
{
	int k;
	ElemType temp,leave;
	k = GetLocation(L, e.Plate);
	if(k!=-1)
	{
		if (k == L.top) Pop(L, leave);
		else
		{
			SqStack Cache;
			InitStack(Cache, L.top - k);
			while (!StackFull(Cache))
			{
				Pop(L, temp);
				Push(Cache, temp);
			}
			Pop(L, leave);
			while (!StackEmpty(Cache))
			{
				Pop(Cache, temp);
				Push(L, temp);
			}
		}
		printf("Vehicle %d is leaving the lot.\nDuration %d. \nCharges $%d \n\n", leave.Plate, e.Time - leave.Time, (e.Time - leave.Time) * s);
		if (P.front != P.rear)
		{
			Dequeue(P, temp);
			temp.Time = e.Time;
			Push(L, temp);
		}
	}

}