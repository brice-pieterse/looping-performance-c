// adds all of the numbers between a and b using 2 x 2 unrolled while loop
// using multiple accumulators to test ILP
long sum_all_unrolled(long a, long b){

   long acc = a;
   long step = a + 1;
   long temp0 = 0;
   long temp1 = 0;

    // unrolled while
    while(b - step >= 1){
        temp0 += step;
        temp1 += (step + 1);
        step += 2;
    }

    acc += (temp0 + temp1);

    // add remaining if needed
    if(step == b){
        acc += step;
    }
    
    return acc;
}
