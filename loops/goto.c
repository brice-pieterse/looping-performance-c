#include <stdio.h>

// adds all of the numbers between a and b using do while loop
long sum_all_goto(long a, long b){
   long acc = 0;
   long step = a;
//    long loops = 0;

    sum_all:
        acc += step;
        step++;
        // loops++;
        if(step <= b)
            goto sum_all;

    // printf("\n %ld \n", loops);

    return acc;
}
