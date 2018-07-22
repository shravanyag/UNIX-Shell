#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<unistd.h>
#include<errno.h>
#define BUFSIZE 100

int shell_call(char **buf);
char **accept_command();

int main(int argc, char *argv[])
{
	char **buf = malloc(BUFSIZE * sizeof(char*));
	printf("Welcome to our shell.\n");
	printf("To exit type `quit` and to set environment variables:\n");
	printf("`PATH = ...` to set path\n");
	printf("`CWD = ...` to set correct working directory\n");
	printf("`SHELL = ...` to set shell to execute\n\n");
	int status;
	while(1)
	{
		printf(">> ");	
		buf = accept_command();
		if(strcmp(buf[0], "cd") == 0)
		{
			if(chdir(buf[1]) != 0)
				printf("error: could not perform cd\n");
		}
		else if(strcmp(buf[0], "quit") == 0)
			exit(0);
		else
			status = shell_call(buf);
	}	
}

char** accept_command()
{
	char *input = malloc(sizeof(char) * BUFSIZE);
	int index = 0;
	int ch;	
	while((ch = getchar()) != '\n')
	{
		input[index] = ch;
		index++;
	}

	int words = 0;
  	char **buf = malloc(BUFSIZE * sizeof(char*));
  	char *token = strtok(input, " ");

	while (token != NULL) 
	{
		buf[words] = token;
		words++;
		token = strtok(NULL, " ");
	}
	buf[words] = NULL;
	return buf;

}

int shell_call(char **buf)
{
	pid_t pid;
	int status;
	pid = fork();
	if(pid == -1)
	{
		printf("Error : could not fork the child\n");
	}
	else if(pid == 0)
	{		
		if((execvp(buf[0], buf)) == -1)
			fprintf(stderr, "shell: couldn't exec %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	else if(pid > 0)
	{
		waitpid(pid, &status, WUNTRACED);
	}
	return 1;
}
