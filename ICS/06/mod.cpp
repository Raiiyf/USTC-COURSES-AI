#include<stdio.h>

short DIV8(short R1)
{
    short R2,R3,R4,R5;
    R4 = 0;
    R2 = 1;
    R3 = 8;
    while(R3){
        R5 = R1 & R3;
        if(R5 != 0) R4 += R2;
        R2 += R2;
        R3 += R3;
    }
    return R4;
}

int main(){
    //This program calculates R1 mod 7, the result will be stored in R1 and will also be printed out.
    short R1 = 288;
    short R0 = 0,R2 = 0,R4 = 0;
    do
    {
        R2 = R1 & 7;
        R4 = DIV8(R1);
        R1 = R2 + R4;
        R0 = R1 - 7;
    }while(R0>0);
    if(R0 == 0) R1 -= 7;
    printf("%d\n",R1);
    return 0;
}