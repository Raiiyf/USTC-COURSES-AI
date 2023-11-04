//input in R0, returns in R1
#include<stdio.h>

short Judge(short R0)
{//if R0 is a prime number, return 1, else return 0
    short R1,R2,R3,R4,R5;
    for(R1 = 1, R2 = 2;;R2++){
        R4 = R2;
        R3 = 0;
        while(R4){
            R3 += R2;
            R4--;
        }
        if(R3 > R0) return 1;
        R5 = R0;
        while(R5 > 0) R5 -= R2;
        if(R5 == 0) return 0;
    }
}

int main(){
    //This program judges whether R0 is a prime number, the result will be printed out.
    short R0 = 7;
    printf("R1 = %d\n",Judge(R0));
    return 0;
}