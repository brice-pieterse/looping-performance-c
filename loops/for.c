#include <stdio.h>

// adds all of the numbers between a and b using for loop
long sum_all_for(long a,long b){
   long acc = 0;
   long loops = 0;

    for(long c = a; c <= b; c++){
        acc += c;
        loops++;
    }

    printf("\n %ld \n", loops);

    return acc;
}