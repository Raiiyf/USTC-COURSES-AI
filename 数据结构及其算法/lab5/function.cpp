#include <stdio.h>
#include <stdlib.h>

#include "function.h"

ALGraph G;
int visited[20];

void Printtittle()
{
	printf("-------CityPipeLineSystem-------\nCreate (C)\n\nDFS (D)\n\nBFS (B)\n\nMST (M)\n\nPlease enter your instruction:\n");
}

void InitGraph()
{
	G.arcnum = G.vexnum = 0;

	int i = 0;
	for (i = 0; i < 20; i++)
	{
		G.vertices[i].data = 0;
		G.vertices[i].firstarc = NULL;
	}
}

void Create()
{
	FILE* ptf;
	int err = fopen_s(&ptf, "data.txt", "r");
	if (err != 0) {
		printf("Failed to open data.txt!\n");
		exit(0);
	}

	InitGraph();

	fscanf_s(ptf, "%d %d", &G.vexnum, &G.arcnum);

	int i = 0;
	int u, v;
	float w;
	ArcNode* p, * q;
	while (!feof(ptf)) {
		fscanf_s(ptf, "%d %d %f", &u, &v, &w);

		G.vertices[u].data = u;
		p = G.vertices[u].firstarc;
		q = new ArcNode;
		q->adjvex = v;
		q->weight = w;
		q->nextarc = p;
		G.vertices[u].firstarc = q;

		G.vertices[v].data = v;
		p = G.vertices[v].firstarc;
		q = new ArcNode;
		q->adjvex = u;
		q->weight = w;
		q->nextarc = p;
		G.vertices[v].firstarc = q;
	}

	fclose(ptf);
}

void DFStraverse()
{
	void DFS(ALGraph G, int u);

	if (G.vexnum == 0) {
		printf("Graph hasn't been loaded!\n");
		return;
	}
	else {
		int i = 0;
		int count = 0;
		for (i = 0; i < G.vexnum; i++) visited[i] = 0;
		for (i = 0; i < G.vexnum; i++)
			if (visited[i] == 0) {
				count++;
				DFS(G, i);
				printf("\n");
			}
		printf("有 %d 个连通分量\n", count);
	}
}

void DFS(ALGraph G,int u)
{
	visited[u] = 1;
	printf("%d ", u);

	ArcNode* p;
	for (p = G.vertices[u].firstarc; p; p = p->nextarc)
		if (visited[p->adjvex] != 1) DFS(G, p->adjvex);
}

void InitQueue(SqQueue& Q, int msize)
{
	Q.elem = new int[msize];
	Q.Queuesize = msize;
	Q.front = Q.rear = 0;
}

void Enqueue(SqQueue& Q, int e)
{
	if ((Q.rear + 1) % Q.Queuesize == Q.front) {
		printf("Queue is full!");
		return;
	}
	else {
		Q.elem[Q.rear] = e;
		Q.rear = (Q.rear + 1) % Q.Queuesize;
	}
}

void DeQueue(SqQueue& Q, int &e)
{
	if (Q.rear != Q.front) {
		e = Q.elem[Q.front];
		Q.front = (Q.front + 1) % Q.Queuesize;
	}
}

void BFS()
{
	SqQueue Q;
	InitQueue(Q, G.vexnum + 1);
	int i;
	for (i = 0; i < G.vexnum; i++) visited[i] = 0;

	ArcNode* p;
	int v = 0;
	int count = 0;
	int u;
	for (v = 0; v < G.vexnum; v++) {
		if (visited[v] != 1) {
			count++;
			printf("%d ", v);
			visited[v] = 1;
			Enqueue(Q, v);
			while (Q.rear != Q.front) {
				DeQueue(Q, u);
				p = G.vertices[u].firstarc;
				for (p = G.vertices[u].firstarc; p; p = p->nextarc){
					if (visited[p->adjvex] != 1) {
						printf("%d ", p->adjvex);
						visited[p->adjvex] = 1;
						Enqueue(Q, p->adjvex);
					}
				}
			}
			printf("\n");
		}
	}
	printf("共有 %d 个连通分量\n", count);
}

void BFStraverse()
{
	if (G.vexnum == 0) {
		printf("Graph hasn't been loaded!\n");
		return;
	}
	else BFS();
}

int MinEdge(float lowcost[])
{
	int i = 0;
	int k = 0;
	float temp = 32767;

	for (i = 0; i < G.vexnum; i++) {
		if (lowcost[i] != 0) {
			if (lowcost[i] < temp) {
				k = i;
				temp = lowcost[i];
			}
		}
	}

	return k;
}

void Prim(int v)
{
	int adjvex[20];
	int i;
	for (i = 0; i < G.vexnum; i++) 	adjvex[i] = v;
	adjvex[v] = -1;

	float lowcost[20] = { 0 };
	ArcNode* p;
	for (p = G.vertices[v].firstarc; p; p = p->nextarc)	lowcost[p->adjvex] = p->weight;
	for (i = 0; i < G.vexnum; i++) 
		if (i != v && lowcost[i] == 0) lowcost[i] = 32767;

	int k;
	for (i = 0; i < G.vexnum - 1; i++) {
		k = MinEdge(lowcost);
		lowcost[k] = 0;
		for (p = G.vertices[k].firstarc; p; p = p->nextarc) {
			if (p->weight < lowcost[p->adjvex]) {
				lowcost[p->adjvex] = p->weight;
				adjvex[p->adjvex] = k;
			}
		}
	}

	for (i = 0; i < G.vexnum; i++) 
		if (adjvex[i] != -1) printf("(%d %d)\n", adjvex[i], i);
}

void MST()
{
	int i = 0;

	if (G.vexnum == 0) {
		printf("Graph hasn't been loaded!\n");
		return;
	}
	else	Prim(i);

}