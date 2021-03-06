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
int changeMode(int seq, char* newmode);
int parallel(char *** cmd, int seq)

int main(int argc, char **argv) {
    char *prompt = ";) ";
    printf("%s", prompt);
    fflush(stdout);
    unsigned int sequential = 1;
	
 
    char buffer[1024];
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
		/*char* test = "a b c d e f g";
		char*** cmd[2] = {tokenify(test), NULL}; 
		int a = 0;
		int b = 0;
		for(; cmd[a]!=NULL; a++){
			for(; (cmd[a])[b] != NULL; b++){
				printf("Command: %s\n", (cmd[a])[b]);
				//printf("Size of Token: %d\n", strlen((cmd[a])[b]));
			}
			b=0;
		}
		*/

		int com = 0; //Will be used to increment through cmd
		//Sequential Running of cmd:
		for(; sequential==1 && cmd[com] != NULL; com++){	
			if((cmd[com])[0] == "mode"){
				sequential = changeMode(sequential, (cmd[com])[1]);
			}
			
    		pid_t p = fork();
    		if (p == 0) {
    	    	    /* in child */
    		        if (execv((cmd[com])[0], cmd[com]) < 0) {
    	    	        fprintf(stderr, "execv failed: %s\n", strerror(errno));
    	    	    }
	
		    } else if (p > 0) {
		            /* in parent */
		            int rstatus = 0;
		            pid_t childp = wait(&rstatus);
		
		            /* for this simple starter code, the only child process we should
		               "wait" for is the one we just spun off, so check that we got the
		               same process id */ 
		            assert(p == childp);
		
		            printf("Parent got carcass of child process %d, return val %d\n", childp, rstatus);
		    } else {
		            /* fork had an error; bail out */
		            fprintf(stderr, "fork failed: %s\n", strerror(errno));
		    }
	
    	}
		
    	printf("%s", prompt);
    	fflush(stdout);
	}
    printf("exited\n");
    return 0;
}	

int changeMode(int seq, char* newmode){
	char** mode = {"parallel\n", "sequential"};
	if(newmode == NULL){
		printf("\nCURRENT TASK MODE: %s\n", mode[seq]); 
	} else if(newmode == "sequential"){
		seq = 1;
	} else if (newmode == "parallel"){
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
	int john = 1; //number of last valid semicolon (length of semindex) 
	semindex[0] = -1;
	int lastcom = 0;
	printf("AHAHHA\n");
	for(; i < comlen; i++){
		if(comlist[i] == ';'){
			semindex[john] = i;
			john++;
			printf("found semi \n");
			
		} else if(!isspace(comlist[i])){
			lastcom = john;
		}
		//printf("Ran semicolon find: %d of %d\n", i, john);
	}
	char *** parsed = malloc(sizeof(char**)*(john+1)); //with a reminder to set the last element to NULL
	i = 0;
	char* totoken;
	for(; i < john; i++){
		totoken = strndup(&comlist[semindex[i]+1], sizeof(char)*(1+semindex[i+1]-semindex[i]));
		parsed[i] = tokenify(totoken);
		free(totoken);
		printf("Ran tokenify: %d of %d\n", i, john);
	}	
	free(semindex);
	parsed[john+1] = NULL; //changed this to +1 bc we added -1 as element 0
	
	return parsed;
}

int parallel(char *** cmd) {

  /* Check each element of commands by calling builtin. If builtin returns 0 then not a built in, equals 1 then run all processes --> do changemode, equals -1 run all processes ..> exit */

	char** modechange; // command to change mode
	int exit = 0;     // change to 1 if i get an exit command
	int index = 0;	// place in commands
	int rbuiltin;	// result of builtin
	int* index builtins = malloc((sizeof(cmd)/sizeof(char ***))*sizeof(int)) //will not want to exec these
	int seq = 0; // will return seq
	int numpids = sizeof(cmd)/sizeof(char***)
	int* pids = malloc((numpids)*sizeof(pid_t)); // process ids of children
	
	while(cmd[index] != NULL) {
		rbuiltin = builtIn(cmd[index];
		if (rbuiltin == 1){
			modechange = cmd[index]; //keep track of where mode command was
		}
		else if (rbuiltin == -1) {
			exit == 1; // record that an exit request was made
		}
	}	
	int j = 0;
	int builtinsf = 0; //indexing through builtins	
	while (j < index) {
		if (j != builtins[builtinsf]) { // check if we want to run next command 
			pids[j] = fork();
			if (pids[j] == 0) {
			   /* in child */
				if (execv((cmd[j])[0], cmd[j]) < 0) {
			    	fprintf(stderr, "execv failed: %s\n", strerror(errno));
			    }
			}
		}
		else {
			j ++;
			builtinsf += 1;
		}
	}
	int pidindices =0;
	for (;pidindices < numpids; pidindices++){
		 if (pids[j] > 0) {
					    /* in parent */
				        int rstatus = 0;
				        pid_t childp = waitpid(pids[j],&rstatus,0); 
				        printf("Parent got carcass of child process %d, return val %d\n", childp, rstatus);
		} else {
				        /* fork had an error; bail out */
				        fprintf(stderr, "fork failed: %s\n", strerror(errno));
				}
	}
	if (exit = 1){
		exit();
	}
	if (modechange != NULL) {
		seq = changeMode(seq, modechange);
	}
	
	return seq;
}

/* 	Array Concatonation is a helper function I wrote that is used in "tokenify"  
	to make the combination of the total array and a buffer array (of strings).
*/
char** arrConcat(char** addto, char** addition, int* tosize, int* addsize)
{
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
		
	while(j<*addsize){
		//memsize = strlen(addition[j])*sizeof(char);
		newarr[i] = strdup(addition[j]);
		free(addition[j]);
		i++;
		j++;
	}
	free(addto);
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
	}*/
	return totalarray;
}

