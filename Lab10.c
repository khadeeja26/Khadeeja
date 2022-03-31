#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
//#include <sys/wait.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

void createarray(char *buf, char **array) {
	int i, count, len;
	len = strlen(buf);
	buf[len - 1] = '\0';
	for (i = 0, array[0] = &buf[0], count = 1; i < len; i++) {
		if (buf[i] == ' ') {
			buf[i] = '\0';
			array[count++] = &buf[i + 1];
		}
	}
	array[count] = (char *) NULL;
}
int main(int argc, char **argv) {
	pid_t pid;
	int status;
	char line[BUFSIZ], buf[BUFSIZ], *args[BUFSIZ];
	char out[100];
	char err[100];


	time_t t1, t2;
	if (argc < 2) {
		printf("Usage: %s <commands file>\n", argv[0]);
		exit(-1);
	}
	FILE *fp1 = fopen(argv[1], "r");
	if (fp1 == NULL ) {
		printf("Error opening file %s for reading\n", argv[1]);
		exit(-1);
	}
	FILE *fp2 = fopen("output.log", "w");
	if (fp2 == NULL ) {
		printf("Error opening file output.log for writing\n");
		exit(-1);
	}
	while (fgets(line, BUFSIZ, fp1) != NULL ) {
		strcpy(buf, line);
		createarray(line, args);
#ifdef DEBUG
		int i;
		printf("%s", buf);
		for (i = 0; args[i] != NULL; i++)
		printf("[%s] ", args[i]);
		printf("\n");
#endif
		time(&t1);
		pid = fork();
		if (pid == 0) {

		    sprintf(out, "./%d.out", getpid());
		    sprintf(err, "./%d.err", getpid());

			int fd1 = open(out, O_CREAT | O_WRONLY, 0600);
			int fd2 = open(err, O_CREAT | O_WRONLY, 0600);

			dup2(fd1, 1);
			dup2(fd2, 2);
			printf("This message will be printed in the file and not on terminal");

			close(fd1);
			close(fd2);

			execvp(args[0], args);
			perror("exec");
			exit(-1);
		} else if (pid > 0) {
			printf("Child started at %s", ctime(&t1));
			printf("Wait for the child process to terminate\n");
			wait(&status);
			time(&t2);
			printf("Child ended at %s", ctime(&t2));
			if (WIFEXITED(status)) {
				printf("Child process exited with status = %d\n",
						WEXITSTATUS(status));
			} else {
				printf("Child process did not terminate normally!\n");

			}
			buf[strlen(buf) - 1] = '\t';
			strcat(buf, ctime(&t1));
			buf[strlen(buf) - 1] = '\t';
			strcat(buf, ctime(&t2));
			fprintf(fp2, "%s", buf);
			fflush(fp2);
		} else {
			perror("fork");
			exit(EXIT_FAILURE);
		}
	}
	fclose(fp1);
	fclose(fp2);
	printf("[%ld]: Exiting main program .....\n", (long) getpid());
	return 0;
}
