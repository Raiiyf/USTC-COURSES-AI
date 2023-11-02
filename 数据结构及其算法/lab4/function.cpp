#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "function.h"

int n = 0;
HuffTree T = NULL;
Hufftable* HT = NULL;

void printtittle()
{
	printf("------HuffmanEn&Decoding-----\n1.Initialization(I)\n2.Encoding(E)\n3.Decoding(D)\n4.Print(P)\n5.TreePrinting(T)\n6.Quit(Q)\n");
	printf("Please enter your instruction:\n");
}

void Select(HuffTree HT, int i, int &s)
{
	int j = 1;
	int temp = -1;
	s = 0;

	for (j = 1; j < i; j++) {
		if (HT[j].parent == 0) {
			if (temp == -1) {
				temp = HT[j].weight;
				s = j;
				}
			else if(HT[j].weight <= temp){
				 temp = HT[j].weight;
				 s = j;
				 }
		}
	}
}

void Initialization()
{
	FILE* fp;
	int err = fopen_s(&fp, "CharSet.txt", "r");
	if (err != 0) {
		printf("Failed to open CharSet.txt!\n");
		exit(0);
	}
	
	CharSet* head, *p, *q;
	q = head = new CharSet;
	char u;
	head->next = NULL;
	if (!feof(fp)) {
		fscanf_s(fp, "%c,%d", &head->character,1,&head->frequency);
		n++;
		u=fgetc(fp);
	}
	while (!feof(fp)) {
		p = new CharSet;
		fscanf_s(fp, "%c,%d", &p->character,1,&p->frequency);
		u=fgetc(fp);
		q->next = p;
		n++;
		q = p;
	}
	q->next = NULL;

	int m;
	m = 2 * n - 1;
	T = new HTNode[m + 1];
	int i;
	p = head;
	int s1, s2;
	
	for (i = 1; i <= m; i++) {
		if (i <= n) {
			T[i].data = p->character;
			T[i].weight = p->frequency;
			T[i].lchild = T[i].rchild = T[i].parent = 0;
			p = p->next;
		}
		else {
			T[i].data = '\0';
			T[i].weight = T[i].lchild = T[i].rchild = T[i].parent = 0;
		}
	}

	for (i = n + 1; i <= m; i++) {
		Select(T, i, s1);
		T[i].lchild = s1;
		T[s1].parent = i;
		Select(T, i, s2);
		T[i].rchild = s2;
		T[s2].parent = i;
		T[i].weight = T[s1].weight + T[s2].weight;
	}

	FILE* fhfm;
	int errhfm = fopen_s(&fhfm, "hfmtree.txt", "w");
	if (errhfm != 0) {
		printf("Failed to open hfmtree.txt!\n");
		exit(0);
	}

	for (i = 1; i <= m; i++) {
		fprintf_s(fhfm, "%c %d %d %d %d\n", T[i].data, T[i].weight, T[i].parent, T[i].lchild, T[i].rchild);
	}

	printf("Initialization sucess.\n");
	fclose(fp);
	fclose(fhfm);
}

void Push(SqStack& S, char e)
{ 
	S.elem[++S.top] = e;
}

void Pop(SqStack& S)
{
	char e;
	if (S.top != -1) {
		e = S.elem[S.top--];
	}
}

void Init_Stack(SqStack &S)
{
	S.stacksize = n;
	S.elem = new char[n];
	int i = 0;
	for (i = 0; i < n; i++) {
		S.elem[i] = '2';
	}
	S.top = -1;
}

void mstrcpy(char* &d, char* r)
{
	int s = 0;
	int i = 0;
	for (s = 0; d[s]; s++) {};
	while (r[s+i]) {
		d[i] = r[i];
		i++;
	}
	d[i] = '\0';
}

