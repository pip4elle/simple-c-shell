#include "sh.h"


int sh_launch(char** args) {

    pid_t pid, wpid;
    int status;

    pid = fork();

    if(pid < 0) {
        fprintf(stderr, "ERROR: Couldn't fork the main process\n");
        return 1;
    } else if(pid == 0) {
        if(execvp(args[0], args) == -1) {
            fprintf(stderr, "ERROR: Couldn't run %s\n", args[0]);
            exit(EXIT_FAILURE);
        }
    } else {
        do {
            wpid = waitpid(pid, &status, WUNTRACED);

            if (wpid == -1) {
                fprintf(stderr, "ERROR: Couldn't wait for the child process to exi\n");
            }

        } while(!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}


int sh_execute(char** args) {
    
    if(args[0] == NULL)
        return 1;
}


char** sh_tokenize(char* line) {

    int buffsize = TOKENS_BUFF_SIZE, position = 0;
    char* token;
    char** tokens = (char**) calloc(buffsize, sizeof(char*));

    if(!tokens) {
        fprintf(stderr, "ERROR: Tokens buffer allocation could not be performed\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, TOKENS_DELIMITERS);

    while(token != NULL) {
        // The -1 is used to ensure NULL terminator character
        if(position >= buffsize - 1) { 
            buffsize += TOKENS_BUFF_SIZE;
            tokens = (char**) realloc(tokens, buffsize * sizeof(char*));

            if(!tokens) {
                fprintf(stderr, "ERROR: Tokens buffer allocation could not be performed\n");
                exit(EXIT_FAILURE);
            }
        }

        tokens[position] = token;
        
        position++;

        token = strtok(NULL, TOKENS_DELIMITERS);
    }

    tokens[position] = NULL;
    return tokens;
}


char* sh_readline() {

    int buffsize = LINE_BUFF_SIZE, position = 0;
    char* buffer = (char*) calloc(buffsize, sizeof(char));
    char c;

    if(!buffer) {
        fprintf(stderr, "ERROR: Line buffer allocation could not be performed\n");
        exit(EXIT_FAILURE);
    }

    printf(" :> ");

    while(1) {
        c = getchar();
        
        // Handle EOF before reading any character
        if (c == EOF && position == 0) {
            free(buffer);
            return NULL;
        }

        // The EOF check is used in case a file is used as stdin
        if(c == '\n' || c == EOF) {
            buffer[position] = '\0';
            return buffer;
        }

        buffer[position] = c;
        
        position++;

        // Ensure space for null terminator
        if (position >= buffsize - 1) {
            buffsize += LINE_BUFF_SIZE;
            char* temp = (char*) realloc(buffer, buffsize * sizeof(char));

            if (!temp) {
                free(buffer);
                fprintf(stderr, "ERROR: Line buffer allocation could not be performed\n");
                exit(EXIT_FAILURE);
            }

            buffer = temp;
        }
    }
}


void sh_loop() {

    int status;
    char* line;
    char** args;

    do {
        line = sh_readline();
        args = sh_tokenize(line);
        status = sh_execute(args);

        free(line);
        free(args);
    } while(status);
}