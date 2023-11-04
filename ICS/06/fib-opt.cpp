#include <stdio.h>

int main(){
    short R0 = 18;  //change the value of R0 here
    short R1;
    short R2;
    short R3;
    short R4;
    short R5;
    short R7;       //output in R7, which would also be printed out

    R5 = 0x3ff;
    R1 = 1;
    R2 = 1;
    R3 = 2;
    R7 = 1;

    R0 -= 2;
    if(R0 == 0){
        R7 = R3;
        printf("%d\n",R7);
        return 0;
    }
    else if(R0 < 0) {
        printf("%d\n",R7);
        return 0;
    }

    while(R0)
    {
        R7 = R3 + R1;
        R7 = R7 + R1;
        R1 = R2;
        R2 = R3;
        R3 = R7 & R5;
        R0--;
    }
    R7 = R3;
    printf("%d\n",R7);
}