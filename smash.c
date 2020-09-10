#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

char error_message[30] = "An error has occurred\n";

char* accessCheck(char * cmd, char* path) {
	char* temp = strdup(path);
	
	char* temp2 = strdup(cmd);
	cmd = strsep(&temp2, " \t\f\n\r\v"); //take just the program to execute
	temp2 = malloc(1000);
	char* token = malloc(1000);
	while((token = strsep(&temp, " "))) {
			
			if(strcmp(token, "")) {
				strcpy(temp2, token);
			strcat(temp2, "/");
			strcat(temp2, cmd);//append prog name
						
			if (access(temp2, X_OK) == 0) {
			return temp2;
		}
		}
	}
	return NULL;
	
}

int exitCheck (char * cmd) {
	if (cmd == NULL)
		return 0;
	char * cmdTok = "";
	char * cmdDup = strdup(cmd);
	while (cmdTok != NULL && strcmp(cmdTok, "") == 0) { //look thorugh the rest of the command
		cmdTok = strsep(&cmdDup, " \t\f\n\r\v");
	}
	if (cmdTok == NULL) { //if it stopped somewhere in the middle, there are extra args
		return 0;
	}
	else {
		return 1;
	}
}

int cdCheck (char * cmd) {
	if (cmd == NULL)
		return 1;
	char * cmdTok = "";
	char * cmdDup = strdup(cmd);
	while (cmdTok != NULL && strcmp(cmdTok, "") == 0) { //get to the next args
	cmdTok = strsep(&cmdDup, " \t\f\n\r\v");
	}
	if (cmdTok == NULL) { //if there is no next arg, error
		return 1;
	}
	else { //cmdTok now has the next arg
		char * temp = malloc(1000);
		while (temp != NULL && strcmp(temp, "") == 0) { //if this stops in the middle, that means there are extra args
			temp = strsep(&cmdDup, " \t\f\n\r\v");
		}	
		if (temp != NULL) {
			return 1;
		}
		else {
			return 0;
		}
	}
}

int pathCheck (char * cmd) {
	if (cmd == NULL)
		return 1;
	char * cmdTok = "";
	char * cmdDup = strdup(cmd); 
	while (cmdTok != NULL && strcmp(cmdTok, "") == 0) {
		cmdTok = strsep(&cmdDup, " \t\f\n\r\v");
	}
	//cmdTok now has the next arg
	if (cmdTok == NULL) { //if there is no next arg
		return 1;
	}
	if (strcmp(cmdTok, "add") == 0) {
		cmdTok = "";
		while (cmdTok != NULL && strcmp(cmdTok, "") == 0) {
			cmdTok = strsep(&cmdDup, " \t\f\n\r\v");
		}
		//cmdTok now has the next arg
		if (cmdTok == NULL) { //if there is no next arg
			return 1;
		}
		else {
			char * temp = malloc(1000);
			while (temp != NULL && strcmp(temp, "") == 0) { //checking if there are any extra args
				temp = strsep(&cmdDup, " \t\f\n\r\v");
			}
			if (temp != NULL) {
				return 1;
			}
		}
						
	return 0;				
	}
	else if (strcmp(cmdTok, "remove") == 0) {
		cmdTok = "";
		while (cmdTok != NULL && strcmp(cmdTok, "") == 0) {
			cmdTok = strsep(&cmdDup, " \t\f\n\r\v");
		}
		//cmdTok now has the next arg
		if (cmdTok == NULL) { //if there is no next arg
			return 1;
		}
		else {
			char * temp = malloc(1000); 
			while (temp != NULL && strcmp(temp, "") == 0) { //checking for extra args
				temp = strsep(&cmdDup, " \t\f\n\r\v");
			}
			if (temp != NULL) {
				return 1;
			}
		}
		return 0;
	}
	else if (strcmp (cmdTok, "clear") == 0) {
		cmdTok = "";
		while (cmdTok != NULL && strcmp(cmdTok, "") == 0) { //checking for extra args
			cmdTok = strsep(&cmdDup, " \t\f\n\r\v");
		}
		
		if (cmdTok != NULL) {
			return 1;
		}
		return 0;
	}
	else {
		return 1; //none of the args matched
	}
}

