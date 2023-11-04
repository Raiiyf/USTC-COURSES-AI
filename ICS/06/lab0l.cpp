#include <stdio.h>

int main(){
    short R0=-100;     //change the value of R0 & R1 here
    short R1=-233;      
    short R7;           // R7 = R0 * R1, the result would be printed out
    if(R0 == 0) R7 = 0;
    else{
        while(R0 != 0){
            R7+= R1;
            R0--;
        }
    }
    printf("%d\n",R7);
    return 0;
}