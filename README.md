# Microshell

This is a simple shell program that mimics basic shell functionality, such as executing commands with pipes and semicolons. It was developed as part of an exam (rank 4) at School 42.

## Table of Contents

- [Introduction](#introduction)
- [Features](#features)
- [How to Use](#how-to-use)
- [Code Explanation](#code-explanation)
- [Tips](#tips)

## Introduction

Microshell is a command-line program that allows you to execute shell commands. It can handle basic features such as pipes ('|') and semicolons (';'). The program also implements the 'cd' command for changing directories and provides error handling for system calls and failed executions.

## Features

- Executes shell commands with pipes and semicolons.
- Implements the 'cd' built-in command.
- Error handling for system calls and failed executions.

## How to Use

To use the microshell program, follow these steps:

1. Compile the program using a C compiler (e.g., GCC):

   ```
   gcc -o microshell microshell.c
   ```

2. Run the program with the desired shell commands as arguments. For example:

   ```
   ./microshell /bin/ls "|" /usr/bin/grep microshell ";" /bin/echo i love my microshell
   ```

This command will execute a sequence of shell commands as specified.

## Code Explanation

### `microshell.h`

This header file contains necessary includes and defines error messages.

### `microshell.c`

The main program file contains the code for the microshell. Here's an overview of the code:

- `print`: A function for printing error messages to the standard error stream (stderr).
- `err`: A function to print an error message and return an error status.
- `error_fds`: A function to handle file descriptor errors during piping.
- `cd`: Implements the 'cd' command with error handling.
- `exec`: Executes external commands using `execve` with error handling.
- `main`: The main function that parses command-line arguments and executes commands accordingly.

## Tips

- The program implements pipes ('|') and semicolons (';') like a basic shell.
- The 'cd' command is implemented as a built-in command.
- Error handling is in place for system calls and failed executions.
- The code is designed to manage more than hundreds of pipes, even with limited open file descriptors.


## More details

### `cd` Function

```c
int cd(char *dir, int i)
{
    if (i != 2)
        return (err(BAD, NULL));
    if (chdir(dir) == -1)
        return (err(CD, dir));
    return 1;
}
```

1. `cd` is a function that handles the 'cd' command.
2. It checks if the number of arguments (i) is equal to 2. If not, it returns an error message (BAD) because 'cd' should have exactly one argument.
3. If the argument count is correct, it attempts to change the current working directory using `chdir(dir)`.
4. If `chdir` returns -1 (indicating an error), it returns an error message (CD) indicating that the directory change failed.
5. If everything is successful, it returns 1 to indicate success.

### `exec` Function

```c
int exec(char **token, char **env, int i, int has_pipe)
{
    int fd[2];

    if (has_pipe && pipe(fd) == -1)
        return (err(FAT, NULL));
    int pid = fork();
    if (pid == 0)
    {
        token[i] = NULL;
        if (has_pipe && error_fds(fd, 1))
            return (err(FAT, NULL));
        execve(*token, token, env);
        return (err(EXE, *token));
    }
    if (has_pipe && error_fds(fd, 0))
        return (err(FAT, NULL));
    waitpid(pid, &i, 0);
    return (WEXITSTATUS(i));
}
```

1. `exec` is a function responsible for executing external commands.
2. It checks whether a pipe is required (has_pipe) and, if so, creates a pipe using `pipe(fd)`.
3. It then forks a new process using `fork()`.
4. In the child process (pid == 0), it modifies the `token` array to prepare it for execution by setting the last element to NULL.
5. If a pipe is required, it checks for errors in setting up file descriptors (error_fds) for the pipe.
6. It uses `execve` to execute the command, passing the token array, environment (env), and the path to the executable.
7. If `execve` returns, it means there was an error, so it returns an error message (EXE) with the name of the failed executable.
8. In the parent process (pid > 0), it checks for errors in setting up file descriptors for the pipe if needed.
9. It waits for the child process to finish using `waitpid` and retrieves the exit status.
10. It returns the exit status of the executed command.

### `main` Function

```c
int main(int i, char **token, char **env)
{
    i = 0;
    int status = 0;

    while (token[i] && token[++i])
    {
        token += i;
        i = 0;
        while (token[i] && strcmp(token[i], "|") && strcmp(token[i], ";"))
            i++;
        if (!strcmp(*token, "cd"))
            status = cd(token[1], i);
        else if (i)
            status = exec(token, env, i, token[i] && !strcmp(token[i], "|"));
    }
    return (status);
}
```

1. The `main` function is the entry point of the program.
2. It initializes variables for command parsing and status tracking.
3. It uses a while loop to iterate through the command-line arguments (token array).
4. It parses the command sequence using nested loops to identify command boundaries (| and ;).
5. If the current command is 'cd', it calls the `cd` function to change the directory.
6. If it's an external command, it calls the `exec` function to execute the command and track its exit status.
7. The final status returned by the `main` function is the exit status of the last executed command.


