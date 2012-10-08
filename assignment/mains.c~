/*
 * project 1 (shell) main.c template 
 *
 * Sebastian Sangervasi and Nickie VanMeter
 *
 */

/* you probably won't need any other header files for this project */
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
struct node {
	char name[128];
	pid_t pid;
	int status;
	struct node *next; 
};

long int minus(long int alpha, long int beta);
char** arrConcat(char** addto, char** addition, int* tosize, int* addsize);
char** tokenify(char* str);
char*** parseCommand(char* comlist);
int builtIn(char** command);
int changeMode(int seq, char* newmode);
int*** parallel(char *** cmd, struct node *paths);
int runSeq(char ** command, struct node *paths);
void freeCmd(char *** cmd);
int arrlen(char*** arr);
void exitUsage(struct rusage* usageBegin);
void list_clear(struct node *list);
void list_dump(struct node *list);
void list_insert(char *name, struct node **head);
void list_insert_ordered(char *name,  struct node **head);


int main(int argc, char **argv) {
	
	struct rusage usageBegin;
	getrusage(RUSAGE_SELF, &usageBegin);

	char *prompt = ";) ";
	printf("%s", prompt);
	fflush(stdout);
	int sequential = 1;

	struct node* tasks = NULL;

	struct node *paths = NULL;
	struct stat haspath;
	int canhas = stat("shell-config", &haspath);
	if(canhas==0){
		FILE * pathfile = fopen("shell-config", "r");		
		char* pathline = malloc(128*sizeof(char));
		while(fgets(pathline, 128, pathfile) != NULL){
			pathline[strlen(pathline)-1] = '/';
			list_insert(pathline, &paths);
		}
		free(pathline);
		fclose(pathfile);
	}

	char buffer[1024] = "initialized";
	int polloop = 1;
	while(polloop){
		struct pollfd pfd = {0, POLLIN}; //"In" events check
		int input = poll(&pfd, 1, 1000); //Input will be a 1=yes, 0=no input in the past second.
		if(input ==0){
			//run through the grand linked list of background processes.
			struct node *checkup = tasks;
			struct node *suture = tasks;
			int checkstatus = 0;
			while(checkup!=NULL){
				pid_t childp = waitpid((*checkup).pid, &checkstatus, WNOHANG);
				if(childp != 0){
					printf("Parent got carcass of child process %d, return val %d\n", childp, checkstatus);
					if(checkup == tasks){
						tasks = (*checkup).next;
						free(checkup);
						checkup = tasks;
					}else{
						(*suture).next = (*checkup).next;
						free(checkup);
						checkup = (*suture).next;
					}
				}else{
					if(checkup != tasks){
						suture = (*suture).next;
					}
					checkup = (*checkup).next;
				}
			}

		} else if(input<0){
			//probably need to free junk before exit.
			polloop = 0;
		}else{
			//run our fgets and do stuff with their commands.
	
			if(fgets(buffer, 1024, stdin) != NULL){
				int buflen = 1023;
				int i = 0;
				while( i<buflen && buffer[i] != '#'){
					i++;
				}
				buffer[i] = '\0';
				buflen = i-1;
				//printf("Buffer:%s", buffer);
				char *** cmd = parseCommand(buffer);
				//char* test = "a b c d e f g";
				//char*** cmd[2] = {tokenify(test), NULL}; 
				/*int a = 0;
				int b = 0;
				for(; cmd[a]!=NULL; a++){
					for(; (cmd[a])[b] != NULL; b++){
						printf("Command: %s\n", (cmd[a])[b]);
					}
					b=0;
				}*/
		

				int com = 0; //Will be used to increment through cmd
				int built;
				//Sequential Running of cmd:
				for(; sequential==1 && cmd[com] != NULL; com++){	
					built = builtIn(cmd[com]);
					if(built == 1){
						sequential = changeMode(sequential, (cmd[com])[1]);
					} else if(built == -1){
						sequential = -1;
					} else{
						sequential = runSeq(cmd[com], paths);
					}
				}	
				int*** newpros;
				if (sequential == 0 && cmd[com]!=NULL){					
					newpros = parallel(cmd+com, paths);	
					sequential = (newpros[0])[0][0];
					if(sequential!=-2){
						int index = newpros[1][0][0];
						for(;index > 1; index--){
							/*printf("Index points:%p, Index is:%d\n", newpros[index], index);
							printf("Index[1] points:%p\n", newpros[index][1]);
							printf("Index[1][0] points:%d\n", newpros[index][1][0]);*/
							if(newpros[index][1][0]>0){
								list_insert( (char*)newpros[index][0], &tasks);
								(*tasks).pid = *(newpros[index][1]);
								(*tasks).status = 1; //1 for running
							}
							free(newpros[index][1]);
							free(newpros[index][0]);
							free(newpros[index]);	
						}
						free(newpros);
					}
				}
				if(sequential == -2){
					free(newpros[0][0]);
					free(newpros[0]);
					free(newpros);
					exit(2);
				}
				freeCmd(cmd);
				free(cmd);
				if(sequential == -1){ //totally wrong. fix. -- fixed 
					if(tasks!=NULL){
						struct node *waiter = tasks;
						while(waiter!=NULL){
							int rstatus = 0;
							pid_t childp = waitpid((*waiter).pid, &rstatus,0); 
							printf("Parent got carcass of child process %d, return val %d\n", childp, rstatus);
							waiter = (*waiter).next;
						}
						while((*tasks).next!=NULL){
							waiter = (*tasks).next;
							free(tasks);
							tasks = waiter;
						}
					free(tasks);
					}
					exitUsage(&usageBegin);
					exit(2);
				}
				printf("%s", prompt);
				fflush(stdout);
			}
		}
	}
	exitUsage(&usageBegin);
	printf("exited\n");
	return 0;	
}