void Coding(HuffTree T, int root, Hufftable* &HT, SqStack &S)
{
	if (root!= 0) {
		if (T[root].lchild == 0) {
			Push(S, '\0');
			HT[root].Character = T[root].data;
			HT[root].Code = new char[S.top + 1];
			int j = 0;
			for (j = 0; j <= S.top; j++) HT[root].Code[j] = '\0';
			mstrcpy(HT[root].Code, S.elem);
			Pop(S);
		}

		Push(S, '0');
		Coding(T, T[root].lchild, HT, S);
		Pop(S);
		Push(S, '1');
		Coding(T, T[root].rchild, HT, S);
		Pop(S);
	}

 }

void Encoding()
{
	if (T == NULL) {//if the tree hasn't been loaded, than load the tree actively.
		FILE* fhfm;//hfmtree
		int errhfm = fopen_s(&fhfm, "hfmtree.txt", "r");
		if (errhfm != 0) {
			printf("Failed to open hfmtree.txt!\n");
			exit(0);
		}

		int m = 0;
		char row;
		while (!feof(fhfm)) {
			row = fgetc(fhfm);
			if(row == '\n')	m++;
		}
		n = (m + 1) / 2;
		rewind(fhfm);

		T = new HTNode[m + 1];
		int i = 1;
		while (!feof(fhfm)) {
			fscanf_s(fhfm, "%c %d %d %d %d", &T[i].data,1, &T[i].weight, &T[i].parent, &T[i].lchild, &T[i].rchild);
			i++;
			row=fgetc(fhfm);
		}

		printf("Load hfmtree.txt successfully.\n\n");
		fclose(fhfm);
	}

	HT = new Hufftable[n+1];
	SqStack S;
	Init_Stack(S);
	Coding(T, 2*n-1, HT, S);  //Coding. Loading result into the Memory. Form: character | code.

	//int z;
	//for (z = 1; z <= n; z++) {
	//	printf("%c,%s\n", HT[z].Character, HT[z].Code);
	//} 


	FILE* ftbt;//ToBeTran
	int errtbt = fopen_s(&ftbt, "ToBeTran.txt", "r");
	if (errtbt != 0) {
		printf("Failed to open ToBeTran.txt!\n");
		exit(0);
	}

	FILE* fcf; //CodeFile
	int errcf = fopen_s(&fcf, "CodeFile.txt", "w");
	if (errcf != 0) {
		printf("Failed to open CodeFile.txt!\n");
		exit(0);
	}

	char fpt_tbt;
	while (!feof(ftbt)) {
		fpt_tbt = fgetc(ftbt);
		if (fpt_tbt != '\n') {
			int j = 1;
			for (j = 1; j <= n; j++) {
				if (fpt_tbt == HT[j].Character) {//traverse Huffman table, find the code in accordant with.
					fprintf_s(fcf,"%s", HT[j].Code);
					break;
				}
			}
		}
	}
	printf("CodeFile.txt has been written!\n");
	fclose(ftbt);
	fclose(fcf);
}

void Print()
{
	FILE* fcf;//CodeFile
	int errcf = fopen_s(&fcf, "CodeFile.txt", "r");
	if (errcf != 0) {
		printf("Can not find CodeFile.txt! Please ENCODING first!\n");
		exit(0);
	}

	FILE* fcp;//CodePrint
	int errcp = fopen_s(&fcp, "CodePrint.txt", "w");
	if (errcp != 0) {
		printf("Failed to open CodePrint.txt!\n");
		exit(0);
	}

	int counter = 0;
	char temp;
	while (!feof(fcf)) {
		temp = fgetc(fcf);
		fputc(temp, fcp);
		printf("%c", temp);
		counter++;
		if (counter == 50) {
			fputc('\n', fcp);
			printf("\n");
			counter = 0;
		}
	}

	printf("\nCodePrint.txt has been written!\n");
	fclose(fcf);
	fclose(fcp);
}

