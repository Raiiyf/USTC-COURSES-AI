#include<stdio.h>

void recursion(short *R1,short &R0)
{
    R0++;
    *R1 = *R1 - 1;
    if(*R1 != 0) recursion(R1,R0);
}

int main(){
    //just initiate the program, the result will be printed out.
    short R0 = 0;
    short R1 = 5;
    recursion(&R1,R0);
    printf("R0:%d\tR1:%d\n",R0,R1);
}