void exitUsage(struct rusage * usageBegin){
	struct rusage usageEnd;
	struct timeval userBeg, systemBeg, userEnd, systemEnd;
	if(0 == getrusage(RUSAGE_SELF, &usageEnd)){
		userBeg = (*usageBegin).ru_utime;
		systemBeg = (*usageBegin).ru_stime;
		userEnd = (usageEnd).ru_stime;
		systemEnd = (usageEnd).ru_stime;
		printf("TIME SPENT IN FANTASTIC USER MODE <3 %ld.%ld sec <3\n", minus(userBeg.tv_sec, userEnd.tv_sec), minus(userBeg.tv_usec, userEnd.tv_usec));
		printf("TIME SPENT IN WONDERFUL KERNEL MODE :$ %ld.%ld sec :$ \n", minus(systemBeg.tv_sec, systemEnd.tv_sec), minus(systemBeg.tv_usec, systemEnd.tv_usec));   
	}
	return;	
}

long int minus(long int alpha, long int beta){
	if(alpha>beta){
		return (alpha-beta);
	}
	return (beta-alpha);
}

void freeCmd(char *** cmd){
	int index = 0;
	while(cmd[index]!=NULL){
		free(cmd[index]);
		index++;
	}
	free(cmd[index]);
}

char* pathCheck(char* command, struct node *paths){
	command = strdup(command);
	struct stat ispath;
	int canhas = stat(command, &ispath);
	if(canhas==0){
		return command;
	}
	char *newcommand = malloc(sizeof(command)+(128*sizeof(char*))); //keep track of this shit -- kept track
	while(paths!=NULL){
		strcpy(newcommand, (*paths).name);
		newcommand = strcat(newcommand, command);
		//printf("new com: -%s-\n", newcommand);
		canhas = stat(newcommand, &ispath);
		if(canhas==0){
			free(command);
			return newcommand;
		}
		newcommand[0] = '0';
		paths = (*paths).next;
	}
	free(newcommand);
	return NULL;
}

