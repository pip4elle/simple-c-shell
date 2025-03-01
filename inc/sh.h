#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


#define LINE_BUFF_SIZE 128
#define TOKENS_BUFF_SIZE 16
#define TOKENS_DELIMITERS " \t\r\n\a"


void sh_loop();
char* sh_readline();
char** sh_tokenize(char* line);
int sh_execute(char** args);
int sh_launch(char** args);