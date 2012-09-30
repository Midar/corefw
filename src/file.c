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

#include <string.h>
#include <limits.h>

#include <sys/stat.h>

#include <fcntl.h>
#include <unistd.h>

#include "stream.h"
#include "file.h"

#ifndef O_BINARY
# define O_BINARY 0
#endif

#ifndef S_IRGRP
# define S_IRGRP 0
#endif
#ifndef S_IROTH
# define S_IROTH 0
#endif
#ifndef S_IWGRP
# define S_IWGRP 0
#endif
#ifndef S_IWOTH
# define S_IWOTH 0
#endif

#define DEFAULT_MODE S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH

struct CFWFile {
	CFWStream stream;
	int fd;
	bool at_end;
};

static int
parse_mode(const char *mode)
{
	if (!strcmp(mode, "r"))
		return O_RDONLY;
	if (!strcmp(mode, "rb"))
		return O_RDONLY | O_BINARY;
	if (!strcmp(mode, "r+"))
		return O_RDWR;
	if (!strcmp(mode, "rb+") || !strcmp(mode, "r+b"))
		return O_RDWR | O_BINARY;
	if (!strcmp(mode, "w"))
		return O_WRONLY | O_CREAT | O_TRUNC;
	if (!strcmp(mode, "wb"))
		return O_WRONLY | O_CREAT | O_TRUNC | O_BINARY;
	if (!strcmp(mode, "w+"))
		return O_RDWR | O_CREAT | O_TRUNC;
	if (!strcmp(mode, "wb+") || !strcmp(mode, "w+b"))
		return O_RDWR | O_CREAT | O_TRUNC | O_BINARY;
	if (!strcmp(mode, "a"))
		return O_WRONLY | O_CREAT | O_APPEND;
	if (!strcmp(mode, "ab"))
		return O_WRONLY | O_CREAT | O_APPEND | O_BINARY;
	if (!strcmp(mode, "a+"))
		return O_RDWR | O_CREAT | O_APPEND;
	if (!strcmp(mode, "ab+") || !strcmp(mode, "a+b"))
		return O_RDWR | O_CREAT | O_APPEND | O_BINARY;

	return -1;
}

static ssize_t
file_read(void *ptr, void *buf, size_t len)
{
	CFWFile *file = ptr;
	ssize_t ret;

	if ((ret = read(file->fd, buf, len)) == 0)
		file->at_end = true;

	return ret;
}

static bool
file_write(void *ptr, const void *buf, size_t len)
{
	CFWFile *file = ptr;
	ssize_t ret;

	if ((ret = write(file->fd, buf, len)) < len)
		return false;

	return true;
}

static bool
file_at_end(void *ptr)
{
	CFWFile *file = ptr;

	return file->at_end;
}

static void
file_close(void *ptr)
{
	CFWFile *file = ptr;

	close(file->fd);
}

static struct cfw_stream_ops stream_ops = {
	.read = file_read,
	.write = file_write,
	.at_end = file_at_end,
	.close = file_close
};

static bool
ctor(void *ptr, va_list args)
{
	CFWFile *file = ptr;
	const char *path = va_arg(args, const char*);
	const char *mode = va_arg(args, const char*);
	int flags;

	/* Make sure we have a valid file in case we error out */
	cfw_stream->ctor(ptr, args);
	file->at_end = false;

	if ((flags = parse_mode(mode)) == -1)
		return false;

	if ((file->fd = open(path, flags, DEFAULT_MODE)) == -1)
		return false;

	file->stream.ops = &stream_ops;

	return true;
}

static void
dtor(void *ptr)
{
	cfw_stream->dtor(ptr);
}

static CFWClass class = {
	.name = "CFWFile",
	.size = sizeof(CFWFile),
	.ctor = ctor,
	.dtor = dtor
};
CFWClass *cfw_file = &class;

static CFWFile cfw_stdin_ = {
	.stream = {
		.obj = {
			.cls = &class,
			.ref_cnt = INT_MAX
		},
		.ops = &stream_ops
	},
	.fd = 0,
	.at_end = false
};
static CFWFile cfw_stdout_ = {
	.stream = {
		.obj = {
			.cls = &class,
			.ref_cnt = INT_MAX
		},
		.ops = &stream_ops
	},
	.fd = 1,
	.at_end = false
};
static CFWFile cfw_stderr_ = {
	.stream = {
		.obj = {
			.cls = &class,
			.ref_cnt = INT_MAX
		},
		.ops = &stream_ops
	},
	.fd = 2,
	.at_end = false
};
CFWFile *cfw_stdin = &cfw_stdin_;
CFWFile *cfw_stdout = &cfw_stdout_;
CFWFile *cfw_stderr = &cfw_stderr_;