void Decoding()
{
	if (T == NULL) {//if the tree hasn't been loaded, than load the tree actively.
		FILE* fhfm;//hfmtree
		int errhfm = fopen_s(&fhfm, "hfmtree.txt", "r");
		if (errhfm != 0) {
			printf("Failed to open hfmtree.txt!\n");
			exit(0);
		}

		int m = 0;
		char row;
		while (!feof(fhfm)) {
			row = fgetc(fhfm);
			if (row == '\n')	m++;
		}
		n = (m + 1) / 2;
		rewind(fhfm);

		T = new HTNode[m + 1];
		int i = 1;
		while (!feof(fhfm)) {
			fscanf_s(fhfm, "%c %d %d %d %d", &T[i].data, 1, &T[i].weight, &T[i].parent, &T[i].lchild, &T[i].rchild);
			i++;
			row = fgetc(fhfm);
		}

		printf("Load hfmtree.txt successfully.\n\n");
		fclose(fhfm);
	}

	FILE* ftf;//TextFile
	int errtf = fopen_s(&ftf, "TextFile.txt", "w");
	if (errtf != 0) {
		printf("Failed to open TextFile.txt!\n");
		exit(0);
	}
	FILE* fcf; //CodeFile
	int errcf = fopen_s(&fcf, "CodeFile.txt", "r");
	if (errcf != 0) {
		printf("Failed to open CodeFile.txt!\n");
		exit(0);
	}

	char dcp;
	int tcp = 2 * n - 1;

	while (!feof(fcf)) {
		dcp = getc(fcf);
		if (T[tcp].lchild == 0) {
			fputc(T[tcp].data, ftf);
			tcp = 2 * n - 1;
		}
		
		if (dcp == '0') {
			tcp = T[tcp].lchild;
		}
		if (dcp == '1') {
			tcp = T[tcp].rchild;
		}
		
	}

	printf("Decoding successfully, TextFile.txt has been written!\n");
	fclose(fcf);
	fclose(ftf);
}

void PrintTreeNode(HuffTree T, int root, int deg, char branch, FILE* fp)
{
	if (root == 0 || root == -1) return;
	else {
//		PrintTreeNode(T, T[root].lchild, deg + 1, '0', fp);

		int counter;
		for (counter = 0; counter < deg - 1; counter++) {
			printf("     ");
			fprintf_s(fp, "    ");
		}

		printf("%c(%c)\n", T[root].data, branch);
		fprintf_s(fp, "%c(%c)\n", T[root].data, branch);
		
		PrintTreeNode(T, T[root].lchild, deg + 1, '0', fp);
		PrintTreeNode(T, T[root].rchild, deg + 1, '1', fp);
	}


}

void TreePrinting()
{
	if (T == NULL) {//if the tree hasn't been loaded, than load the tree actively.
		FILE* fhfm;//hfmtree
		int errhfm = fopen_s(&fhfm, "hfmtree.txt", "r");
		if (errhfm != 0) {
			printf("Failed to open hfmtree.txt!\n");
			exit(0);
		}

		int m = 0;
		char row;
		while (!feof(fhfm)) {
			row = fgetc(fhfm);
			if (row == '\n')	m++;
		}
		n = (m + 1) / 2;
		rewind(fhfm);

		T = new HTNode[m + 1];
		int i = 1;
		while (!feof(fhfm)) {
			fscanf_s(fhfm, "%c %d %d %d %d", &T[i].data, 1, &T[i].weight, &T[i].parent, &T[i].lchild, &T[i].rchild);
			i++;
			row = fgetc(fhfm);
		}

		printf("Load hfmtree.txt successfully.\n\n");
		fclose(fhfm);
	}

	FILE* ftp;//TreePrint
	int errtp = fopen_s(&ftp, "TreePrint.txt", "w");
	if (errtp != 0) {
		printf("Failed to open TreePrint.txt!\n");
		exit(0);
	}

	PrintTreeNode(T, 2 * n - 1, 1, '@', ftp);
}