//Checks if str is juts a bunch of spaces/""
int emptyCheck (char * str) {
	if (str == NULL)
		return 1;
	char * tok = malloc (1000);
	char * dup = strdup(str);
	while ((tok = strsep(&dup, " \t\f\n\r\v"))) {
		if (strcmp(tok, "")) {
			return 0;
		}
	}
	return 1;
}

int builtInCheck(char* cmd, char* path) {
	if (emptyCheck(cmd)) {
		return 0;
	}
	else {
		char* cmdTok = malloc(1000);
		char* cmdDup = strdup(cmd);
		cmdTok = "";
		while(strcmp(cmdTok, "") == 0) {
			cmdTok = strsep(&cmdDup, " \t\f\n\r\v");
		}
		char *prog = cmdTok;
		if (strcmp(prog, "exit")==0) {
			return exitCheck(cmdDup);
		}
		else if (strcmp(prog, "cd") == 0) {
			return cdCheck(cmdDup);
		}
		else if (strcmp(prog, "path") == 0) {
			
			return pathCheck(cmdDup);
		}
		else {
			if (accessCheck(cmdTok, path)) {
				return 0;
			}
			else return 1;
		}
		
	}
}

int redirectCheck(char* cmd, char *path) {
	char* cmdTok = malloc(1000);
	char* cmdDup = strdup(cmd);
	if (strstr(cmd, ">")==NULL) {
		return builtInCheck(cmd, path);
	}
	cmdTok = strsep(&cmdDup, ">");
	
	if (emptyCheck(cmdTok)) {
		
		return 1;
	}
	if (cmdDup == NULL) {
		
		return 1;
	}
	char * prog = cmdTok;
	cmdTok = strsep(&cmdDup, ">");
	
	if (emptyCheck(cmdTok)) {
		
		return 1;
	}
	//trim cmdTok
	char* temp = strdup(cmdTok);
	cmdTok = "";
	while (cmdTok != NULL && strcmp(cmdTok, "") == 0) {
		cmdTok = strsep(&temp, " \t\f\n\r\v"); 
	}
	if (!emptyCheck(temp)) {
		
		return 1;
	}
	
	
		if (accessCheck(prog, path))
			return 0;
		else 
			return 1;
	
	
	
	}
	


int parallelCheck(char* cmd, char* path) {
	char* cmdTok = malloc(1000);
	char* cmdDup = strdup(cmd);
	while ((cmdTok = strsep(&cmdDup, "&"))) {
		if (!emptyCheck(cmdTok)){
			
			if (redirectCheck(cmdTok, path) == 1) {
				
				return 1;
			}
		}
	}
	return 0;
}

int multipleCheck (char* cmd, char * path, char* commands[]) {
	int c = 0;
	char* cmdTok = malloc(1000);
	char* cmdDup = strdup(cmd);
	while ((cmdTok = strsep(&cmdDup, ";"))) {
		
		if (!emptyCheck(cmdTok)) {
			
			commands[c++] = cmdTok;
			if (parallelCheck(cmdTok, path) == 1) {
				
				return 1;
			}
		}
	}
	return 0;
}

void exitExec() {
	exit(0);
}

void cdExec(char* cmd) { //cmd has     args
	char* cmdTok = malloc(1000);
	char* cmdDup = strdup(cmd);
	cmdTok = "";
	while (strcmp(cmdTok, "") == 0) {
		cmdTok = strsep(&cmdDup, " \t\f\n\r\v");
	}
	if (chdir(cmdTok) == -1) {
		
		write(STDERR_FILENO, error_message, strlen(error_message));	
	}

	
}

