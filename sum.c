#include <stdio.h>
#include <stdlib.h>
#include "./loops/loops.h"



long sum_all_instant(long a, long b){    
    return ((b - a + 1) * (a+b)) >> 1;
}


// helper
unsigned char get_operands(char *a, char *b, long arr[2]){

    int op1 = 0;
    int op2 = 0;
    char index = 0;

    // extract first operand
    while(a[index] >= 48 && a[index] <= 57){
        if(op1 > 0){
            // we moved down a power
            op1 *= 10;
        }
        op1 += (a[index] - 48);
        index++;
    }

    if(a[index] != '\0' && a[index] != '\n'){
        // invalid input, expect numeric operands
        return 0;
    }
    
    index = 0;

    // extract second operand
    while(b[index] >= 48 && b[index] <= 57){
        if(op2 > 0){
            // we moved down a power
            op2 *= 10;
        }
        op2 += (b[index] - 48);
        index++;
    }

    if(b[index] != '\0' && b[index] != '\n'){
         // invalid input, expect numeric operands
        return 0;
    }

    arr[0] = op1;
    arr[1] = op2;

    return 1;

}


int main(int argc, char *argv[]){

   long operands[2];

    if (argc < 3 || !get_operands(argv[1], argv[2], operands)) {
        fprintf(stderr, "An error occurred.\n");
        exit(EXIT_FAILURE);
    }

    printf("\nOperands: %ld  %ld \n", operands[0], operands[1]);

    // test various loops (uncomment to test a loop)
    
    // long result = sum_all_for(operands[0], operands[1]);
    // long result = sum_all_while(operands[0], operands[1]);
    // long result = sum_all_do(operands[0], operands[1]);
    // long result = sum_all_goto(operands[0], operands[1]);
    long result = sum_all_unrolled(operands[0], operands[1]);
    // long result = sum_all_instant(operands[0], operands[1]);

    printf("\nSummation result: %ld \n", result);

    printf("Program executed successfully.\n");
    exit(EXIT_SUCCESS);
}