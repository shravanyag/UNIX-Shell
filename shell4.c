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
void insert_history(char **buf);
int pseudonym(char **buf);
int redirect(char **buf);
int piping(char **buf);
void file_input(char *contents);

char ***history;
char **alias;
int queue;
int alias_index;
int start = -1;

int main(int argc, char *argv[])
{
	char **buf = malloc(BUFSIZE * sizeof(char*));
	history = malloc(25 * sizeof(char **));
	alias = malloc(25 * sizeof(char *));
	queue = -1;
	alias_index = -1;
	int curr = -1;
	printf("Welcome to our shell.\n");
	printf("To exit type `quit` and to set environment variables:\n");
	printf("`PATH = ...` to append path\n");
	
	printf("\n");
	int status;
	while(1)
	{
		printf("(*_*) ");	
		buf = accept_command();
		insert_history(buf);
		if(strcmp(buf[0], "prev") == 0)
		{
			if (queue != -1)
			{
				curr = (queue - atoi(buf[1]) + 1) % 25;
				buf = history[curr];
			}
			else
				printf("History is empty!!!\n");
		}
		if(strcmp(buf[0], "cd") == 0)
		{
			if(chdir(buf[1]) != 0)
			{
				printf("error: could not perform cd\n");
			}
		}
		else if(strcmp(buf[0], "quit") == 0)
		{
			exit(0);
		}

		else if(strcmp(buf[0], "PATH") == 0 && buf[1] == NULL)
		{
			printf("PATH - %s\n", getenv("PATH"));
		}
		else if(strcmp(buf[0], "PATH") == 0 && strcmp(buf[1] , "=") == 0)
		{
			char *temp = (char *)(malloc(sizeof(char) * 1000));
			temp = strcat(getenv("PATH"), ":");
			strcat(temp, buf[2]);
			setenv("PATH", temp, 1);
			printf("PATH set to : \n%s\n", temp);
		}
		else if(strcmp(buf[0], "CWD") == 0 && buf[1] == NULL)
		{
			char cwd[1000];
	
			if (getcwd(cwd, sizeof(cwd)) != NULL)	
			{
				printf("CURRENT WORKING DIR: %s\n", cwd);
			}
		}	
		else if(strcmp(buf[0], "aka") == 0)
		{
			if((pseudonym(buf)) == 0)
			{
				printf("Couldn't rename!\n");
			}
		}
		else if(strcmp(buf[0], "re") == 0)
		{
			if(redirect(buf) == 0)	
			{
				printf("Couldn't redirect\n");
			}
		}
		else if(strcmp(buf[0], "pipe") == 0)
		{
			if(piping(buf) == 0)
			{
				printf("Couldn't execute pipes\n");
			}
		}
		//editor part is this
		else if(strcmp(buf[0], "editor") == 0)
		{
			if(buf[1] != NULL && buf[2] == NULL)
			{
				FILE * fp;
				int nothing = 0;
				char *contents = (char *)(malloc(sizeof(char) * 10000));
				char *reading = (char *)(malloc(sizeof(char) * 50000));
				if(fp = fopen(buf[1], "r"))
				{
					char c = fgetc(fp);
					int iy = 0;
					while(c != EOF)
					{
						reading[iy] = c; 
						printf("%c", c);
						c = fgetc(fp);
						++iy;
					}
					fclose(fp);
				}
				else
				{
					reading = "";
					nothing = 1;
				}
				file_input(contents);
				fp = fopen(buf[1], "w");
		
				if(nothing == 0)
				{
					strcat(reading, contents);
					fputs(reading, fp);
				}
				else
					fputs(contents, fp);
				
				fclose(fp);
			}
			else
			{
				printf("error : check number of arguments\n");
			}
		}
		else if(strcmp(buf[0], "history") == 0)
		{
			printf("Previously executed commands are:\n");
			int ix = 0; int iy;
			if(queue != -1)
			{
				while(history[ix] != NULL)
				{
					iy= 0;
					while(history[ix][iy] != NULL)
					{
						printf("%s ", history[ix][iy]);
						iy += 1;
					}
					printf("\n");
					ix += 1;
				}
			}
			else
				printf("History empty!!!\n");
		}		
		else
		{
			status = shell_call(buf);
		}
		printf("\n");
	}	
}

