#include "sh.h"


char* sh_built_in_cmd_list[] = {
    "help",
    "cd",
    "exit"
};


int (*sh_built_in_funcs[]) (char**) = {
    &sh_cmd_help,
    &sh_cmd_cd,
    &sh_cmd_exit
};


int built_in_cmds_num() {
    return sizeof(sh_built_in_cmd_list) / sizeof(char*);
}


int sh_cmd_help(char** args) {

    printf("Built ins programs:\n\n");
    
    for(int i = 0; i < built_in_cmds_num(); i++)
        printf("%s\n", sh_built_in_cmd_list[i]);
    
    printf("\nFor informations on external programs use man.\n");

    return 1;
}


int sh_cmd_cd(char** args) {
    
    if(args[1] == NULL) {
        fprintf(stderr, "ERROR: You must provide a path to cd.\n");
        return 2;
    }

    if(chdir(args[1]) == -1) {
        fprintf(stderr, "ERROR: Couldn't enter %s.\n", args[1]);
        return 2;
    }
    
    return 1;
}


int sh_cmd_exit(char** args) {
    printf("EXIT");
    return 0;
}


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

    for(int i = 0; i < built_in_cmds_num(); i++)
        if(strcmp(args[0], sh_built_in_cmd_list[i]) == 0)
            return (*sh_built_in_funcs[i])(args);
    
    return sh_launch(args);        
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