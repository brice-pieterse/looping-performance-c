// adds all of the numbers between a and b using do while loop
long sum_all_do(long a,long b){
   long acc = 0;
   long step = a;

    do {
        acc += step;
        step++;
    } 
    while(step <= b);

    return acc;
}