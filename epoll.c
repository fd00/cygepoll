/* epoll.c
 Copyright (c) Daisuke Fujimura, All rights reserved.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 3.0 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library.*/

#include <errno.h>
#include <limits.h>
#include <up.h>

#include <sys/epoll.h>

static upoll_t* ups[OPEN_MAX];
static int index = 1;

int
epoll_create(int size)
{
        if (index >= OPEN_MAX) {
                errno = ENFILE;
                return -1;
        }
        ups[index++] = upoll_create(size);
        return index - 1;
}

int
epoll_ctl(int epfd, int op, int fd, struct epoll_event *event)
{
        upoll_t* upq = ups[epfd];
        upoll_event_t* uevent = (upoll_event_t*) event;
        return upoll_ctl(upq, op, fd, uevent);
}

int
epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout)
{
        upoll_t* upq = ups[epfd];
        upoll_event_t* uevents = (upoll_event_t*) events;
        return upoll_wait(upq, uevents, maxevents, timeout);
}