int runSeq(char ** command, struct node *paths){
	//stat check here
	char* goodCommand = pathCheck(command[0], paths);
	if(goodCommand == NULL){
		printf("OH NO! COULDN'T EXECUTE THAT COMMAND: %s\n", command[0]);
		return 1;
	}else{
		command[0] = goodCommand;
	}
	pid_t p = fork();
	if (p == 0) {
		/* in child */
		if (execv(command[0], command) < 0) {
			fprintf(stderr, "OH NO! COULDN'T EXECUTE THAT COMMAND: %s\n", strerror(errno));
			return -2;
		}
	} else if (p > 0) {
		/* in parent */
		int rstatus = 0;
		pid_t childp = wait(&rstatus);
		/* for this simple starter code, the only child process we should
	   "wait" for is the one we just spun off, so check that we got the
	   same process id */ 
		assert(p == childp);
		printf("CHILD PROCESS (%d) DID AN EXEC AND GOT PASSED TO IT'S PARENT. Return val %d\n", childp, rstatus);
	} else {
	 	/* fork had an error; bail out */
		fprintf(stderr, "OH NO! WE HAD A FAILURE WHEN FORKING TO A CHILD: %s\n", strerror(errno));
	}
	
	return 1;
}

int arrlen(char*** arr){
	int count = 0;
	while(arr[count] != NULL){
		count++;
	}
	return count;
}

int*** parallel(char *** cmd, struct node *paths) {

  /* Check each element of commands by calling builtin. If builtin returns 0 then not a built in, equals 1 then run all processes --> do changemode, equals -1 run all processes ..> exit */
	int*** processes;
	char* modechange = "NOT THIS"; // command to change mode
	unsigned int modetest=0;
	int exit = 0; // change to 1 if i get an exit command
	int index = 0;	// place in commands
	int rbuiltin;	// result of builtin
	int ** seqholder = malloc(sizeof(int*)); // will return seq
	int * seq = malloc(sizeof(int));
	*seq = 0;
	seqholder[0] = seq;
	int numcommands = arrlen(cmd);
	int* builtins = malloc(numcommands*sizeof(int)); //will not want to exec these
	builtins[0] = -1;
	
	int builtinsf = 0;
	for(; cmd[index] != NULL; index++) {
		rbuiltin = builtIn(cmd[index]);
		if (rbuiltin == 1){
			modechange = (cmd[index])[1]; //keep track of where mode command was
			modetest = 1;
			builtins[builtinsf] = index;
			builtinsf++; 
		}
		else if (rbuiltin == -1) {
			exit = 1; // record that an exit request was made
			builtins[builtinsf] = index;
			builtinsf++; 
		}
	}	

	
	if(builtinsf<numcommands){
		builtins[builtinsf] = -1;
	}
	int numpids = numcommands-builtinsf;
	//int* pids = malloc(numpids*sizeof(pid_t)); // process ids of children
	processes = malloc((2+numpids)*sizeof(int**));
	processes[0] = seqholder;
	int** numpro = malloc(sizeof(int*));
	numpro[0] = malloc(sizeof(int));
	numpro[0][0] = numpids;
	processes[1] = numpro;
	builtinsf = 0; //indexing through builtins
	index = 2;
	char* goodCommand;
	int j = 0;
	while (j < numcommands) {
		if (j != builtins[builtinsf]) { // check if we want to run next command
			//stat check here 
			//printf("\ncmd[j][0]:%p\n", cmd[j]);
			goodCommand = pathCheck((cmd[j])[0], paths);
			if(goodCommand == NULL){
				printf("OH NO! COULDN'T EXECUTE THAT COMMAND: %s", (cmd[j])[0]);
				
			}else{
				(cmd[j])[0] = goodCommand;		
				//pids[index] = fork();
				processes[index] = malloc(2*sizeof(int*));
				(processes[index])[0] = malloc(strlen(goodCommand)*sizeof(int));
				(processes[index])[0] = (int*)goodCommand;

				(processes[index])[1] = malloc(sizeof(int));
				//(processes[index])[1][0] = malloc(sizeof(int));
				(processes[index])[1][0] = fork();
				if ((processes[index])[1][0] == 0) {
				   /* in child */
					if (execv((cmd[j])[0], cmd[j]) < 0) {
					fprintf(stderr, "EXEC FAILED: %s\n", strerror(errno));
						(processes[index])[1][0]=-1;
						int*** failure = malloc(sizeof(int**));
						int** seqfail = malloc(sizeof(int*));
						int* faildigit = malloc(sizeof(int));
						*faildigit = -2;
						seqfail[0] = faildigit;
						failure[0] = seqfail;
						return failure;
				}
				}else if((processes[index])[1][0]<0){
					/* fork had an error; bail out */
					fprintf(stderr, "FORK FAILED: %s\n", strerror(errno));
				}
				index++;
			}
		}
		else {
			builtinsf += 1;
		}
		j++;
	}

	if (1 == modetest) {
		(processes[0])[0][0] = changeMode(0, modechange); //should be same as: seq[0] = changeMode(0, modechange);
	}

	if (exit == 1){
		(processes[0])[0][0] = -1;
	}
	free(builtins);
	return processes;
}

