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
int words;

int main(int argc, char *argv[])
{
	char **buf = malloc(BUFSIZE * sizeof(char*));
	int status;
	while(1)
	{
		printf(">> ");	
		buf = accept_command();
		status = shell_call(buf);
	}	
}

/*void accept_command(char *buf)
{
	int ch;	
	int index = 0;
	while((ch = getchar()) != '\n')
	{
		buf[words][index] = ch;
		index++;
	}

	buf[words][index] = '\0';
	char *token = strtok(buf
	/*	
	 for(int i = 0; i <= words; ++i)
	 {
	 	printf("%d %s\n", i+1, buf[i]);
	 }
	 
}
*/
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
	printf("%s\n", input);

	int position = 0;
  	char **tokens = malloc(BUFSIZE * sizeof(char*));
  	char *token;

	token = strtok(input, " ");
	while (token != NULL) 
	{
		tokens[position] = token;
		position++;
		token = strtok(NULL, " ");
	}
	tokens[position] = NULL;
	return tokens;

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
