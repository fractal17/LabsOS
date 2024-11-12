#include "main.h"

// cd command:
// change the current default directory to <directory>
int shell_command_cd(char *args[]) {
    if (args[1] == NULL) {
        char path[256];

        getcwd(path, 256);
        printf("%s\n", path);

        return 0;
    }

    // try to change the current default directory to <directory>
    if (chdir(args[1])) {
        perror("cd");
        return 0;
    }

    return 0;
}

// clr command:
// clear the screen
int shell_command_clr(char *args[]) {
    printf("\033c");
    return 0;
}

// dir command:
// output the contents of the <directory>
int shell_command_dir(char *args[]) {
    DIR *directory;
    struct dirent *entry;

    // if the <directory> argument is missing, content of the current direcotry is output
    if (args[1] == NULL) {
        char path[256];
        getcwd(path, 256);

        directory = opendir(path);
    }
    else
        directory = opendir(args[1]);

    if (directory == NULL) {
        perror("dir");
        return 0;
    }

    while ((entry = readdir(directory)) != NULL) {
        // output the contents of the <directory>
        printf("%s\n", entry->d_name);
    }

    closedir(directory);

    return 0;
}

// echo command:
// display <comment> on the display followed by a new line
int shell_command_echo(char *args[]) {
    if (args[1] == NULL) {
        printf("Usage: echo <comment>\n");
        return 0;
    }

    // output <comment> on the display followed by a new line
    for (int i = 1; i < sizeof(*args); ++i) {
        if (args[i] == NULL)
            break;

        printf("%s ", args[i]);
    }
    printf("\n");

    return 0;
}

// env command:
// list all the environment strings
int shell_command_env(char *args[]) {
    // environ is a global variable that contains the environment strings
    extern char **environ;

    int i = 0;
    // output all the environment strings
    while(environ[i])
        printf("%s\n", environ[i++]);

    return 0;
}

// help command:
// display the help message
int shell_command_help(char *args[]) {
    printf("Avalibale commands:\n");

    //cd
    printf("cd: \ncd <directory> change the current default directory to <directory>. "
           "If the <directory> argument is missing, outputs the current directory. "
           "If the directory is missing, outputs the error message\n\n");

    //clr
    printf("clr: \nclear the screen\n\n");

    //dir
    printf("dir: \ndir <directory> output the contents of the <directory>. "
           "If the <directory> argument is missing, content of the current direcotry is output. "
           "If the <directory> argument is null, then an error message is displayed.\n\n");

    //echo
    printf("echo: \necho <comment> display <comment> on the display followed by a new line. "
           "Multiple spaces/tabulations reduced to a sungle space.\n\n");

    //env
    printf("env: \n ist all the environment strings\n\n");

    //pause
    printf("pause: \npause operation of the shell until any key is pressed\n\n");

    //quit
    printf("quit: \nquit the shell\n\n");

    //any other command
    printf("Any other command will be executed by the shell.\n\n");

    //help
    printf("Use 'help' to display this help message.\n");

    return 0;
}

// pause command:
// pause operation of the shell until any key is pressed
int shell_command_pause(char *args[]) {
    printf("Shell on pause! Press any key...\n");
    getchar();

    return 0;
}

// quit command:
// quit the shell
int shell_command_quit(char *args[]) {
    return 1;
}

// execute_command:
// execute the any command which is not built-in
int execute_command (char *args[]) {
    // create a child process
    pid_t pid = fork();

    switch (pid) {
        // if the child process is created successfully, execute the command
        case 0:
            execvp(args[0], args);
            // if the command is not found, output an error message
            perror("execvp");
            return(EXIT_FAILURE);

        // if the child process is not created, output an error message
        case -1:
            perror("fork");
            break;
        // wait for the child process to finish
        default:
            wait(NULL);
    }

    return 0;
}

// list of built-in commands
char *command_list[] = {
    "cd",
    "clr",
    "dir",
    "echo",
    "env",
    "help",
    "pause",
    "quit"
};

// Array of pointers to functions that implement built-in commands
// where (*commands[]) - array of pointers t functions
// where int (...) (char *[]) - pointer to function that takes an array of pointers to char
// as an argument and returns an integer
int (*commands[]) (char *[]) = {
        &shell_command_cd,
        &shell_command_clr,
        &shell_command_dir,
        &shell_command_echo,
        &shell_command_env,
        &shell_command_help,
        &shell_command_pause,
        &shell_command_quit
};

// command_count:
// return the number of built-in commands
int command_count() {
    return sizeof(command_list) / sizeof(char *);
}

// is_command_exist:
// check if the command is built-in
bool is_command_exist(char * args[]) {
    int i;
    for (i = 0; i < command_count(); ++i) {
        // if the command is built-in, execute it
        if (args[0] != NULL && strcmp(args[0], command_list[i]) == 0) {
            // execute the command
            status = (*commands[i])(args);
            break;
        }
    }
    // return true if the command is built-in
    return i != command_count();
}


int main(int argc, char *argv[]) {
    char *input_ptr; // pointer to the input string
    size_t input_size; // size of the input string
    status = 0; // status of the shell, if 1 then shell will be terminated
    char *args[10]; // array of pointers to the input string
    char path[256]; // path to the shell directory

    getcwd(path, 256);
    // set the environment variables
    setenv("shell", strcat(path, "/linuxshell"), 1);
    setenv("parent", path, 1);

    while(!status) {
        getcwd(path, 256);
        // output the shell prompt
        // \033[0;32m - green color
        printf("\033[0;32m");
        printf("LinuxShell: ");
        // \033[0;34m - blue color
        printf("\033[0;34m");
        printf("%s", path);
        // \033[0m - reset color
        printf("\033[0m");
        printf("$ ");

        input_ptr = NULL;
        getline(&input_ptr, &input_size, stdin);

        // split the input string into tokens
        int i = 0;
        args[i++] = strtok(input_ptr, " \n");
        while((args[i++] = strtok(NULL, " \n")));

        if (!is_command_exist(args))
            execute_command(args);

        // free the memory
        input_size = 0;
        free(input_ptr);
        memset(args, 0, 10 * sizeof(char *));
    }
}