int builtIn(char** command){
	int built = 0;
	char * mode = "mode";
	char * exit = "exit";
	if(0 == strcmp(command[0], mode)){
		built = 1;
	}else if(0 == strcmp(command[0], exit)){
		built = -1;
	}
	return built;
}

int changeMode(int sequential, char* newmode){
	char* mode[] = {"parallel", "sequential", "p","s"};
	int seq = sequential;
	if(newmode == NULL){
		printf("\nCURRENT TASK MODE: %s\n", mode[seq]); 
	} else if(0 == strcmp(newmode, mode[1]) || 0 == strcmp(newmode, mode[3])){
		seq = 1;
	} else if (0 == strcmp(newmode, mode[0]) || 0 == strcmp(newmode, mode[2])){
		seq = 0;
	} else{
		printf("EXCUSE ME, UH, MODE COMMAND NOT RECOGNIZED.\n IF YOU WANT TO CHANGE TASK MODE, ENTER EITHER:\n\tmode sequential\n\tmode parallel\n");
		printf("\nCURRENT TASK MODE, just FYI, dude: %s\n", mode[seq]);
	}
	return seq;
}




char*** parseCommand(char* comlist)
{
	
	int i = 0;
	int comlen = strlen(comlist);
	int* semindex = malloc(sizeof(int)*(comlen+1));
	int john = 1;  
	semindex[0] = -1;
	int lastcom = 0; //number of last valid semicolon (length of semindex)
	int valid = 0;
	for(; i < comlen; i++){
		if(1==valid && comlist[i] == ';'){
			semindex[john] = i;
			john++;
			valid = 0;
			//printf("found semi \n");
			
		} else if(!isspace(comlist[i]) && comlist[i]!=';'){
			lastcom = john;
			valid = 1;
		}
		//printf("Ran semicolon find: %d of %d\n", i, john);
	}
	if(john == 1){
		semindex[john] = comlen;
	}
	char *** parsed = malloc(sizeof(char**)*(lastcom+1)); //with a reminder to set the last element to NULL
	i = 0;
	int j = 0;
	char* totoken;
	char** tokened;
	for(; i < lastcom; i++){
		totoken = strndup(&comlist[semindex[i]+1], sizeof(char)*(semindex[i+1]-semindex[i]));		
		tokened = tokenify(totoken);
		if(tokened[0] != NULL){
			parsed[j] = tokened;
			j++;
		} else{
			free(tokened);
		}
		//printf("TOTOKEN IS -%s-\n", totoken);
		/*int blah = 0;
		while((parsed[i])[blah] != NULL){
			printf("TOKENIFIED -%s-\n", (parsed[i])[blah]);
			blah++;
		}*/
		free(totoken);
		//printf("Ran tokenify: %d of %d\n", i, john);
	}	
	free(semindex);
	parsed[lastcom] = NULL; 
	
	return parsed;
}





