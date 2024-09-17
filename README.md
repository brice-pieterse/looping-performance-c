### Loop Performance Profiling in C

The following briefly examines the effects of loop unrolling on the performance of a very basic C program:

Sum all numbers from a through b. Obviously such an algorithm could be performed without ANY loop whatsoever using a simple formula:

$\frac{(b−a+1)(a+b)}{2}$

However using a loop to perform this task provides a fitting environment for the test. We'll use the `Measure-Command` cmdlet in Powershell on Windows to measure the execution time of our program:

Compiling and running one loop at a time:

`gcc sum.c loops/loops.h loops/for.c -o sum`

Running:

`Measure-Command { ./sum 0 10000000000 }`

Here's the results:


| **Loop Type** | **a**  | **b**        | **Execution Time (ms)** |
|---------------|--------|--------------|------------------------|
| `for`         | 0      | 50           | 19                     |
| `for`         | 0      | 10000000     | 36                     |
| `for`         | 0      | 100000000    | 157                    |
| `for`         | 0      | 1000000000   | 1358                   |
| `for`         | 0      | 10000000000  | 1971                   |
| `while`       | 0      | 50           | 20                     |
| `while`       | 0      | 10000000     | 36                     |
| `while`       | 0      | 100000000    | 190                    |
| `while`       | 0      | 1000000000   | 1683                   |
| `while`       | 0      | 10000000000  | 2377                   |
| `do`          | 0      | 50           | 20                     |
| `do`          | 0      | 10000000     | 38                     |
| `do`          | 0      | 100000000    | 191                    |
| `do`          | 0      | 1000000000   | 1689                   |
| `do`          | 0      | 10000000000  | 2359                   |
| `goto`        | 0      | 50           | 21                     |
| `goto`        | 0      | 10000000     | 39                     |
| `goto`        | 0      | 100000000    | 179                    |
| `goto`        | 0      | 1000000000   | 1677                   |
| `goto`        | 0      | 10000000000  | 2367                   |
| `unrolled while`   | 0    | 50          | 20                  |
| `unrolled while`   | 0    | 10000000    | 28                  |
| `unrolled while`   | 0    | 100000000   | 90                  |
| `unrolled while`   | 0    | 1000000000  | 717                 |
| `unrolled while`   | 0    | 10000000000 | 998                 |



The obvious takeaways are:

- Exceptional performance of the unrolled while loop:

By a large margin, the unrolled while loop outperforms the other loops, especially as the number of loop iterations grows. For each iteration of a loop, we know that we will need to perform some loop control intructions which introduce overhead. By reducing the number of loop iterations, we obviously increase performance since fewer instructions need to be executed (though instruction level parallelism might negate much of the penalty of this extra check). We were able to reduce the data dependency between the different iterations by using multiple accumulators, which leverages the parallelism offered by modern CPUs. Finally, we keep the CPU's pipeline operating at full capacity (modern CPUs use branch prediction, however it can still predict wrong, which causes the CPU to flush its pipeline of speculatively loaded instructions or results).

Note that we could improve each iteration's performance GREATLY by using a constant time formula of the sum of the next 10 integers, `step << 3 + step << 1 + 55`, but this defeats the purpose of the experiment.



- Similar performance between the other loops:

This was no surprise. The loops follow a similar structure, so the assembly produced by the compiler is likely to be almost identical (confirmed in the assembly analysiss below). 

An interesting note is that when the exact same C code was compiled to ARM assembly and executed on an Apple Silicon mac, the goto loop was atleast 2x **slower** than the other standard loops (for, while, do-while). But adding a dummy variable in the middle of the goto loop that counted each iteration doubled it's performance and brought it up to par with the others. Without diving into the ARM code, my guess is the compiler made better use of the registers, bringing down the number of memory accesses. It's a good reminder that the compiler isn't perfect, it too can make silly mistakes that greatly impact our performance.



**The takeaway**: loop unrolling and multiple accumulators provide an efficient mechanism for speeding up computations that would otherwise involve a large number of loop iterations.



### Assembly Analysis

Here's a look at the various loops in assembly. Note that I used gcc to compile each loop function separately, and I did so on an ARM-based mac, so I had to specify a different target in order to generate x86 assembly:


`gcc -target x86_64-apple-darwin -S -o do.s do.c`


I've also excluded some of the unnecessary assembly (ie. alignment) so we can pay attention to the logic behind each loop. 


#### Do-While Loop

```
_sum_all_do:                           
	.cfi_startproc
	pushq	%rbp                    # stack setup 
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)          # a
	movq	%rsi, -16(%rbp)         # b
	movq	$0, -24(%rbp)           # acc
	movq	-8(%rbp), %rax          
	movq	%rax, -32(%rbp)         # step = a

LBB0_1:                             # loop start                 
	movq	-32(%rbp), %rax         # move step into rax
	addq	-24(%rbp), %rax         # add acc to step
	movq	%rax, -24(%rbp)         # set acc to step
	movq	-32(%rbp), %rax         # mov last step into rax
	addq	$1, %rax                # add one to rax
	movq	%rax, -32(%rbp)         # mov rax into step                    
	movq	-32(%rbp), %rax         # mov step into rax
	cmpq	-16(%rbp), %rax         # compare step and b
	jle	LBB0_1                      # if step <= b, loop
                                    # exit
	movq	-24(%rbp), %rax
	popq	%rbp
	retq
	.cfi_endproc
```

Statistics for the main loop (LBB0_1):
registers used: 2 (%rbp, %rax) 
jumps: 1
operations: 9