void pathExec(char* cmd, char* path) {
	char * cmdTok = "";
	char * cmdDup = strdup(cmd); 
	while (cmdTok != NULL && strcmp(cmdTok, "") == 0) {
		cmdTok = strsep(&cmdDup, " \t\f\n\r\v");
	}
	//cmdTok now has the next arg
	if (strcmp(cmdTok, "add")==0) {
		cmdTok = "";
		while (cmdTok != NULL && strcmp(cmdTok, "") == 0) {
			cmdTok = strsep(&cmdDup, " \t\f\n\r\v");
		}
		//cmdTok has last arg
		char * space = malloc(1);
		strcpy(space, " ");
		strcat(cmdTok, space);
		strcat(space, cmdTok);
		strcat(space, path);
		strcpy(path, space);
	}
	else if (strcmp(cmdTok, "remove") == 0) {
		cmdTok = "";
		while (cmdTok != NULL && strcmp(cmdTok, "") == 0) {
			cmdTok = strsep(&cmdDup, " \t\f\n\r\v");
		}
		char * space = malloc(1000);
		strcpy(space, " ");
		strcat(cmdTok, space);
		strcat(space, cmdTok);
		strcpy(cmdTok, space);
		char * temp = strstr(path, cmdTok);
		char * right = temp + strlen(cmdTok);
		char * temp2 = malloc (1000);
		strncpy(temp2, path, strlen(path)-strlen(temp));
		strcat(temp2, right);
		strcpy(path, temp2);
		strcat(path, " ");
			
		
	}
	else if (strcmp (cmdTok, "clear") == 0) {
		strcpy(path, "");
	}
	
}

void builtInExec (char* cmd, char* path) { //cmd has the spaces
	
	
	if (cmd == NULL || emptyCheck(cmd)){
		
		return ;
	}
	//want to get first word
	
	char* cmdTok = malloc(1000);
	char* cmdDup = strdup(cmd);
	cmdTok = "";
	while (cmdTok != NULL && strcmp(cmdTok, "") == 0) {
		cmdTok = strsep(&cmdDup, " \t\f\n\r\v");
	}
	
	//cmdTok has first word //cmdDup has       args
	if (strcmp(cmdTok, "exit") == 0) {
		exit(0);
	}
	else if (strcmp(cmdTok, "cd") == 0) {
		cdExec(cmdDup);
	}
	else if (strcmp(cmdTok, "path") == 0) {
		pathExec(cmdDup, path);
	}
	else{
		char *prog2 = accessCheck(cmdTok, path);
		if (prog2 != NULL) {
		char *args[1000];
		int c = 0;
		args[c++] = cmdTok;
		cmdTok = "";
		while ((cmdTok = strsep(&cmdDup, " \t\f\n\r\v"))) {
			
			if (strcmp(cmdTok, "")) {
				args[c++] = cmdTok;
			}
		}
		args[c] = NULL;
				
		execv(prog2, args);
	}
	else {

		write(STDERR_FILENO, error_message, strlen(error_message));	
	}
	}
	
}

void redirectExec(char* cmd, char* path) { //cmd has command with spaces!
	
	if (cmd == NULL || emptyCheck(cmd)) {
		return ;
	}
	if (strstr(cmd, ">")) {
		char * cmdTok = malloc(1000);
		char * cmdDup = strdup(cmd);
		char * prog = strsep(&cmdDup, ">");   
		// prgog has     ls    -l     
		//cmdDup has      output   
		
		//Let's get the file first
		cmdTok = "";
		
		while (cmdTok!= NULL && strcmp(cmdTok, "") == 0) {
			cmdTok = strsep(&cmdDup, " \t\f\n\r\v");
		}
		//cmdTok now has file name without spaces
		
		FILE * fptr = fopen(cmdTok, "w");
		
		int fd = fileno(fptr);
		
		if (dup2(fd, 1) == -1){
			write(STDERR_FILENO, error_message, strlen(error_message));
		}
		if (dup2(fd, 2) == -1) {
			write(STDERR_FILENO, error_message, strlen(error_message));
		}
		
		//Handle program:
		

		cmdDup = strdup(prog);
				
		cmdTok = "";
		while (cmdTok!= NULL && strcmp(cmdTok, "") == 0) {
			
			cmdTok = strsep(&cmdDup, " \t\f\n\r\v");
			
		}
		
		prog = cmdTok;
		//cmdDup has -l with spaces at end
		//cmdTok has ls     -l   
		//cmdDup = strdup(cmdTok);
		//prog = strsep(&cmdDup, " \t\f\n\r\v");
		//prog has ls and cmdDup has      -l
		char * prog2 = accessCheck(prog, path);
		if (prog2 != NULL) {
			
		char *args[1000];
		int c = 0;
		args[c++] = prog;
		cmdTok = "";
		
		
		while ((cmdTok = strsep(&cmdDup, " \t\f\n\r\v"))) {
			
			if (strcmp(cmdTok, "")) {
				args[c++] = cmdTok;
			}
		}
		args[c] = NULL;
		execv(prog2, args);
	}
	else {
		
		write(STDERR_FILENO, error_message, strlen(error_message));	
	}
	}
	else {
		builtInExec(cmd, path);
		
	}
}

