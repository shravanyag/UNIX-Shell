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
void file_input(char *contents);

char ***history;
int queue;

int main(int argc, char *argv[])
{
	char **buf = malloc(BUFSIZE * sizeof(char*));
	history = malloc(25 * sizeof(char **));
	queue = -1;
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

void insert_history(char **buf)
{
	if(strcmp(buf[0], "prev") != 0 && strcmp(buf[0], "history") != 0)
	{
		queue = (queue + 1) % 25;
		history[queue] = buf;
	}
}

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