#### For Loop


```
_sum_all_for:                           
	.cfi_startproc
	pushq	%rbp                        # stack setup 
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)              # a
	movq	%rsi, -16(%rbp)             # b
	movq	$0, -24(%rbp)               # set acc to 0
	movq	-8(%rbp), %rax              # mov a into rax
	movq	%rax, -32(%rbp)             # mov rax into step var 
LBB0_1:                                 # loop start
	movq	-32(%rbp), %rax             # mov step into rax 
	cmpq	-16(%rbp), %rax             # compare b to step (arith) 
	jg	LBB0_4                          # if step > b, end
	movq	-32(%rbp), %rax             # move step into rax
	addq	-24(%rbp), %rax             # add acc to step
	movq	%rax, -24(%rbp)             # set acc to step
	movq	-32(%rbp), %rax             # mov last step into rax
	addq	$1, %rax                    # add one to rax
	movq	%rax, -32(%rbp)             # mov rax into step
	jmp	LBB0_1                          # loop
LBB0_4:
	movq	-24(%rbp), %rax
	popq	%rbp
	retq
	.cfi_endproc
```

Statistics for the main loop (LBB0_1):
registers used: 2 (%rbp, %rax) 
jumps: 2
operations: 10



#### While Loop


```
_sum_all_while:                        
	.cfi_startproc
	pushq	%rbp                        # stack setup 
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp

    movq	%rdi, -8(%rbp)              # a
	movq	%rsi, -16(%rbp)             # b
	movq	$0, -24(%rbp)               # set acc to 0
	movq	-8(%rbp), %rax              # mov a into rax
	movq	%rax, -32(%rbp)             # mov rax into step var 

LBB0_1:                                 
	movq	-32(%rbp), %rax             # mov step into rax
	cmpq	-16(%rbp), %rax             # cmp b to step
	jg	LBB0_3                          # if step > b, end
	movq	-32(%rbp), %rax             # move step into rax
	addq	-24(%rbp), %rax             # add acc to step
	movq	%rax, -24(%rbp)             # set acc to step
	movq	-32(%rbp), %rax             # mov last step into rax
	addq	$1, %rax                    # add one to rax
	movq	%rax, -32(%rbp)             # mov rax into step
	jmp	LBB0_1                          # loop
LBB0_3:
	movq	-24(%rbp), %rax
	popq	%rbp
	retq
	.cfi_endproc
```

Statistics for the main loop (LBB0_1):
registers used: 2 (%rbp, %rax) 
jumps: 2
operations: 10



#### Goto Loop


```
_sum_all_goto:
	.cfi_startproc

	pushq	%rbp                        # stack setup 
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)              # a
	movq	%rsi, -16(%rbp)             # b
	movq	$0, -24(%rbp)               # set acc to 0
	movq	-8(%rbp), %rax              # mov a into rax
	movq	%rax, -32(%rbp)             # mov rax into step var
LBB0_1:                                
	movq	-32(%rbp), %rax             # move step into rax
	addq	-24(%rbp), %rax             # add acc to step
	movq	%rax, -24(%rbp)             # set acc to step
	movq	-32(%rbp), %rax             # mov last step into rax
	addq	$1, %rax                    # add one to rax
	movq	%rax, -32(%rbp)             # mov rax into step
	movq	-32(%rbp), %rax             # mov step back into rax
	cmpq	-16(%rbp), %rax             # compare b to step
	jg	LBB0_3                          # if step > b, exit
	jmp	LBB0_1                          # loop
LBB0_3:
	movq	-24(%rbp), %rax
	popq	%rbp
	retq
	.cfi_endproc
```


Statistics for the main loop (LBB0_1):
registers used: 2 (%rbp, %rax) 
jumps: 2
operations: 10



#### Unrolled While Loop


```
_sum_all_unrolled:                      
	.cfi_startproc
	pushq	%rbp                       # stack setup 
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, -24(%rbp)
	movq	-8(%rbp), %rax
	addq	$1, %rax
	movq	%rax, -32(%rbp)
	movq	$0, -40(%rbp)               # accumulator 1
	movq	$0, -48(%rbp)               # accumulator 1
LBB0_1:                                 # loop start
	movq	-16(%rbp), %rax
	subq	-32(%rbp), %rax
	cmpq	$1, %rax
	jl	LBB0_3                          # if b - step < 1, break out of unrolled while

	movq	-32(%rbp), %rax
	addq	-40(%rbp), %rax
	movq	%rax, -40(%rbp)
	movq	-32(%rbp), %rax
	addq	$1, %rax
	addq	-48(%rbp), %rax
	movq	%rax, -48(%rbp)
	movq	-32(%rbp), %rax
	addq	$2, %rax
	movq	%rax, -32(%rbp)
	jmp	LBB0_1

LBB0_3:
	movq	-40(%rbp), %rax
	addq	-48(%rbp), %rax
	addq	-24(%rbp), %rax
	movq	%rax, -24(%rbp)
	movq	-32(%rbp), %rax
	cmpq	-16(%rbp), %rax
	jne	LBB0_5

	movq	-32(%rbp), %rax
	addq	-24(%rbp), %rax
	movq	%rax, -24(%rbp)

LBB0_5:
	movq	-24(%rbp), %rax
	popq	%rbp
	retq
	.cfi_endproc
```

Statistics for the main loop (LBB0_1):
registers used: 2 (%rbp, %rax) 
jumps: 2
operations: 15
