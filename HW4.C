#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/queue.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <dirent.h>
#include <stdio.h>
#include <inttypes.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include <string.h>
#include <sys/wait.h>

#ifndef MAX_PROC
#define MAX_PROC 32767
#endif

#ifndef RESOLUTION
#define RESOLUTION 1
#endif

void submit(char **argv);
void showJobs(char **argv);
void submitHistory(char **argv);

typedef struct _queue queue;
struct _queue {
	int size; /* maximum size of the queue */
	void **buffer; /* queue buffer */
	int start; /* index to the start of the queue */
	int end; /* index to the end of the queue */
	int count; /* no. of elements in the queue */
};
typedef void (*eptr)(void *arg, void *data);

queue *queue_init(int n);
int queue_insert(queue *q, void* item);
void* queue_delete(queue *q);
void queue_each(queue *q, eptr func, void *data);
void queue_destroy(queue *q);

typedef struct Compare compare;
struct Compare {
	pid_t tid;
	int *running;
	int status;
};

typedef struct Jobs Job;
struct Jobs {
	int jobId;
	char** argv;
	int status;
	int jobEnd;
	pid_t pid;
	unsigned long start;
	unsigned long stop;
};

queue *jobs = 0;
int cnt = 0;
pid_t pid2 = 0;
pid_t schedule = 0;
int presentJobId = 0;



enum JOB_STATUS {
	WAITING = 0, RUNNING = 1, COMPLETED = 2
};

const char* JOB_STATUS_str[] = { "WAITING", "RUNNING", "COMPLETED" };

#define JOB_STATUS_STRING(js) JOB_STATUS_str[js]

int main(int argc, char **argv) {

	int n, i = 0, count = 2;

	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "submit") == 0) {
			submit(argv);
		}
		if (strcmp(argv[i], "showjobs") == 0) {
			showJobs(argv);
		}
		if (strcmp(argv[i], "submithistory") == 0) {
			submitHistory(argv);
		}

		jobs = queue_init(MAX_PROC);
		while ((n = getopt(argc, argv, "p:")) != -1) {
			switch (n) {
			case 'p':
				count = atoi(optarg);
				break;
				return 0;
			}
		}
	}
}
	/* create the queue data structure and initialize it */
	queue *queue_init(int n) {
		queue *q = (queue *) malloc(sizeof(queue));
		q->size = n;
		q->buffer = malloc(sizeof(int) * n);
		q->start = 0;
		q->end = 0;
		q->count = 0;

		return q;
	}

	/* insert an item into the queue, update the pointers and count, and
	 *  return the no. of items in the queue (-1 if queue is null or full) */
	int queue_insert(queue *q, void* item) {
		if ((q == NULL) || (q->count == q->size))
		return -1;

		q->buffer[q->end % q->size] = item;
		q->end = (q->end + 1) % q->size;
		q->count++;

		return q->count;
	}

	/* delete an item from the queue, update the pointers and count, and
	 *  return the item deleted (-1 if queue is null or empty) */
	void* queue_delete(queue *q) {
		if ((q == NULL) || (q->count == 0))
		return 0;

		void* x = q->buffer + q->start;
		q->start = (q->start + 1) % q->size;
		q->count--;

		return x;
	}

	/* display the contents of the queue data structure */
	void queue_each(queue *q, eptr func, void *data) {
		if (q != NULL && q->count != 0) {
			for (int i = 0; i < q->count; i++)
			func(q->buffer + ((q->start + i) % q->size), data);
		}
	}

	/* delete the queue data structure */
	void queue_destroy(queue *q) {
		free(q->buffer);
		free(q);
	}

	void submit(char **argv) {
		Job *j;

		if ((j = malloc(sizeof(Job)))) {
			memset(j, 0, sizeof(Job));
			j->argv = argv;
			j->status = WAITING;
			j->jobId = presentJobId++;
		}

		queue_insert(jobs, e);
	}

	void showJobs(char **argv) {
		queue_each(jobs, showJobs_helper, 0);
	}

	void showJobs_helper(void* elem, void *data) {
		Job *j = elem;

		if (j->status != COMPLETED) {
			printf("%d\t\t", j->jobId);
		}
	}

	void submitHistory(char **argv) {
		queue_each(jobs, submitHistory_helper, 0);
	}

	void submitHistory_helper(void *elem, void *data) {
		Job *j = elem;
		char buffer[256];
		time_t timeInfo;

		if (j->status == COMPLETED) {
			printf("%d\t\t", j->jobId);

			timeInfo = localtime(&-j->start);
			printf("Start Time :", asctime(timeInfo));

			timeInfo = localtime(&j->stop);
			printf("Stop Time :", asctime(timeInfo));

			printf("%s\r\n", j->status ? "Success" : "Failed");
		}
	}
