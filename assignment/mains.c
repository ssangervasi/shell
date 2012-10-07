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

char** arrConcat(char** addto, char** addition, int* tosize, int* addsize);
char** tokenify(char* str);
char*** parseCommand(char* comlist);
int builtIn(char** command);
int changeMode(int seq, char* newmode);
int parallel(char *** cmd);
int runSeq(char ** command);
void freeCmd(char *** cmd);
int arrlen(char*** arr);

int main(int argc, char **argv) {
    char *prompt = ";) ";
    printf("%s", prompt);
    fflush(stdout);
    int sequential = 1;
	
 
    char buffer[1024] = "initialized";
    while (fgets(buffer, 1024, stdin) != NULL) {
        /* process current command line in buffer */
        /* just a hard-coded command here right now */
        //char *cmd[] = { "/bin/ls", "-ltr", ".", NULL };

		int buflen = 1023;
		int i = 0;
		while( i<buflen && buffer[i] != '#'){
			i++;
		}
		buffer[i] = '\0';
		buflen = i-1;
		
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
				sequential = runSeq(cmd[com]);
			}
		}	

    	if (sequential == 0 && cmd[com]!=NULL){
			sequential = parallel(cmd+com);	
    	}
		if(sequential == -2){
			exitUsage();
			exit(2);
		}
		freeCmd(cmd);
		free(cmd);
		if(sequential == -1){
			exit(2);
		}
    	printf("%s", prompt);
    	fflush(stdout);
	}
    printf("exited\n");
    return 0;

}

void exitUsage(){
	

void freeCmd(char *** cmd){
	int index = 0;
	while(cmd[index]!=NULL){
		free(cmd[index]);
		index++;
	}
	free(cmd[index]);
}

int runSeq(char ** command){
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
	return count-1;
}

int parallel(char *** cmd) {

  /* Check each element of commands by calling builtin. If builtin returns 0 then not a built in, equals 1 then run all processes --> do changemode, equals -1 run all processes ..> exit */

	char* modechange = "NOT THIS"; // command to change mode
	unsigned int modetest=0;
	int exit = 0;     // change to 1 if i get an exit command
	int index = 0;	// place in commands
	int rbuiltin;	// result of builtin
	int seq = 0; // will return seq
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

	int j = 0;
	if(builtinsf<numcommands){
		builtins[builtinsf] = -1;
	}
	int numpids = 1+numcommands-builtinsf;
	int* pids = malloc(numpids*sizeof(pid_t)); // process ids of children
	
	builtinsf = 0; //indexing through builtins
	index = 0;
	while (j <= numcommands) {
		if (j != builtins[builtinsf]) { // check if we want to run next command 
			pids[index] = fork();
			if (pids[index] == 0) {
			   /* in child */
				if (execv((cmd[j])[0], cmd[j]) < 0) {
			    	fprintf(stderr, "EXEC FAILED: %s\n", strerror(errno));
					pids[index]=-1;
					return -2;
			    }
			}else if(pids[index]<0){
				/* fork had an error; bail out */
				fprintf(stderr, "FORK FAILED: %s\n", strerror(errno));
			}
			index++;
		}
		else {
			builtinsf += 1;
		}
		j ++;
	}
	
	int pidindex = 0;
	for (;pidindex < numpids; pidindex++){
		 if (pids[pidindex] > 0) {
			/* in parent */
			int rstatus = 0;
			pid_t childp = waitpid(pids[pidindex],&rstatus,0); 
			printf("Parent got carcass of child process %d, return val %d\n", childp, rstatus);
		} 
	}
	if (1 == modetest) {
		seq = changeMode(seq, modechange);
	}

	if (exit == 1){
		seq = -1;
	}
	free(builtins);
	free(pids);
	return seq;
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

