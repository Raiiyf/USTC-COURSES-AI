#include<stdio.h>

typedef struct LNode{
	int number;
	int passwd;
	struct LNode *next;
}LNode,*Linklist; 

Linklist CreateLoop(int n,int *a)
{
	LNode *L,*p,*q;
	p=L= new LNode;
	p->number=0;
	p->passwd=0;
	int i;
	for(i=0;i<n;i++)
	{
		q=p;
		p=new LNode;
		p->number=i+1;
		p->passwd=*(a+i);
		q->next=p;
	}
	p->next=L;
	return L;
}

main()
{
	Linklist L;
	int n,m;
	
	while(1){
	printf("������Լɪ�򻷵ĳ��ȣ�");
	scanf("%d",&n);
	if(n>=1) break;
	else printf("�Ƿ����ݣ����������룡\n");
	}
	
	while(1){
	printf("������m�ĳ�ʼֵ m:");
	scanf("%d",&m);
	if(m>=1) break;
	else printf("�Ƿ����ݣ����������룡\n");
	}
	
	printf("������ÿ���˵����룺\n");
	int i,a[n];
	for(i=0;i<n;i++) scanf("%d",&a[i]);
	int flag=0;
	for(i=0;i<n;i++) if(a[i]<=0) flag==1;
	
	if(flag) printf("FALSE\n");
	else{	
		L=CreateLoop(n,a);
		LNode *p=L;
	
		while(n)
		{
			while(m-1||p->number==0)
			{
				if(p->number!=0) m--;
				p=p->next;
			}
			printf("%d\n",p->number);
			p->number=0;
			m=p->passwd;
			p=p->next;
			n--;
		}
	}
}
