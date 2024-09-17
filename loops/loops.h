#ifndef LOOPS_H
#define LOOPS_H

// adds all of the numbers between a and b using do while loop
long sum_all_do(long a, long b);

// adds all of the numbers between a and b using for loop
long sum_all_for(long a, long b);

// adds all of the numbers between a and b using do while loop
long sum_all_goto(long a, long b);

// adds all of the numbers between a and b using 2 x 2 unrolled while loop
// using multiple accumulators to test ILP
long sum_all_unrolled(long a, long b);

// adds all of the numbers between a and b using while loop
long sum_all_while(long a,long b);

#endif /* LOOPS_H */