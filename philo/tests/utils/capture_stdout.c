#include <errno.h>
#include "../tests.h"

/* Guards against nested captures: a second start_capture() without an     */
/* end_capture() in between would lose the real stdout forever.            */
static int	g_capturing = 0;

/* One capture file per process so parallel test runs never clash. */
static const char	*capture_path(void)
{
	static char	path[64];

	if (path[0] == '\0')
		snprintf(path, sizeof(path), "/tmp/.unit_test_capture_%d", getpid());
	return (path);
}

int	start_capture(void)
{
	int	fd;
	int	saved_stdout;

	if (g_capturing)
		return (-1);
	fflush(stdout);
	fd = open(capture_path(), O_WRONLY | O_CREAT | O_TRUNC, 0600);
	if (fd == -1)
		return (-1);
	saved_stdout = dup(STDOUT_FILENO);
	if (saved_stdout == -1)
	{
		close(fd);
		return (-1);
	}
	if (dup2(fd, STDOUT_FILENO) == -1)
	{
		close(fd);
		close(saved_stdout);
		return (-1);
	}
	close(fd);
	g_capturing = 1;
	return (saved_stdout);
}

/* read() may deliver fewer bytes than asked, or be interrupted by a       */
/* signal (EINTR) — relevant for projects that install handlers.           */
static int	read_all(int fd, char *buf, ssize_t n)
{
	ssize_t	total;
	ssize_t	r;

	total = 0;
	while (total < n)
	{
		r = read(fd, buf + total, n - total);
		if (r < 0 && errno == EINTR)
			continue ;
		if (r <= 0)
			return (-1);
		total += r;
	}
	return (0);
}

char	*end_capture(int saved_stdout)
{
	char	*buf;
	int		fd;
	ssize_t	n;

	fflush(stdout);
	dup2(saved_stdout, STDOUT_FILENO);
	close(saved_stdout);
	g_capturing = 0;
	fd = open(capture_path(), O_RDONLY);
	if (fd == -1)
		return (NULL);
	n = lseek(fd, 0, SEEK_END);
	if (n == -1)
	{
		close(fd);
		return (NULL);
	}
	buf = malloc(n + 1);
	if (!buf)
	{
		close(fd);
		return (NULL);
	}
	lseek(fd, 0, SEEK_SET);
	if (read_all(fd, buf, n) == -1)
	{
		free(buf);
		close(fd);
		return (NULL);
	}
	buf[n] = '\0';
	close(fd);
	unlink(capture_path());
	return (buf);
}
