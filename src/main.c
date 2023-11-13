#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"

int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);

char *builtin_str[] = {
        "cd",
        "help",
        "exit"
};
int (*buitin_func[])(char **) = {
    &lsh_cd,
    &lsh_help,
    &lsh_exit
};

int lsh_num_builtins(){
    return sizeof(builtin_str) / sizeof(char *);
}

int lsh_cd(char **args){
    if (args[1] == NULL){
        fprintf(stderr, "lsh: expected argument to \"cd\"\n");
    } else {
        if (chdir(args[1]) != 0){
            perror("lsh");
        }
    }
    return 1;
}

int lsh_help(char **args){
    int i;
    printf("Welcome to the lsh shell.\n");
    printf("Type program names and arguments, and hit enter.\n");
    printf("The following are built in:\n");
    for (i=0; i<lsh_num_builtins(); i++){
        printf(" %s\n", builtin_str[i]);
    }
    printf("Use the man command for more information on other commands.\n");
    return 1;
}

int lsh_exit(char **args){
    return 0;
}

char *lsh_read_line(void)
{
    int bufsize = LSH_TOK_BUFSIZE;
    int position = 0;

    char *line = malloc(bufsize * sizeof(char));
    int c;

    if (line == NULL){
        fprintf(stderr, "Error: unable to allocate memory for the line.\n");
        exit(EXIT_FAILURE);
    }
    while(1){
        c = getchar();

        if (c == EOF || c == '\n'){
            line[position] = '\0';
            return line;
        } else {
            line[position] = c;
        }
        position++;

        if (position >= bufsize){
            bufsize += LSH_TOK_BUFSIZE;
            line = realloc(line, bufsize * sizeof(char));
            if (line == NULL){
                fprintf(stderr, "Error: unable to allocate memory for the line.\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}

char **lsh_split_line(char *line)
{
    int bufsize = LSH_TOK_BUFSIZE, position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;

    if (!tokens){
        printf("Error: allocation error\n");
        exit(EXIT_FAILURE);
    }
    token = strtok(line, LSH_TOK_DELIM);

    while(token != NULL){
        tokens[position++] = token;

        if(position >= bufsize){
            bufsize += LSH_TOK_BUFSIZE;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if(!tokens) {
                printf("Error: reallocation error\n");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, LSH_TOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}

int lsh_launch(char **args)
{
    pid_t pid, wpid;
    int status;



    pid = fork();
    if (pid==0){
        if(execvp(args[0], args) == -1){
            perror("lsh");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0){
        perror("lsh");
    } else {
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        }while(!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 1;
}

int lsh_execute(char **args)
{
    int i;

    if (args[0] == NULL){
        return 1;
    }

    for (i=0; i < lsh_num_builtins(); i++){
        if (strcmp(args[0], builtin_str[i]) == 0){
            return (*buitin_func[i])(args);
        }
    }
    return lsh_launch(args);
}

void lsh_loop(void)
{
    char *line;
    char **args;
    int status=0;
    printf("Starting the loop: \n");
    printf("Line 1 value is here: %s\n", line);
    printf("Status 1 value is here: %d\n", status);
    do {
        printf(">");
        line = lsh_read_line();
        //printf("Line value is here: %s\n", line);
        args = lsh_split_line(line);
        //printf("Args value is here: \n");
        status = lsh_execute(args);
        free(line);
        free(args);
    } while (status);
}

int main(int argc, char **argv)
{
    // load config files, if any.
    printf("C shell is starting to run\n");
    // Run command loop.
    lsh_loop();

    //Perform any shutdown/cleanup.

    return EXIT_SUCCESS;
}





