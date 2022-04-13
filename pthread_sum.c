#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>


pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;

typedef struct Pthread_Sum Pthread;
struct Pthread_Sum{
	double*	a;
	double sum;
	int 	N;
	int 	size;
	int		tid;
};

void *compute(void *arg) {
    int myStart, myEnd, myN, i;

    Pthread *pt = arg;

    myN = pt->N/pt->size;
    myStart = pt->tid*myN;
    myEnd = myStart + myN;
    if (pt->tid == (pt->size-1)) myEnd = pt->N;
    double mysum = 0.0;
    for (i=myStart; i<myEnd; i++)
      mysum += pt->a[i];
    pthread_mutex_lock(&mutex);
    pt->sum += mysum;
    pthread_mutex_unlock(&mutex);
    return (NULL);
}
int main(int argc, char **argv) {

	double *a=NULL, sum=0.0;
	int    N, size=0;
    long i;
    pthread_t *tid;
    Pthread *pt = 0;

    pt = (Pthread*) malloc(sizeof(Pthread)*size);

    if (argc != 3) {
       printf("Usage: %s <# of elements> <# of threads>\n",argv[0]);
       exit(-1);
    }
    N = atoi(argv[1]);
    size = atoi(argv[2]);
    tid = (pthread_t *)malloc(sizeof(pthread_t)*size);
    a = (double *)malloc(sizeof(double)*N);
    for (i=0; i<N; i++)
      a[i] = (double)(i + 1);
    for ( i = 0; i < size; i++){
      pthread_create(&tid[i], NULL, compute, (void *)&pt[i]);
    }
    for ( i = 0; i < size; i++){
      pthread_join(tid[i], NULL);
    }
    printf("The total is %g, it should be equal to %g\n",
           sum, ((double)N*(N+1))/2);

    return 0;
}
