// adds all of the numbers between a and b using while loop
long sum_all_while(long a,long b){
   long acc = 0;
   long step = a;

    while(step <= b){
        acc += step;
        step++;
    }

    return acc;
}