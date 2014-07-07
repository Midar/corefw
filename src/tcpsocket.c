/*
 * Copyright (c) 2012, Jonathan Schleifer <js@webkeks.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#define _C99_SOURCE
#define _POSIX_SOURCE

#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "stream.h"
#include "tcpsocket.h"

struct CFWTCPSocket {
	CFWStream stream;
	int fd;
	bool at_end;
};

static ssize_t
sock_read(void *ptr, void *buf, size_t len)
{
	CFWTCPSocket *sock = ptr;
	ssize_t ret;

	if ((ret = recv(sock->fd, buf, len, 0)) == 0)
		sock->at_end = true;

	return ret;
}

static bool
sock_write(void *ptr, const void *buf, size_t len)
{
	CFWTCPSocket *sock = ptr;
	ssize_t ret;

	if ((ret = send(sock->fd, buf, len, 0)) < len)
		return false;

	return true;
}

static bool
sock_at_end(void *ptr)
{
	CFWTCPSocket *sock = ptr;

	return sock->at_end;
}

static void
sock_close(void *ptr)
{
	CFWTCPSocket *sock = ptr;

	if (sock->fd != -1)
		close(sock->fd);
}

static struct cfw_stream_ops stream_ops = {
	.read = sock_read,
	.write = sock_write,
	.at_end = sock_at_end,
	.close = sock_close
};

static bool
ctor(void *ptr, va_list args)
{
	CFWTCPSocket *sock = ptr;

	cfw_stream->ctor(ptr, args);

	sock->fd = -1;
	sock->stream.ops = &stream_ops;
	sock->at_end = false;

	return true;
}

static void
dtor(void *ptr)
{
	cfw_stream->dtor(ptr);
}

bool
cfw_tcpsocket_connect(CFWTCPSocket *sock, const char *host, uint16_t port)
{
	struct addrinfo hints, *res, *res0;
	char portstr[7];

	if (sock->fd != -1)
		return false;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	snprintf(portstr, 7, "%" PRIu16, port);

	if (getaddrinfo(host, portstr, &hints, &res0))
		return false;

	for (res = res0; res != NULL; res = res->ai_next) {
		if ((sock->fd = socket(res->ai_family, res->ai_socktype,
		    res->ai_protocol)) == -1)
			continue;

		if (connect(sock->fd, res->ai_addr, res->ai_addrlen) == -1) {
			close(sock->fd);
			sock->fd = -1;
			continue;
		}

		break;
	}

	freeaddrinfo(res0);

	return (sock->fd != -1);
}

static CFWClass class = {
	.name = "CFWTCPSocket",
	.size = sizeof(CFWTCPSocket),
	.ctor = ctor,
	.dtor = dtor
};
CFWClass *cfw_tcpsocket = &class;