/* 	Array Concatonation is a helper function I wrote that is used in "tokenify"  
	to make the combination of the total array and a buffer array (of strings).
*/
char** arrConcat(char** addto, char** addition, int* tosize, int* addsize)
{
	//printf("one\n");
	int i=0;
	int j=0;
	int totsize = (*tosize)+(*addsize)+1;
	//int memsize;
	char** newarr = malloc(totsize*sizeof(char*));
	while(i<*tosize){
		//memsize = strlen(addto[i])*sizeof(char);
		newarr[i] = strdup(addto[i]);
		free(addto[i]);
		i++; 
	}
	free(addto[*tosize]);
	while(j<*addsize){
		//memsize = strlen(addition[j])*sizeof(char);
		newarr[i] = strdup(addition[j]);
		free(addition[j]);
		i++;
		j++;
	}
	/*while(j<6){
		free(addition[j]);
		j++;
	}*/
	free(addto);
	newarr[totsize-1] = NULL;
	//printf("two\n");
	return newarr; 
}

char** tokenify(char* str)
{
	int i = 0;
	const int slen = strlen(str);
	int buffsize =6; // I'm using this variable so that you can set how big the buffer is during this process not including the concluding NULL element.
	int totalsize = 0; //Refers to the number of places in totalarray that are not the concluding NULL element. 
	
	char** totalarray = malloc(sizeof(char*));
	char** buffarray = malloc(sizeof(char*)*(buffsize+1));
	int inbuff = 0;
	
	char* begin = NULL;
	int gotill = -1;
	int fullword = 0;
	int upto = 0;

	totalarray[0] = NULL;

	for(; i<=slen; i++){
		if(!isspace(str[i]) && str[i]!=';'){
			if(begin == NULL){
				begin = (str+i);
			}			
			gotill++;
						
		}
		else{
			if(begin != NULL){
				gotill++;
				fullword = 1;
			}
			
			if(begin != NULL && 1 == fullword){
				
				buffarray[inbuff] = malloc(sizeof(char)*(gotill+1));
				for(; upto<gotill; upto++){
					(buffarray[inbuff])[upto]= *(begin+upto);
				}
				(buffarray[inbuff])[gotill] = '\0';
				
				inbuff++;
				gotill=-1;
				begin = NULL;
				upto = 0; 
				fullword = 0;	
			
				if(buffsize==inbuff){					
					totalarray = arrConcat(totalarray, buffarray, &totalsize, &buffsize);
					totalsize += (buffsize);
					inbuff = 0;						
					free(buffarray);	
					buffarray = malloc(sizeof(char*)*(buffsize+1));
					
				}
			}
			
		}
	}
	if(inbuff>0){		
		totalarray = arrConcat(totalarray, buffarray, &totalsize, &inbuff);		
	}
	free(buffarray); //I hope I have taken care of all memory leaks...
	/*int blah = 0;	
	for(; totalarray[blah]!=NULL; blah++){
		printf("token:%s\n", totalarray[blah]);
	}
	*/
	//printf("has null\n");
	return totalarray;
}

/* This is linked list stuff. This should be handy*/


void list_insert_ordered(char *name,  struct node **head) 
{
	struct node *newnode = malloc(sizeof(struct node));
	strncpy( (*newnode).name, name, 127);
	if(NULL == *head){
		(*newnode).next = NULL;
		*head = newnode;
		return;
	}
	struct node *pre = NULL;
	struct node *post = *head;
	while(NULL != post && strcasecmp((*post).name, name) < 1){
		pre = post;
		post = (*post).next;
	}
	(*newnode).next = post;
	if(NULL == pre){
		*head = newnode;
		return;	
	}
	(*pre).next = newnode;
	return;  
}


void list_insert(char *name, struct node **head) {
	struct node *newnode = malloc(sizeof(struct node));
	strncpy(newnode->name, name, 127);

	newnode->next = *head;
	*head = newnode;
}

void list_dump(struct node *list) {
	int i = 0;
	printf("\n\nDumping list\n");
	while (list != NULL) {
		printf("%d: %s\n", i++, list->name);
		list = list->next;
	}
}

void list_clear(struct node *list) {
	while (list != NULL) {
		struct node *tmp = list;
		list = list->next;
		free(tmp);
	}
}
  

