#include <stdio.h>

int main(){
    short R0=0;     //change the value of R0 & R1 here
    short R1=1;
    short R7;       // R7 = R0 * R1, the result would be printed out
    short R2;
    short R3;
    short R4;
    
    R2 = 1;
    R3 = R2 & R0;
    if(R3 != 0) R7 += R1;//bit[0]

    R4 = R1 + R1;
    R2 += R2;
    R3 = R2 & R0;
    if(R3 != 0) R7 += R4;//bit[1]

    R4 += R4;
    R2 += R2;
    R3 = R0 & R2;
    if(R3 != 0) R7 += R4;

    R4 += R4;
    R2 += R2;
    R3 = R0 & R2;
    if(R3 != 0) R7 += R4;

    R4 += R4;
    R2 += R2;
    R3 = R0 & R2;
    if(R3 != 0) R7 += R4;

    R4 += R4;
    R2 += R2;
    R3 = R0 & R2;
    if(R3 != 0) R7 += R4;

    R4 += R4;
    R2 += R2;
    R3 = R0 & R2;
    if(R3 != 0) R7 += R4;

    R4 += R4;
    R2 += R2;
    R3 = R0 & R2;
    if(R3 != 0) R7 += R4;

    R4 += R4;
    R2 += R2;
    R3 = R0 & R2;
    if(R3 != 0) R7 += R4;
    
    R4 += R4;
    R2 += R2;
    R3 = R0 & R2;
    if(R3 != 0) R7 += R4;

    R4 += R4;
    R2 += R2;
    R3 = R0 & R2;
    if(R3 != 0) R7 += R4;

    R4 += R4;
    R2 += R2;
    R3 = R0 & R2;
    if(R3 != 0) R7 += R4;

    R4 += R4;
    R2 += R2;
    R3 = R0 & R2;
    if(R3 != 0) R7 += R4;
    
    R4 += R4;
    R2 += R2;
    R3 = R0 & R2;
    if(R3 != 0) R7 += R4;

    R4 += R4;
    R2 += R2;
    R3 = R0 & R2;
    if(R3 != 0) R7 += R4;
    
    R4 += R4;
    R2 += R2;
    R3 = R0 & R2;
    if(R3 != 0) R7 += R4;

    printf("%d\n",R7);
}