void parExec(char* cmd, char* path) { 
if (cmd == NULL || emptyCheck(cmd)) {
	return ;
}

	if (strstr(cmd, "&") == NULL) {
	
	if (cmd == NULL || emptyCheck(cmd))
		return ;
	//want to get first word
	char* cmdTok = malloc(1000);
	char* cmdDup = strdup(cmd);
	cmdTok = "";
	while (cmdTok != NULL && strcmp(cmdTok, "") == 0) {
		cmdTok = strsep(&cmdDup, " \t\f\n\r\v");
	}
	//cmdTok has first word //cmdDup has       args
	if (strcmp(cmdTok, "exit") == 0) {
		exit(0);
	}
	else if (strcmp(cmdTok, "cd") == 0) {
		cdExec(cmdDup);
	}
	else if (strcmp(cmdTok, "path") == 0) {
		pathExec(cmdDup, path);
	}
	else {
		if (fork() == 0)
		redirectExec(cmd, path);
		else {
			wait(NULL);
		}
	}
	}
	else {
	char* cmdTok = malloc(1000);
	char* cmdDup = strdup(cmd);
	int c = 0;
	
	
	char * commands[1000] = {NULL}; //to hold all the commands to be executed parallely
	while ((cmdTok = strsep(&cmdDup, "&"))) {
		if (!emptyCheck(cmdTok)) {
			commands[c++] = cmdTok;
		}
	}
	for(int i = 0; i < c; i++) {
		if (fork() == 0) {
			
			
			redirectExec(commands[i], path);
			
		
		}			
	}
	for(int i = 0; i < c; i++) {
		wait(NULL);
	}
	
	}
}


int main(int argc, char * argv[]) {
	
	if (argc == 1) {
		char path[1000] = " /bin ";
		while (1){
			fflush(stdout);
			printf("smash> ");
			fflush(stdout);
			char * cmd = malloc(1000);
			//fflush(stdin);
			//char * buffer = NULL;
			size_t len= 0;
			getline(&cmd, &len, stdin);
			//scanf("%[^\n]%*c", cmd);
			
			char * cmdDup = strdup(cmd);
			
			//char * cmdTok = malloc(1000);
			char * commands[1000] = {NULL};
			
			if (multipleCheck(cmdDup, path, commands)) {
				
				write(STDERR_FILENO, error_message, strlen(error_message));
			}
			else {
				
				for(int i = 0; commands[i]; i++) {
					parExec(commands[i], path);
				}
			}	
			if (feof(stdin)) {
				exit(0);
			}
			
		}
	
	}		
	if (argc == 2) {
		FILE* file = fopen(argv[1], "r");
		if (file == NULL) {
			write(STDERR_FILENO, error_message, strlen(error_message));
			exit(1);
		}
		char path[1000] = " /bin ";
		char * cmd = NULL;
		size_t len = 0;
		while (getline(&cmd, &len, file) != -1) {
			char * cmdDup = strdup(cmd);
			char * commands[1000] = {NULL};
			
			if (multipleCheck(cmdDup, path, commands)) {
				write(STDERR_FILENO, error_message, strlen(error_message));
			}
			else {
				
				for(int i = 0; commands[i]; i++) {
					parExec(commands[i], path);
				}
			}
			
		}
		
			if (feof(file)) {
				
				fclose(file);
				exit(0);
			}
		
	}
	else {
		write(STDERR_FILENO, error_message, strlen(error_message));
		exit(1);
	}

}
	

