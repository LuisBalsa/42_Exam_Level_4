#include "microshell.h"

void	print(char *s)
{
	while (s && *s)
		write(2, s++, 1);
}

int	err(char *msg, char *key)
{
	print(msg);
	print(key);
	print("\n");
	return 1;
}

int	error_fds(int *fd, int i)
{
	return(dup2(fd[i], i) == -1 || close(fd[0]) == -1 || close(fd[1]) == -1);
}

int	cd(char *dir, int i)
{
	if (i != 2)
		return (err(BAD, NULL));
	if (chdir(dir) == -1)
		return (err(CD, dir));
	return 0;
}

int	exec(char **token, char **env, int i, int has_pipe)
{
	int	fd[2];

	if (has_pipe && pipe(fd) == -1)
		return (err(FAT, NULL));
	int	pid = fork();
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

int	main(int i, char **token, char **env)
{
	i = 0;
	int	status = 0;

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
