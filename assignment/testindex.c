//Testing the indexing of a char***

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <poll.h>
#include <signal.h>
#include <errno.h>
#include <assert.h>



int main(int argc, char **argv){
	char* command = "this is a command";
	char *newcommand = malloc(sizeof(command)+(128*sizeof(char*)));
	*newcommand = command;
	printf("-%s-", newcommand);


	char *** test = malloc(sizeof(char**)*5);
	char* first[] = {"this", "is", "first"};
	char* second[] = {"this", "is", "second"};
	char* third[] = {"this", "is", "third"};
	char* fourth[] = {"this", "is", "fourth"};
	test[0] = first;
	test[1] = second;
	test[2] = third;
	test[3] = fourth;
	test[4] = NULL;

	int index = 0;
	int word = 0;
	for(; test[index]!=NULL; index++){
		word = 0;
		for(; word < 3 ; word++){
			printf(" %s", (test[index])[word]);
		}
		printf(".\n");
	}
	char *** tester = (test+1);
	printf("Now tester\n");
	index = 0;
	for(; tester[index]!=NULL; index++){
		word = 0;
		for(; word < 3 ; word++){
			printf(" %s", (tester[index])[word]);
		}
		printf(".\n");
	}	

	return 0;
}
