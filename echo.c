/**
 * http://d.hatena.ne.jp/odz/20070507/1178558340
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <netinet/in.h>

#define SERVER_PORT 10007
#define MAX_EVENTS 10
#define BACKLOG 10

static int _listener;
static int epfd;

static void
die (const char* msg)
{
  perror (msg);
  exit (EXIT_FAILURE);
}

static void
setnonblocking (int sock)
{
  int flag = fcntl (sock, F_GETFL, 0);
  fcntl (sock, F_SETFL, flag | O_NONBLOCK);
}

static int
setup_socket ()
{
  int sock;
  struct sockaddr_in sin;

  if ((sock = socket (PF_INET, SOCK_STREAM, 0)) < 0)
    {
      die ("socket");
    }

  memset(&sin, 0, sizeof sin);

  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = htonl(INADDR_ANY);
  sin.sin_port = htons(SERVER_PORT);

  if (bind (sock, (struct sockaddr *) &sin, sizeof sin) < 0)
    {
      close (sock);
      die ("bind");
    }

  if (listen (sock, BACKLOG) < 0)
    {
      close (sock);
      die ("listen");
    }

  return sock;
}

int
main ()
{
  struct epoll_event ev;
  struct epoll_event events[MAX_EVENTS];
  char buffer[1024];

  if ((epfd = epoll_create (MAX_EVENTS)) < 0)
    {
      die ("epoll_create");
    }

  _listener = setup_socket ();

  memset(&ev, 0, sizeof ev);
  memset(events, 0, sizeof events);
  ev.events = EPOLLIN;
  ev.data.fd = _listener;
  epoll_ctl (epfd, EPOLL_CTL_ADD, _listener, &ev);

  for (;;)
    {
      int i;
      int nfd = epoll_wait (epfd, events, MAX_EVENTS, -1);

      for (i = 0; i < nfd; i++)
	{
	  if (events[i].data.fd == _listener)
	    {
	      struct sockaddr_in client_addr;
	      socklen_t client_addr_len = sizeof client_addr;

	      int client = accept (_listener, (struct sockaddr *) &client_addr,
				   &client_addr_len);
	      if (client < 0)
		{
		  perror ("accept");
		  continue;
		}

	      setnonblocking (client);
	      memset(&ev, 0, sizeof ev);
	      ev.events = EPOLLIN | EPOLLET;
	      ev.data.fd = client;
	      epoll_ctl (epfd, EPOLL_CTL_ADD, client, &ev);
	    }
	  else
	    {
	      int client = events[i].data.fd;
	      int n = read (client, buffer, sizeof buffer);
	      if (n < 0)
		{
		  perror ("read");
		  epoll_ctl (epfd, EPOLL_CTL_DEL, client, &ev);
		  close (client);
		}
	      else if (n == 0)
		{
		  epoll_ctl (epfd, EPOLL_CTL_DEL, client, &ev);
		  close (client);
		}
	      else
		{
		  write (client, buffer, n);
		}
	    }
	}
    }

  return 0;
}
