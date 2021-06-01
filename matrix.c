/* 

Philip Massouh-- CSE 2431 Lab4

*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <string.h>


#define N 1200				/* Matrix dimensions */
#define M 1000
#define P 500
#define MAX_THREADS 7		/* Maximum number of threads for computation */

int A[N][M];				/* Matrices */
int B[M][P];			
int C1[N][P];			
int C[N][P];		

struct thread_task {
	int thread_count, location;
};


void init_A();				/* initialize matrix A */
void init_B();				/* initialize matrix B */	
void init_C1();				/* single thread answer matrix C */	
void *init_C(void* args);	/* multi thread answer matrix C */	
int C_C1_check();			/* check single with multi for errors*/
void print_Cs();

int main(int argc, char*argv[]) {
	int thread_count, i, j;
	struct timespec start, finish;
	double elapsed;
	

	if (argc != 2 || 
		(thread_count = atoi(argv[1])) < 1 || 
			thread_count > MAX_THREADS) {
		printf("Bad argument. Expected single integer between 1 and 7.\n");
		exit(-1);
	}

	/* Calculate and benchmark C1 and start output */
	init_A();
	init_B();
	clock_gettime(CLOCK_MONOTONIC, &start);
	init_C1();
	clock_gettime(CLOCK_MONOTONIC, &finish);
	elapsed = (finish.tv_sec - start.tv_sec);
	elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
	printf("Threads     Seconds      Error\n");
	printf("\n1           %f     N/A     \n", elapsed);

	/* This part of the code fails if I try to write it intuitively!!
	Somehow isolating one step of mutiliplication is the only
	way I've been able to get it working otherwise the first row is just 0's */
	/* AB = C  with different thread counts */
	for (i = 1; i < thread_count; i++) {

		/* Prepare threads and the data they will need */
		pthread_t* threads;
		struct thread_task arguments[MAX_THREADS];
		struct thread_task whyMustIDoThis;
		threads = (pthread_t*) malloc(sizeof(pthread_t) * thread_count);

		/* Start clock */
		clock_gettime(CLOCK_MONOTONIC, &start);

		/* Create threads and multiply*/
		for (j = 1; j <= i; j++) {
			arguments[j-1].thread_count = i + 1;
			arguments[j-1].location = j;
			if (pthread_create(&threads[j], NULL, init_C, (void *)&arguments) != 0) {
				printf("ERROR: Thread creation failure.\n");
				exit(-1);
			}
		}

		/* if i reindex code and dont do this, it fails */
		/* if i try putting it at the beginning, it fails s*/
		whyMustIDoThis.thread_count = i + 1;
		whyMustIDoThis.location = 0;
		init_C((void *)&whyMustIDoThis);

		for(j = 1; j <= i; j++){
  			pthread_join(threads[j], NULL);
		}

		/* End clock */
		clock_gettime(CLOCK_MONOTONIC, &finish);
		elapsed = (finish.tv_sec - start.tv_sec);
		elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;

		/* Output with error checking */
		printf("\n%d           %f     ", i+1, elapsed);
		if (C_C1_check()) {
			printf("No error\n");
		} else {
			printf("Error\n");
		}

		/* print_Cs(); */

		/* Wipe matrix C, threads, and their arguments before next attempt */
    	memset(C, 0, sizeof(int)*P);
		memset(&arguments, 0, thread_count*sizeof(struct thread_task));
	}

	return 0;
}

void init_A() {
	int i, j;
	for (i = 0; i < N; i++) {
		for (j = 0; j < M; j++) {
			A[i][j] = i + j;
		}
	}
}

void init_B() {
	int i, j;
	for (i = 0; i < M; i++) {
		for (j = 0; j < P; j++) {
			B[i][j] = j;
		}
	}
}

void init_C1() {
	int i, j, k;
	for (i = 0; i < N; i++) {
    	for (j = 0; j < P; j++) {
         	C1[i][j] = 0;
			for (k = 0; k < M; k++) {
				C1[i][j] += A[i][k] * B[k][j];
        	}
   		}
	}
}
/* TODO: re-expand calculations... this is unreadable */
void *init_C(void* args) {
	int i, j, k;
	int row_bounds[2];
	struct thread_task *this_thread;

	/* Determine where to start and stop multiplication */
	this_thread = (struct thread_task*) args;
	/* basically we start at whats been done * number of rows / number of threads */
	row_bounds[0] = (this_thread->location * N) / this_thread->thread_count;
	/* just add 1 to end i.e. 7 threads one task is 4/7 to 5/7 */
	row_bounds[1] = ((this_thread->location + 1) * N) / this_thread->thread_count;

	/* From https://cse.buffalo.edu/faculty/miller/Courses/CSE633/Ortega-Fall-2012-CSE633.pdf slide 6*/
	for (i = row_bounds[0]; i < row_bounds[1]; i++) {
		for (j = 0; j < P; j++) {
			C[i][j] = 0;
			for (k = 0; k < M; k++){
				C[i][j] += A[i][k] * B[k][j];
			}
		}
	}
	return(EXIT_SUCCESS); /* why does compiler demand I return a value here? */
}

int C_C1_check() {
	int i, j;
	for (i = 0; i < N; i++) {
		for (j = 0; j < P; j++) {
			if(C1[i][j] != C[i][j]) {
				return 0;
			}
		}
	}
	return 1;
}

/* for debugging change N M P to smaller values and run this
this is how i determined why a cleaner loop in main wasnt working */
void print_Cs() {
	int i, j;
	for (i = 0; i < N; i++) {
		for (j = 0; j < P; j++) {
			printf("%d  ",C1[i][j]);
		}
		printf("\n");
	}
	printf("---------------------------\n");
	for (i = 0; i < N; i++) {
		for (j = 0; j < P; j++) {
			printf("%d  ",C[i][j]);
		}
		printf("\n");
	}
}