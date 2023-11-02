#ifndef _FUNCTION_
#define _FUNCTION_

typedef struct ArcNode {
	int adjvex;
	float weight;
	struct ArcNode* nextarc;
}ArcNode;

typedef struct VertexNode {
	int data;
	ArcNode* firstarc;
}VertexNode,AdjList[20];

typedef struct {
	AdjList vertices;
	int vexnum, arcnum;
}ALGraph;

typedef struct {
	int* elem;
	int Queuesize;
	int front;
	int rear;
}SqQueue;

void Printtittle();

void Create();

void DFStraverse();

void BFStraverse();

void MST();
#endif // !_FUNCTION_
#pragma once