char** accept_command()
{
	char *input = malloc(sizeof(char) * BUFSIZE);
	int index = 0;
	int arrows = 0;
	int ch;	
	char **buf = malloc(BUFSIZE * sizeof(char*));
	while((ch = getchar()) != '\n')
	{
		if(ch == '\033')
		{
			int count = 1;
			buf[0] = "prev";
			buf[1] = "1";
			return buf;
		}
		input[index] = ch;
		index++;
	}
	int words = 0;
  	char *token = strtok(input, " ");

	while (token != NULL) 
	{
		buf[words] = token;
		words++;
		token = strtok(NULL, " ");
	}
	buf[words] = NULL;
	int alias_temp = 0;
	
	while(alias_temp <= alias_index)
	{
		printf("ok1\n");
		if(strcmp(alias[alias_temp], buf[0]) == 0)
		{
			printf("ok2\n");
			buf[0] = alias[alias_temp + 1];
			buf[1] = "\0";
		}
		alias_temp += 2;
	}
	printf("%s\n", buf[0]);
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
		{
			fprintf(stderr, "shell: couldn't exec %s\n", strerror(errno));
		}
		exit(EXIT_FAILURE);
	}
	else if(pid > 0)
	{
		waitpid(pid, &status, WUNTRACED);
	}
	return 1;
}

int pseudonym(char **buf)
{
	char *temp = (char *)malloc(sizeof(char) * 1000);
	char *shell_cmd = (char *)malloc(sizeof(char) * BUFSIZE);
	temp = buf[1];
	printf("Renamed - %s\n", temp);
	int i = 3;
	while(buf[i] != NULL)
	{
		strcat(shell_cmd, buf[i]);
		strcat(shell_cmd, " ");
		++i;
	}
	alias[alias_index + 1] = temp;
	alias[alias_index + 2] = shell_cmd;
	alias_index += 2;
	printf("Final shell_cmd = %s\n", shell_cmd);	
	return 1;
}

int redirect(char **buf)
{
	if(strcmp(buf[2], ">") == 0 && buf[4] == NULL)
	{
		close(1); // Release fd no - 1
		int fd = fopen(buf[3], "w"); // Open a file with fd no = 1
		// Child process
		if (fork() == 0)
		{
			execvp(buf[1], buf); // By default, the program writes to stdout (fd no - 1). ie, in this case, the file
		}
	}
	else if(buf[2] != ">")
	{
		close(2); // Release fd no - 2
    		int fd = fopen("stderr.txt", "w"); // Opens file with fd no - 2
    		// Child process
    		if (fork() == 0) 
		{
        		execvp(buf[1], buf); // Writes to stderr (fd no 2)
    		}
	}
	else if(strcmp(buf[2], ">") == 0 && buf[4] != NULL)
	{
		close(1); // Release fd no - 1
    		int fd = fopen("stdout_stderr.txt", "w"); //Opens file with fd no - 1
    		// Child process
    		if (fork() == 0) 
		{
        		close(2); // Release fd no - 2
        		dup(1); // Create fd no - 2 which is duplicate of fd no -1. Hence, we joined fd 1 and 2 (stdout and  stderr)
        		execvp(buf[1], buf);
    		}
	}
	else if(strcmp(buf[4], "<") == 0)
	{
		close(0);//Release fd - 0
    		int fd = fopen("stdout.txt", "r"); //Open file with fd - 0
    		//Child process
    		if (fork() == 0) 
		{
			printf("fd - %d\n", fd);
        		execvp(buf[1], buf); // By default, program reads from stdin. ie, fd - 0
    		}
	}
	return 1;
}

int piping(char **buf)
{

// multiple pipes
// 0 is read end, 1 is write end
	int pipefd[2]; 
	pid_t p1, p2;

	if (pipe(pipefd) < 0) 
	{
		printf("\nPipe could not be initialized");
		return 0;
	}
	p1 = fork();
	if (p1 < 0) 
	{
		printf("\nCould not fork");
		return 0;
	}

	if (p1 == 0) 
	{
		// Child 1 executing..
		// It only needs to write at the write end
		close(pipefd[0]);
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);

		if (execvp(buf[1], buf) < 0) 
		{
	    		printf("\nCould not execute command 1..");
	    		exit(0);
		}
	} 
	else 
	{
		// Parent executing
		p2 = fork();

		if (p2 < 0) 
		{
	    		printf("\nCould not fork");
	    		return 0;
		}

		// Child 2 executing..
		// It only needs to read at the read end
		if (p2 == 0) 
		{
	    		close(pipefd[1]);
	    		dup2(pipefd[0], STDIN_FILENO);
	    		close(pipefd[0]);
	    		if (execvp(buf[1], buf) < 0) 
			{
				printf("\nCould not execute command 2..");
				exit(0);
	    		}
		} 
		else 
		{
	    		// parent executing, waiting for two children
	    		wait(NULL);
	    		wait(NULL);
		}
	}
}

void insert_history(char **buf)
{
	if(strcmp(buf[0], "prev") != 0 && strcmp(buf[0], "history") != 0)
	{
		queue = (queue + 1) % 25;
		history[queue] = buf;
	}
}

//editor part
void file_input(char *contents)
{
	int index = 0;
	int ch;
	while((ch = getchar()) != '~')
	{
		if(ch == '\033')
		{
			printf("arrow key entered\n");
			continue;
		}
		contents[index] = ch;
		index++;
	}
}
